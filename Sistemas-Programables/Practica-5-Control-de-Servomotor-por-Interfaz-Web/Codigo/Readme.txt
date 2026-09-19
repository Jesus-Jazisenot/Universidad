Codigo fuente de la practica.

  ControlServoWeb.ino  - El Arduino UNO R4 WiFi levanta un servidor HTTP (puerto 80)
                         que sirve una pagina con un slider. Al moverlo, el
                         navegador manda GET /servo?angulo=NN por fetch (sin
                         recargar la pagina) y el Arduino mueve el servo en el pin 9
                         en tiempo real.   [LISTO, compila]

Antes de subirlo hay que:
  1. Cambiar "NOMBRE_DE_TU_RED" y "CONTRASENA_DE_TU_RED" por los datos reales de la
     red Wi-Fi (2.4 GHz).
  2. Instalar la libreria "Servo" desde el Gestor de Librerias del IDE. A
     diferencia de otras practicas, el core de la UNO R4 (renesas_uno) NO trae esta
     libreria integrada como si trae WiFiS3; hay que instalarla aparte
     (verificado compilando con arduino-cli: sin ella da "Servo.h: No such file
     or directory").

Compilado y verificado con arduino-cli para la placa "arduino:renesas_uno:unor4wifi"
(65448 bytes de programa, 24%). Falta la prueba real con el servomotor y la red
Wi-Fi conectados.

Placa requerida en el Gestor de Tarjetas: "Arduino UNO R4 Boards".

PENDIENTE: probar en hardware real y ajustar el pin del servo si se cablea distinto.
