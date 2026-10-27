/*
  Practica 4.1.2: Boton de arranque/paro a prueba de rebotes y apagones.
  VERSION PARA TINKERCAD: Arduino UNO R3 + puente H L293D + motor DC + pulsador + LCD 16x2 (I2C).
  Tinkercad no tiene el UNO R4 (ni su matriz LED) ni el modulo L298N: la matriz se cambia
  por la LCD y el L298N por el L293D, que se controla igual (EN con PWM, IN1 e IN2).

  Motor:  D9 (PWM) -> EN1,2 (pata 1 del L293D); D7 -> IN1 (pata 2); IN2 (pata 7) a GND.
          Motor entre OUT1 (pata 3) y OUT2 (pata 6). Patas 4, 5, 12 y 13 a GND.
  Boton:  una pata a D2 y la opuesta a GND (INPUT_PULLUP, sin resistencia).
  LCD:    GND, VCC a 5V, SDA a A4, SCL a A5.
  Potencia: pila de 9 V a VCC2 (pata 8) del L293D; VCC1 (pata 16) a 5V del Arduino.
  Negativo de la pila unido al GND del Arduino (tierra comun obligatoria).

  Una pulsacion arranca el motor con rampa (0 a 255 en 1.5 s); la siguiente lo para de inmediato.
  Defensas:
    - Rebote: la clase Boton solo acepta un cambio que se queda quieto 25 ms (filtro con millis()).
    - Apagon: setup() deja el motor apagado antes que nada, avisa el reinicio en la LCD
      y en el Monitor Serie, y el motor solo arranca con una pulsacion nueva.

  Comandos por el Monitor Serie (115200 baudios, cualquier ajuste de linea):
    V n   cambia la ventana del filtro a n ms (para la prueba de 2, 25 y 300 ms)
    C     pone el contador de pulsaciones en 0
    G     alterna el arranque entre rampa y golpe (PWM directo a 255), para compararlos

  No utiliza delay().
*/

#include <Adafruit_LiquidCrystal.h>

const uint8_t PIN_BOTON = 2;
const uint8_t PIN_IN1 = 7;
const uint8_t PIN_ENA = 9;                 // PWM: velocidad del motor.
const uint32_t VENTANA_MS = 25;            // Tiempo que la senal debe quedarse quieta.
const uint32_t RAMPA_MS = 1500;            // De 0 a 255 en este tiempo.
const uint32_t AVISO_MS = 3000;            // Cuanto dura el aviso de reinicio en la LCD.
const uint32_t FIN_DE_ORDEN_MS = 50;       // Silencio que cierra una orden sin fin de linea.
const uint8_t LARGO_ORDEN = 12;


// ---------------------------------------------------------------- Filtro de rebotes
class Boton {
  public:
    Boton(uint8_t pin, uint32_t ventana = VENTANA_MS)
      : pin_(pin), lecturaAnterior_(HIGH), estadoAceptado_(HIGH),
        ultimoCambio_(0), ventana_(ventana), huboPulsacion_(false) {}

    // Si el boton ya esta oprimido al arrancar, se toma como el estado de partida:
    // asi un dedo que se quedo en el boton durante el reinicio no arranca el motor.
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
        ultimoCambio_ = ahora;                // Cada rebote reinicia el cronometro.
      }
      if (ahora - ultimoCambio_ >= ventana_ && lectura != estadoAceptado_) {
        estadoAceptado_ = lectura;
        if (estadoAceptado_ == LOW) {
          huboPulsacion_ = true;              // Solo cuenta el paso de suelto a oprimido.
        }
      }
      lecturaAnterior_ = lectura;
    }

    // Entrega cada pulsacion real una sola vez.
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

void pararMotor() {                        // Paro inmediato.
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
    escribirPwm(255);                      // De golpe: solo para comparar.
    estado = ENCENDIDO;
  }
}

// Sube el PWM en proporcion al tiempo transcurrido desde que empezo la rampa.
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


// ---------------------------------------------------------------- LCD (en lugar de la matriz)
void mostrarAviso() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("*** SISTEMA ***");
  lcd.setCursor(0, 1);
  lcd.print("  REINICIADO");
}

// Linea 1: estado del motor. Linea 2: barra con la velocidad (sube durante la rampa).
void mostrarEstadoMotor() {
  static int ultimoEstado = -1;
  static int ultimaBarra = -1;
  int barra = (estado == APAGADO) ? 0 : (pwm * 16 + 255) / 256;
  if ((int)estado == ultimoEstado && barra == ultimaBarra) {
    return;                                // Sin cambios: no se redibuja.
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

// Junta los caracteres sin bloquear. La orden termina con fin de linea o, si el monitor
// no lo manda (Tinkercad), tras 50 ms sin recibir caracteres.
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
  // PRIMERA ACCION: motor apagado (PWM en 0 e IN1 en LOW), pase lo que pase antes del reinicio.
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
    // Espera al Monitor Serie hasta 2 s, sin delay(); el motor ya esta apagado.
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
