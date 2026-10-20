// Practica 4.1.1-A - Driver de software para un relevador
// Arduino UNO R4 WiFi
//
// Una sola clase (Relevador) controla dos actuadores distintos:
//   - Motor: canal A del L298N, IN1 en D7, ACTIVO EN ALTO
//   - LED:   anodo a 5V, catodo -> 220 ohm -> D6, ACTIVO EN BAJO
//             (imita al optoacoplador de un modulo de relevador real)
//
// Comandos por el Monitor Serie (115200 baudios, cualquier fin de linea):
//   M1 / M0  encender / apagar el motor
//   L1 / L0  encender / apagar el LED
//   E        estado de ambos
//
// Reglas: el programa principal nunca usa digitalWrite(); todo pasa por
// la clase. Sin delay(). Entre dos cambios de un mismo relevador debe
// pasar al menos 1000 ms; si una orden se rechaza, el pin no cambia.

// (PIN_LED ya existe en el core de la R4: es el LED de la placa, D13)
const uint8_t PIN_MOTOR_IN1 = 7;
const uint8_t PIN_LED_OPTO = 6;
const unsigned long TIEMPO_MINIMO_MS = 1000;

class Relevador {
public:
  Relevador(uint8_t pin, bool activoEnBajo, unsigned long tiempoMinimo)
    : pin_(pin), activoEnBajo_(activoEnBajo), encendido_(false),
      ultimoCambio_(0), tiempoMinimo_(tiempoMinimo) {}

  // Estado seguro: se escribe el nivel de APAGADO antes de volver el pin
  // salida, asi el actuador no da ni un pulso al arrancar o tras un RESET.
  // El arranque cuenta como un cambio: la primera orden espera 1 s.
  void iniciar() {
    encendido_ = false;
    escribirPin(false);
    pinMode(pin_, OUTPUT);
    escribirPin(false);
    ultimoCambio_ = millis();
  }

  bool encender() { return cambiar(true); }
  bool apagar() { return cambiar(false); }
  bool estaEncendido() const { return encendido_; }

  // Milisegundos que faltan para aceptar otro cambio (0 = ya se puede).
  unsigned long tiempoRestante() const {
    unsigned long transcurrido = millis() - ultimoCambio_;
    return transcurrido >= tiempoMinimo_ ? 0 : tiempoMinimo_ - transcurrido;
  }

private:
  bool cambiar(bool nuevo) {
    if (nuevo == encendido_) return true;   // ya estaba asi: no hay conmutacion
    if (tiempoRestante() > 0) return false; // muy pronto: el pin no se toca
    encendido_ = nuevo;
    escribirPin(nuevo);
    ultimoCambio_ = millis();
    return true;
  }

  // Unico lugar donde se decide HIGH o LOW.
  void escribirPin(bool encendido) {
    bool nivel = activoEnBajo_ ? !encendido : encendido;
    digitalWrite(pin_, nivel ? HIGH : LOW);
  }

  const uint8_t pin_;
  const bool activoEnBajo_;
  bool encendido_;
  unsigned long ultimoCambio_;
  const unsigned long tiempoMinimo_;
};

Relevador motor(PIN_MOTOR_IN1, false, TIEMPO_MINIMO_MS);  // activo en alto
Relevador led(PIN_LED_OPTO, true, TIEMPO_MINIMO_MS);       // activo en bajo

char letraPendiente = 0;  // 'M' o 'L' esperando su 0/1

void setup() {
  motor.iniciar();  // primero los actuadores: estado seguro antes que nada
  led.iniciar();

  Serial.begin(115200);
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 2000) {}  // espera al USB sin bloquear de mas

  Serial.println();
  Serial.println("Driver de relevador listo. Motor y LED APAGADOS (estado seguro).");
  Serial.println("Comandos: M1/M0 motor, L1/L0 LED, E estado.");
}

void loop() {
  while (Serial.available() > 0) {
    procesarCaracter(Serial.read());
  }
}

void procesarCaracter(char c) {
  if (c >= 'a' && c <= 'z') c -= 'a' - 'A';
  if (c == '\r' || c == '\n' || c == ' ') return;

  if (letraPendiente != 0) {
    char letra = letraPendiente;
    letraPendiente = 0;
    if (c == '0' || c == '1') {
      if (letra == 'M') ejecutar(motor, "Motor", c == '1');
      else ejecutar(led, "LED", c == '1');
      return;
    }
    Serial.print("Comando no reconocido: ");
    Serial.println(letra);
  }

  if (c == 'M' || c == 'L') {
    letraPendiente = c;
  } else if (c == 'E') {
    mostrarEstado();
  } else {
    Serial.print("Comando no reconocido: ");
    Serial.println(c);
  }
}

void ejecutar(Relevador &r, const char *nombre, bool encender) {
  bool yaEstaba = r.estaEncendido() == encender;
  unsigned long falta = r.tiempoRestante();
  bool ok = encender ? r.encender() : r.apagar();

  if (!ok) {
    Serial.print("Orden rechazada: espere 1 s entre cambios (");
    Serial.print(nombre);
    Serial.print(" sigue ");
    Serial.print(r.estaEncendido() ? "ENCENDIDO" : "APAGADO");
    Serial.print(", faltan ");
    Serial.print(falta);
    Serial.println(" ms)");
    return;
  }
  Serial.print(nombre);
  Serial.print(yaEstaba ? " ya estaba " : " ");
  Serial.println(r.estaEncendido() ? "ENCENDIDO" : "APAGADO");
}

void mostrarEstado() {
  Serial.print("Estado -> Motor: ");
  Serial.print(motor.estaEncendido() ? "ENCENDIDO" : "APAGADO");
  Serial.print(" | LED: ");
  Serial.println(led.estaEncendido() ? "ENCENDIDO" : "APAGADO");
}
