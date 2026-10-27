/*
  Practica 4.1.2 (paso 2 del desarrollo): version SIN filtro de rebotes.
  Arduino UNO R4 WiFi + modulo puente H L298N + motorreductor + pulsador.

  Mismas conexiones que Boton_Arranque_Paro: D9 -> ENA, D7 -> IN1, IN2 a GND, boton entre D2 y GND.

  Lee el boton directamente: cada paso de HIGH (suelto) a LOW (oprimido) que ve el Arduino
  cuenta como pulsacion y cambia el motor, incluidos los rebotes. Sirve para la prueba de
  rebote: oprimir 10 veces despacio y comparar con lo que marca el contador.

  Comandos por el Monitor Serie (115200 baudios, con ajuste de linea):
    C     pone el contador en 0 (antes de cada intento)

  No utiliza delay().
*/

const uint8_t PIN_BOTON = 2;
const uint8_t PIN_IN1 = 7;
const uint8_t PIN_ENA = 9;

uint8_t lecturaAnterior = HIGH;
uint32_t pulsaciones = 0;
bool motorEncendido = false;

void escribirMotor(bool encender) {
  motorEncendido = encender;
  digitalWrite(PIN_IN1, encender ? HIGH : LOW);
  analogWrite(PIN_ENA, encender ? 255 : 0);
}

void setup() {
  digitalWrite(PIN_ENA, LOW);                // Motor apagado antes que nada.
  digitalWrite(PIN_IN1, LOW);
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  escribirMotor(false);
  pinMode(PIN_BOTON, INPUT_PULLUP);
  lecturaAnterior = digitalRead(PIN_BOTON);

  Serial.begin(115200);
  uint32_t inicio = millis();
  while (!Serial && millis() - inicio < 2000) {
    // Espera al Monitor Serie hasta 2 s, sin delay().
  }
  Serial.println();
  Serial.println("Practica 4.1.2: version SIN filtro de rebotes");
  Serial.println("Oprima el boton 10 veces, despacio. C = contador a 0.");
}

void loop() {
  uint8_t lectura = digitalRead(PIN_BOTON);
  if (lecturaAnterior == HIGH && lectura == LOW) {   // Suelto -> oprimido (sin filtrar).
    pulsaciones++;
    escribirMotor(!motorEncendido);
    Serial.print("Pulsaciones contadas: ");
    Serial.print(pulsaciones);
    Serial.print(" -> Motor ");
    Serial.println(motorEncendido ? "ENCENDIDO" : "APAGADO");
  }
  lecturaAnterior = lectura;

  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == 'C' || c == 'c') {
      pulsaciones = 0;
      Serial.println("Contador en 0");
    }
  }
}
