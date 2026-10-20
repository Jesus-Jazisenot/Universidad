Codigo fuente de la practica.

  DriverRelevador/DriverRelevador.ino
    class Relevador (privado: pin, activoEnBajo, encendido, ultimoCambio,
    tiempoMinimo = 1000 ms)
      iniciar()        nivel de APAGADO antes y despues de pinMode(OUTPUT);
                       el arranque cuenta como cambio (1a orden tras 1 s)
      encender()       true si cambio o ya estaba encendido; false si no
      apagar()         han pasado 1000 ms (el pin no se toca)
      estaEncendido()  estado actual
      tiempoRestante() ms que faltan para aceptar otro cambio (extra)
      escribirPin()    privado, unico digitalWrite():
                       nivel = activoEnBajo ? !encendido : encendido
    Objetos: motor(D7, activo en alto) y led(D6, activo en bajo).
    loop() sin delay(): lee el Monitor Serie caracter por caracter
      M1/M0 motor, L1/L0 LED, E estado (sirve con cualquier fin de linea
      y con varias ordenes juntas, p. ej. "M1M0").

Ojo: PIN_LED ya existe en el core de la R4 (LED de la placa, D13); por eso
las constantes se llaman PIN_MOTOR_IN1 y PIN_LED_OPTO.

Monitor Serie: 115200 baudios.
Compilado con arduino-cli para "arduino:renesas_uno:unor4wifi": 20% de programa.
