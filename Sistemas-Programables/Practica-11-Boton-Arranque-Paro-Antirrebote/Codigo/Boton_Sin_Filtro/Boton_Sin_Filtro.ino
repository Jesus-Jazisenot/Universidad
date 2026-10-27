/*
  Practica 4.1.2 (paso 2 del desarrollo): version SIN filtro antirrebote.
  Placa Arduino UNO R4 WiFi, puente H L298N, motorreductor y pulsador.

  Se conecta igual que Boton_Arranque_Paro: D9 -> ENA, D7 -> IN1, IN2 a GND, boton entre D2 y GND.

  Lee el boton sin filtrar: toda transicion de HIGH (suelto) a LOW (presionado) que detecta
  el Arduino se toma como pulsacion y cambia el motor, rebotes incluidos. Se usa en la prueba
  de rebote: presionar 10 veces con calma y comparar contra lo que indica el contador.

  Comando disponible en el Monitor Serie (115200 baudios, con ajuste de linea):
    C     reinicia el contador a 0 (antes de cada intento)

  El programa no usa delay().
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
  digitalWrite(PIN_ENA, LOW);                // Lo primero: motor apagado.
  digitalWrite(PIN_IN1, LOW);
  pinMode(PIN_ENA, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  escribirMotor(false);
  pinMode(PIN_BOTON, INPUT_PULLUP);
  lecturaAnterior = digitalRead(PIN_BOTON);

  Serial.begin(115200);
  uint32_t inicio = millis();
  while (!Serial && millis() - inicio < 2000) {
    // Espera hasta 2 s a que abra el Monitor Serie, sin delay().
  }
  Serial.println();
  Serial.println("Practica 4.1.2: version SIN filtro de rebotes");
  Serial.println("Oprima el boton 10 veces, despacio. C = contador a 0.");
}

void loop() {
  uint8_t lectura = digitalRead(PIN_BOTON);
  if (lecturaAnterior == HIGH && lectura == LOW) {   // De suelto a presionado (sin filtro).
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
