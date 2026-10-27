/*
  Practica 4.1.2: Control de arranque y paro con un boton, protegido contra rebotes y cortes de energia.
  ADAPTACION PARA TINKERCAD: Arduino UNO R3, puente H L293D, motor de CD, pulsador y LCD 16x2 (I2C).
  Como Tinkercad no incluye el UNO R4 (ni su matriz LED) ni el modulo L298N, la matriz se
  sustituye por la LCD y el L298N por el L293D, que se maneja igual (EN con PWM, IN1 e IN2).

  Motor:  D9 (PWM) -> EN1,2 (pin 1 del L293D); D7 -> IN1 (pin 2); IN2 (pin 7) a GND.
          Motor conectado entre OUT1 (pin 3) y OUT2 (pin 6). Pines 4, 5, 12 y 13 a GND.
  Boton:  un extremo a D2 y el contrario a GND (INPUT_PULLUP, no lleva resistencia).
  LCD:    GND, VCC a 5V, SDA a A4, SCL a A5.
  Potencia: bateria de 9 V a VCC2 (pin 8) del L293D; VCC1 (pin 16) a 5V del Arduino.
  El negativo de la bateria va al GND del Arduino (la tierra compartida es indispensable).

  Al presionar una vez el motor arranca con rampa (0 a 255 en 1.5 s); al presionar otra vez se detiene al instante.
  Protecciones:
    - Rebote: la clase Boton solo valida un cambio que permanece estable 25 ms (filtro basado en millis()).
    - Corte de energia: lo primero que hace setup() es apagar el motor, indica el reinicio en la LCD
      y en el Monitor Serie, y el motor solo vuelve a encender con una pulsacion nueva.

  Comandos disponibles en el Monitor Serie (115200 baudios, con o sin ajuste de linea):
    V n   ajusta la ventana del filtro a n ms (sirve para probar 2, 25 y 300 ms)
    C     reinicia a 0 el contador de pulsaciones
    G     cambia entre arranque con rampa y arranque directo (PWM a 255), para compararlos

  El programa no usa delay().
*/

#include <Adafruit_LiquidCrystal.h>

const uint8_t PIN_BOTON = 2;
const uint8_t PIN_IN1 = 7;
const uint8_t PIN_ENA = 9;                 // PWM que controla la velocidad del motor.
const uint32_t VENTANA_MS = 25;            // Tiempo que la senal debe mantenerse estable.
const uint32_t RAMPA_MS = 1500;            // Duracion de la rampa de 0 a 255.
const uint32_t AVISO_MS = 3000;            // Duracion del aviso de reinicio en la LCD.
const uint32_t FIN_DE_ORDEN_MS = 50;       // Pausa que da por terminada una orden sin fin de linea.
const uint8_t LARGO_ORDEN = 12;


// ---------------------------------------------------------------- Filtro antirrebote
class Boton {
  public:
    Boton(uint8_t pin, uint32_t ventana = VENTANA_MS)
      : pin_(pin), lecturaAnterior_(HIGH), estadoAceptado_(HIGH),
        ultimoCambio_(0), ventana_(ventana), huboPulsacion_(false) {}

    // La lectura al arrancar se usa como estado inicial: si el boton ya estaba
    // presionado durante el reinicio, eso no cuenta como pulsacion y el motor no enciende.
    void iniciar() {
      pinMode(pin_, INPUT_PULLUP);
      lecturaAnterior_ = estadoAceptado_ = digitalRead(pin_);
      ultimoCambio_ = millis();
      huboPulsacion_ = false;
    }

    void actualizar() {
      uint8_t lectura = digitalRead(pin_);
      uint32_t ahora = millis();
      if (lectura != lecturaAnterior_) {
        ultimoCambio_ = ahora;                // Cualquier rebote vuelve a poner el cronometro en cero.
      }
      if (ahora - ultimoCambio_ >= ventana_ && lectura != estadoAceptado_) {
        estadoAceptado_ = lectura;
        if (estadoAceptado_ == LOW) {
          huboPulsacion_ = true;              // Solo se registra la transicion de suelto a presionado.
        }
      }
      lecturaAnterior_ = lectura;
    }

    // Devuelve cada pulsacion real una unica vez.
    bool fuePresionado() {
      if (huboPulsacion_) {
        huboPulsacion_ = false;
        return true;
      }
      return false;
    }

    void cambiarVentana(uint32_t ventana) { ventana_ = ventana; }
    uint32_t ventana() const { return ventana_; }

  private:
    uint8_t pin_;
    uint8_t lecturaAnterior_;
    uint8_t estadoAceptado_;
    uint32_t ultimoCambio_;
    uint32_t ventana_;
    bool huboPulsacion_;
};


// ---------------------------------------------------------------- Motor
enum EstadoMotor { APAGADO, ARRANCANDO, ENCENDIDO };

Boton boton(PIN_BOTON);
Adafruit_LiquidCrystal lcd(0);

EstadoMotor estado = APAGADO;
uint8_t pwm = 0;
uint32_t inicioRampa = 0;
bool arranqueConRampa = true;
uint32_t pulsaciones = 0;
uint32_t finAviso = 0;
bool avisoVisible = true;

void escribirPwm(uint8_t valor) {
  pwm = valor;
  analogWrite(PIN_ENA, pwm);
}

void pararMotor() {                        // Detiene el motor al instante.
  escribirPwm(0);
  digitalWrite(PIN_IN1, LOW);
  estado = APAGADO;
}

void arrancarMotor() {
  digitalWrite(PIN_IN1, HIGH);
  if (arranqueConRampa) {
    inicioRampa = millis();
    escribirPwm(0);
    estado = ARRANCANDO;
  } else {
    escribirPwm(255);                      // Arranque directo: solo para comparar.
    estado = ENCENDIDO;
  }
}

