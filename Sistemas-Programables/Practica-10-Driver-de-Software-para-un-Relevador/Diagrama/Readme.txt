Diagramas de la practica.

  diagrama-conexion-relevador.png - Hecho por Jesus: Arduino (dibujado como el
    UNO R3 de Tinkercad, mismos pines que la R4 WiFi), L298N con motorreductor
    TT y pilas 2x18650, y LED activo en bajo en protoboard.              [LISTO]
  diagrama-estados-relevador.png - Estados APAGADO <-> ENCENDIDO, estado
    inicial (iniciar()/RESET) y regla del tiempo minimo de 1000 ms.       [LISTO]

Conexiones:
  Pilas + -> +12V del L298N, pilas - -> GND del L298N
  GND del L298N -> GND del Arduino (tierra comun, obligatoria)
  OUT1 / OUT2 -> motorreductor
  D7 -> IN1, IN2 -> GND; jumpers de ENA y 5V-EN puestos; +5V del L298N sin conectar
  5V del Arduino -> anodo del LED; catodo -> 220 ohm -> D6

Generador del diagrama de estados: C:/Users/jesus/Escuela/SistemasProgramables/generadores/diagrama_relevador.py
