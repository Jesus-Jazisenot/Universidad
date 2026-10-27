Diagramas de la practica.

  diagrama-conexion-l298n.jpeg - Arduino UNO R4 WiFi, puente H L298N,
    motorreductor, pilas 18650 y pulsador.
  circuito-tinkercad.jpeg - Circuito armado en Tinkercad: UNO R3, L293D,
    motor DC, pila de 9 V, pulsador y LCD 16x2 I2C.

Conexiones (armado real):
  Pilas + -> +12V del L298N, pilas - -> GND del L298N
  GND del L298N -> GND del Arduino (tierra comun, obligatoria)
  OUT1 / OUT2 -> motorreductor
  D9 -> ENA (jumper ENA quitado, PWM), D7 -> IN1, IN2 -> GND
  Jumper 5V-EN puesto; +5V del L298N sin conectar
  Pulsador entre D2 y GND (INPUT_PULLUP, sin resistencia)
