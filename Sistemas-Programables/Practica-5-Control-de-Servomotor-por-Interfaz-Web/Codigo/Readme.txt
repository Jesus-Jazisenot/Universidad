Codigo fuente de la practica.

  ControlServoWeb.ino  - Este es el codigo real usado en la practica (con el que se
                         grabo el video). El Arduino UNO R4 WiFi levanta un servidor
                         HTTP (puerto 80) que sirve una pagina con un slider y tres
                         botones (0, 90, 180 grados). Al moverlos, el navegador manda
                         GET /set?a=NN por fetch (con un debounce de 40 ms en el
                         slider) y el Arduino mueve el servo en el pin 9 de inmediato.
                         [LISTO, compila y probado con el circuito real, ver video]

Antes de volver a subirlo hay que:
  1. Cambiar SSID y PASS por los datos reales de la red Wi-Fi (2.4 GHz). En el repo
     se dejan como placeholder por seguridad, nunca se sube la contrasena real.
  2. Instalar la libreria "Servo" desde el Gestor de Librerias del IDE. El core de la
     UNO R4 (renesas_uno) NO trae esta libreria integrada, a diferencia de WiFiS3;
     hay que instalarla aparte (verificado compilando con arduino-cli: sin ella da
     "Servo.h: No such file or directory").

Compilado y verificado con arduino-cli para la placa "arduino:renesas_uno:unor4wifi"
(65984 bytes de programa, 25%).

Placa requerida en el Gestor de Tarjetas: "Arduino UNO R4 Boards".

PENDIENTE: capturar el Monitor Serie con la IP asignada y los mensajes "Angulo: NN".