// Aumenta el PWM segun el tiempo que ha pasado desde el inicio de la rampa.
void actualizarRampa() {
  if (estado != ARRANCANDO) {
    return;
  }
  uint32_t transcurrido = millis() - inicioRampa;
  if (transcurrido >= RAMPA_MS) {
    escribirPwm(255);
    estado = ENCENDIDO;
    Serial.println("Rampa terminada: motor a velocidad maxima (PWM 255)");
    return;
  }
  uint8_t valor = (uint8_t)(255UL * transcurrido / RAMPA_MS);
  if (valor != pwm) {
    escribirPwm(valor);
  }
}


// ---------------------------------------------------------------- LCD (sustituye a la matriz)
void mostrarAviso() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("*** SISTEMA ***");
  lcd.setCursor(0, 1);
  lcd.print("  REINICIADO");
}

// Renglon 1: estado del motor. Renglon 2: barra de velocidad que crece durante la rampa.
void mostrarEstadoMotor() {
  static int ultimoEstado = -1;
  static int ultimaBarra = -1;
  int barra = (estado == APAGADO) ? 0 : (pwm * 16 + 255) / 256;
  if ((int)estado == ultimoEstado && barra == ultimaBarra) {
    return;                                // Si nada cambio, no se vuelve a dibujar.
  }
  if ((int)estado != ultimoEstado) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motor: ");
    lcd.print(estado == APAGADO ? "OFF" : (estado == ARRANCANDO ? "RAMPA" : "ON"));
    ultimaBarra = 0;
  }
  lcd.setCursor(ultimaBarra, 1);
  for (int c = ultimaBarra; c < barra; ++c) {
    lcd.print('#');
  }
  ultimoEstado = (int)estado;
  ultimaBarra = barra;
}

void actualizarPantalla() {
  if (avisoVisible) {
    if (millis() < finAviso) {
      return;
    }
    avisoVisible = false;
  }
  mostrarEstadoMotor();
}


// ---------------------------------------------------------------- Monitor Serie
char orden[LARGO_ORDEN + 1];
uint8_t largo = 0;
uint32_t ultimoCaracter = 0;

const char *textoEstado() {
  if (estado == ARRANCANDO) return "ARRANCANDO (rampa)";
  return estado == ENCENDIDO ? "ENCENDIDO" : "APAGADO";
}

void ejecutar(char *texto) {
  if (texto[0] == 'V') {
    long ms = atol(texto + 1);
    if (ms <= 0) {
      Serial.println("Uso: V n  (n = ventana en ms, por ejemplo V 2, V 25 o V 300)");
      return;
    }
    boton.cambiarVentana((uint32_t)ms);
    Serial.print("Ventana del filtro: ");
    Serial.print(boton.ventana());
    Serial.println(" ms");
  } else if (strcmp(texto, "C") == 0) {
    pulsaciones = 0;
    Serial.println("Contador en 0");
  } else if (strcmp(texto, "G") == 0) {
    arranqueConRampa = !arranqueConRampa;
    Serial.print("Arranque: ");
    Serial.println(arranqueConRampa ? "con rampa (1.5 s)" : "de golpe (PWM directo a 255)");
  } else if (texto[0] != '\0') {
    Serial.print("Comando desconocido: ");
    Serial.print(texto);
    Serial.println("  (use V n, C o G)");
  }
}

void cerrarOrden() {
  orden[largo] = '\0';
  ejecutar(orden);
  largo = 0;
}

// Acumula los caracteres sin bloquear. Una orden termina con fin de linea o, cuando el monitor
// no lo envia (Tinkercad), despues de 50 ms sin recibir nada.
void leerSerie() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    ultimoCaracter = millis();
    if (c == '\n' || c == '\r') {
      cerrarOrden();
    } else if (c != ' ' && largo < LARGO_ORDEN) {
      orden[largo++] = (char)toupper((unsigned char)c);
    }
  }
  if (largo > 0 && millis() - ultimoCaracter >= FIN_DE_ORDEN_MS) {
    cerrarOrden();
  }
}


// ---------------------------------------------------------------- Programa principal
void setup() {
  // LO PRIMERO: apagar el motor (PWM en 0 e IN1 en LOW), sin importar lo que pasaba antes del reinicio.
  digitalWrite(PIN_ENA, LOW);
  digitalWrite(PIN_IN1, LOW);
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  pararMotor();

  boton.iniciar();
  lcd.begin(16, 2);
  lcd.setBacklight(1);
  mostrarAviso();
  finAviso = millis() + AVISO_MS;

  Serial.begin(115200);
  uint32_t inicio = millis();
  while (!Serial && millis() - inicio < 2000) {
    // Espera hasta 2 s a que abra el Monitor Serie, sin delay(); el motor ya esta detenido.
  }
  Serial.println();
  Serial.println("*** SISTEMA REINICIADO ***");
  Serial.println("Practica 4.1.2: boton de arranque/paro a prueba de rebotes y apagones");
  Serial.println("Motor APAGADO (estado seguro). Oprima el boton para arrancar.");
  Serial.print("Ventana del filtro: ");
  Serial.print(boton.ventana());
  Serial.println(" ms   |   Comandos: V n (ventana), C (contador a 0), G (rampa/golpe)");
}

void loop() {
  boton.actualizar();
  if (boton.fuePresionado()) {
    pulsaciones++;
    if (estado == APAGADO) {
      arrancarMotor();
    } else {
      pararMotor();
    }
    Serial.print("Pulsacion #");
    Serial.print(pulsaciones);
    Serial.print(" -> Motor ");
    Serial.println(textoEstado());
  }
  actualizarRampa();
  actualizarPantalla();
  leerSerie();
}
