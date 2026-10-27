/*
  Practica 4.1.2: Control de arranque y paro con un boton, protegido contra rebotes y cortes de energia.
  Placa Arduino UNO R4 WiFi, puente H L298N, motorreductor y pulsador.

  Motor:  D9 (PWM) -> ENA del L298N (sin jumper ENA); D7 -> IN1; IN2 conectado a GND.
  Boton:  un extremo a D2 y el contrario a GND (INPUT_PULLUP, no lleva resistencia).
  Potencia: dos baterias 18650 en serie a +12V y GND del L298N; jumper 5V-EN colocado.
  El GND del L298N va al GND del Arduino (la tierra compartida es indispensable).

  Al presionar una vez el motor arranca con rampa (0 a 255 en 1.5 s); al presionar otra vez se detiene al instante.
  Protecciones:
    - Rebote: la clase Boton solo valida un cambio que permanece estable 25 ms (filtro basado en millis()).
    - Corte de energia: lo primero que hace setup() es apagar el motor, indica el reinicio en la matriz LED
      y en el Monitor Serie, y el motor solo vuelve a encender con una pulsacion nueva.

  Comandos disponibles en el Monitor Serie (115200 baudios, con o sin ajuste de linea):
    V n   ajusta la ventana del filtro a n ms (sirve para probar 2, 25 y 300 ms)
    C     reinicia a 0 el contador de pulsaciones
    G     cambia entre arranque con rampa y arranque directo (PWM a 255), para compararlos

  El programa no usa delay().
*/

#include <Arduino_LED_Matrix.h>

const uint8_t PIN_BOTON = 2;
const uint8_t PIN_IN1 = 7;
const uint8_t PIN_ENA = 9;                 // PWM que controla la velocidad del motor.
const uint32_t VENTANA_MS = 25;            // Tiempo que la senal debe mantenerse estable.
const uint32_t RAMPA_MS = 1500;            // Duracion de la rampa de 0 a 255.
const uint32_t AVISO_MS = 3000;            // Duracion del aviso de reinicio en la matriz.
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
ArduinoLEDMatrix matriz;

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


// ---------------------------------------------------------------- Matriz LED
// Letras de 3x5 pixeles; cada fila se guarda en 3 bits (el bit mayor corresponde a la columna izquierda).
const uint8_t LETRA_R[5] = {0b110, 0b101, 0b110, 0b101, 0b101};
const uint8_t LETRA_S[5] = {0b011, 0b100, 0b010, 0b001, 0b110};
const uint8_t LETRA_T[5] = {0b111, 0b010, 0b010, 0b010, 0b010};
const uint8_t LETRA_O[5] = {0b010, 0b101, 0b101, 0b101, 0b010};
const uint8_t LETRA_N[5] = {0b101, 0b111, 0b111, 0b101, 0b101};
const uint8_t LETRA_F[5] = {0b111, 0b100, 0b110, 0b100, 0b100};

void ponerLetra(uint8_t imagen[8][12], const uint8_t letra[5], uint8_t columna) {
  for (uint8_t fila = 0; fila < 5; ++fila)
    for (uint8_t c = 0; c < 3; ++c)
      if (letra[fila] & (0b100 >> c))
        imagen[fila + 1][columna + c] = 1;
}

void mostrarAviso() {                      // Muestra "RST" para indicar que hubo un reinicio.
  uint8_t imagen[8][12] = {};
  ponerLetra(imagen, LETRA_R, 1);
  ponerLetra(imagen, LETRA_S, 5);
  ponerLetra(imagen, LETRA_T, 9);
  matriz.renderBitmap(imagen, 8, 12);
}

// Muestra "ON" u "OFF" y, en la fila inferior, una barra de velocidad que crece durante la rampa.
void mostrarEstadoMotor() {
  static int ultimo = -1;
  int actual = (estado == APAGADO) ? -2 : pwm * 12 / 256;
  if (actual == ultimo) {
    return;                                // Si nada cambio, no se vuelve a dibujar.
  }
  ultimo = actual;
  uint8_t imagen[8][12] = {};
  if (estado == APAGADO) {
    ponerLetra(imagen, LETRA_O, 1);
    ponerLetra(imagen, LETRA_F, 5);
    ponerLetra(imagen, LETRA_F, 9);
  } else {
    ponerLetra(imagen, LETRA_O, 2);
    ponerLetra(imagen, LETRA_N, 6);
    for (int c = 0; c <= actual; ++c) {
      imagen[7][c] = 1;
    }
  }
  matriz.renderBitmap(imagen, 8, 12);
}

void actualizarMatriz() {
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
// no lo envia ("Sin ajuste de linea"), despues de 50 ms sin recibir nada.
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
  matriz.begin();
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
  actualizarMatriz();
  leerSerie();
}
