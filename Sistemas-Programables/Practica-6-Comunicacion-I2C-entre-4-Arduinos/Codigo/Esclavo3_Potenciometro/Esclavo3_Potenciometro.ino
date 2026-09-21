// Esclavo 3 (0x0A) - lee un potenciometro y envia su valor (0-1023) al
// maestro cuando este lo pide, en 2 bytes.
#include <Wire.h>

void setup() {
  Wire.begin(0x0A);
  Wire.onRequest(enviar);
}

void loop() {
}

void enviar() {
  int valor = analogRead(A0);   // 0 a 1023
  Wire.write(highByte(valor));  // se manda en 2 bytes
  Wire.write(lowByte(valor));
}
