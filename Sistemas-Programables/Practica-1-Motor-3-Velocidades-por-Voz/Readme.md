# Nombre del proyecto
Control de velocidad de un motorreductor en 3 niveles por comandos de voz

## Descripción
Una aplicación Android hecha en MIT App Inventor reconoce comandos de voz en español
y controla la velocidad de un motorreductor de CD en tres niveles, mediante un
Arduino UNO R4 WiFi y un puente H L298N. La app envía el nivel absoluto deseado (no
incrementos) por HTTP, y el Arduino aplica el PWM correspondiente.

## Objetivos
- Variar la velocidad de un motor de CD modulando el PWM en el pin ENA de un puente H.
- Controlar una carga de potencia desde un microcontrolador usando un L298N con
  fuente independiente y tierra común.
- Diseñar un protocolo HTTP que envíe estado absoluto para tolerar peticiones perdidas.
- Diagnosticar una falla eléctrica (batería descargada) frente a una falla mecánica
  aparente.

## Herramientas y material utilizado
- Arduino UNO R4 WiFi
- Puente H L298N
- Motorreductor de CD
- Batería LiPo 3S (11.1 V)
- Cables Dupont macho-hembra
- Teléfono Android con MIT AI2 Companion
- Arduino IDE / MIT App Inventor

## Diagrama
El diagrama muestra la conexión entre el Arduino UNO R4 WiFi, el L298N, el
motorreductor y la batería LiPo, indicando la tierra común y la fuente externa.

![Diagrama de conexión](Diagrama/conexion-motor-ilustrado.jpg)

## Código
El programa recibe el nivel de velocidad por HTTP (`/vel/0` a `/vel/3`) y lo traduce
a un valor de PWM aplicado al puente H. La app de App Inventor envía el nivel
absoluto y muestra el estado recibido.

[Ver código](Codigo/ControlMotor.ino) · [Ver app (App Inventor)](Codigo/AppInventor)
· [Guía de hardware](Guia-Hardware.md)

## Reporte
El reporte contiene la metodología, la tabla de conexiones, las capturas de
evidencia y las conclusiones técnicas de la práctica.

[Ver Reporte](Reporte/Reporte%20de%20la%20practica.pdf)

## Resultados
El motor respondió correctamente a los tres niveles de PWM y a los comandos de voz.
También se documentó el caso en que el motor no arrancaba en el nivel 1, causado por
una batería descargada y no por fricción mecánica.

## Video
El video muestra el funcionamiento del control de velocidad por voz sobre el
motorreductor.

[Ver video](https://youtu.be/qG1_gLGx5YU) · [Ver carpeta Video](Video/)

## Conclusiones
Enviar el nivel absoluto en vez de incrementos evita que la app y el motor se
desincronicen si se pierde una petición por la red. La tierra común entre el
Arduino y la fuente del motor, y quitar el jumper de ENA, son dos condiciones
invisibles en el código pero indispensables para que el circuito responda.

El diagnóstico inicial (fricción del motorreductor) resultó incorrecto: la causa
real era la batería descargada, lo que reducía tanto el voltaje como el par
disponible en los niveles bajos de PWM. La práctica dejó claro que antes de ajustar
el código hay que descartar la alimentación.
