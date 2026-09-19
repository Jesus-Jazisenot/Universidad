Diagrama del circuito.

  diagrama-conexion.png  - Arduino UNO R4 WiFi + servomotor MG996R. Senal (naranja)
                           del pin 9 al servo; alimentacion (rojo) desde una fuente
                           externa de 5-6 V (4xAA o powerbank), nunca del pin 5V del
                           Arduino; tierra comun (negro) entre Arduino, fuente y
                           servo.   [LISTO]

Conexion:
  Servo senal    pin 9 (PWM) del Arduino
  Servo VCC      fuente externa 5-6 V (NO el 5V del Arduino: el MG996R consume mas
                 corriente de la que el pin puede entregar)
  Servo GND      mismo nodo que GND del Arduino y GND de la fuente externa
