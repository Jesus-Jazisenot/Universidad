# Nombre del proyecto
Control de un servomotor mediante una interfaz web

## Descripción
Un Arduino UNO R4 WiFi levanta un servidor web que sirve una página con un control
deslizante (slider). Al mover el slider desde el navegador, el Arduino recibe el
ángulo pedido por HTTP y mueve el servomotor a esa posición en tiempo real, sin
recargar la página.

## Objetivos
- Levantar un servidor HTTP en un microcontrolador con Wi-Fi integrado.
- Construir una interfaz web simple (HTML + JavaScript) sin depender de un
  servidor externo.
- Controlar la posición de un servomotor a partir de un valor recibido por HTTP.
- Actualizar el control en tiempo real sin recargar la página (`fetch`).

## Herramientas y material utilizado
- Arduino UNO R4 WiFi
- Servomotor
- Protoboard y cables Dupont
- Fuente externa para el servomotor (si es de torque alto, tipo MG996R)
- Arduino IDE, librería `WiFiS3` (incluida en el core) y librería `Servo`
  (se instala aparte, ver [Codigo/Readme.txt](Codigo/Readme.txt))

## Diagrama
El servomotor va en el pin 9 (PWM); si requiere más corriente de la que da el
Arduino, se alimenta con fuente externa y tierra común.

**PENDIENTE:** falta armar el circuito y subir el esquema. Ver
[Diagrama/Readme.txt](Diagrama/Readme.txt).

[Ver carpeta Diagrama](Diagrama/)

## Código
El programa levanta un servidor HTTP en el puerto 80. Al recibir
`GET /servo?angulo=NN`, mueve el servo a ese ángulo y responde con la página que
contiene el slider. Compilado y verificado con `arduino-cli` para la placa
`arduino:renesas_uno:unor4wifi` (24 % de memoria de programa); falta la prueba en
hardware real.

[Ver código](Codigo/ControlServoWeb.ino)

## Reporte
**PENDIENTE:** subir el PDF con metodología, capturas de la interfaz web
funcionando y conclusiones técnicas. Ver [Reporte/Readme.txt](Reporte/Readme.txt).

## Resultados
**PENDIENTE:** documentar el ángulo alcanzado contra el pedido y el tiempo de
respuesta de la interfaz. Ver [Resultados/Readme.txt](Resultados/Readme.txt).

## Video
**PENDIENTE:** grabar el video del servomotor respondiendo a la interfaz web. Ver
[Video/Readme.txt](Video/Readme.txt).

## Conclusiones
**PENDIENTE:** redactar una vez armado y probado el circuito.
