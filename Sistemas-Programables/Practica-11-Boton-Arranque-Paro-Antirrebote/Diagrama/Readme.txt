Diagramas de la practica.

  diagrama-conexion-l298n.jpeg - Conexion del Arduino UNO R4 WiFi con el puente
    H L298N, el motorreductor, las baterias 18650 y el pulsador.
  circuito-tinkercad.jpeg - Circuito montado en Tinkercad: UNO R3, L293D,
    motor de CD, bateria de 9 V, pulsador y LCD 16x2 I2C.

Conexiones (montaje fisico):
  Baterias + -> +12V del L298N, baterias - -> GND del L298N
  GND del L298N -> GND del Arduino (tierra compartida, indispensable)
  OUT1 / OUT2 -> motorreductor
  D9 -> ENA (sin jumper ENA, control por PWM), D7 -> IN1, IN2 -> GND
  Jumper 5V-EN colocado; +5V del L298N libre
  Pulsador entre D2 y GND (INPUT_PULLUP, no requiere resistencia)
