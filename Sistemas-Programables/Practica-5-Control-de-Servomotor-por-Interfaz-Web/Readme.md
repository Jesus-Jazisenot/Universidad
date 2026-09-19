# Nombre del proyecto
Control de un servomotor mediante una interfaz web

## Descripción
Un Arduino UNO R4 WiFi levanta un servidor web que sirve una página con un control
deslizante y tres botones de posición fija (0°, 90°, 180°). Al usarlos desde el
navegador, el Arduino recibe el ángulo pedido por HTTP (`/set?a=NN`) y mueve el
servomotor de inmediato, sin recargar la página.

## Objetivos
- Levantar un servidor HTTP en un microcontrolador con Wi-Fi integrado.
- Construir una interfaz web simple (HTML + JavaScript) sin depender de un
  servidor externo.
- Controlar la posición de un servomotor a partir de un valor recibido por HTTP.
- Actualizar el control en tiempo real sin recargar la página (`fetch`), con un
  pequeño debounce para no saturar la red al arrastrar el slider.

## Herramientas y material utilizado
- Arduino UNO R4 WiFi
- Servomotor MG996R
- Fuente externa 5-6 V (4×AA o powerbank) para el servo
- Protoboard y cables Dupont
- Arduino IDE, librería `WiFiS3` (incluida en el core) y librería `Servo`
  (se instala aparte, ver [Codigo/Readme.txt](Codigo/Readme.txt))

## Diagrama
El servomotor va en el pin 9 (señal); su alimentación viene de una fuente externa
de 5-6 V, nunca del pin 5V del Arduino, con tierra común entre Arduino, fuente y
servo.

[Ver carpeta Diagrama](Diagrama/)

## Código
El programa levanta un servidor HTTP en el puerto 80. Al recibir
`GET /set?a=NN`, mueve el servo a ese ángulo y responde; la página con el slider y
los botones va guardada en `PROGMEM`. Compilado y probado con el circuito real (ver
video).

[Ver código](Codigo/ControlServoWeb.ino)

## Reporte
El reporte contiene la metodología, capturas de la interfaz en 0°/90°/180° y
conclusiones técnicas.

[Ver Reporte](Reporte/Reporte-Control-Servo-Web.pdf)

## Resultados
El servo respondió a los tres botones (0°/90°/180°) y al slider en tiempo real; se
registró también un primer intento de conexión fallido seguido de una reconexión
exitosa. Resultados cualitativos (video y Monitor Serie), sin medición instrumentada.

[Ver Resultados](Resultados/Resultados-Control-Servo-Web.pdf)

## Video
El video muestra el servomotor respondiendo a la interfaz web en tiempo real.

[Ver video](https://youtube.com/shorts/U4AMvonls_c) · [Ver carpeta Video](Video/)

## Conclusiones
Servir la interfaz desde el propio Arduino evita depender de una app o un servidor
externo: el mismo microcontrolador controla el servomotor y sirve la página que lo
opera, y basta con estar en la misma red Wi-Fi para usarla desde cualquier
navegador.

El debounce de 40 ms en el slider evita mandar una petición HTTP por cada pixel
arrastrado; solo se manda la posición final. Los botones, en cambio, mandan la
petición de inmediato porque representan una sola decisión del usuario, no un
arrastre continuo.

En la prueba real el primer intento de conexión a la red falló y el programa
reintentó solo hasta conectarse, imprimiendo la IP asignada en cuanto lo logró: el
bucle de reintentos con límite (`intentos < 40`) hizo innecesario reprogramar la
placa por una falla de red pasajera.
