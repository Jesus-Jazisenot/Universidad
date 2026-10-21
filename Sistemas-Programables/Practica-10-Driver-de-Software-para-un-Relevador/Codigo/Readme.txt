Codigo fuente de la practica (el que corrio el equipo en el video).

  DriverRelevador/DriverRelevador.ino
    class Relevador(pin, activoEnBajo, tMinimo = 1000)
      iniciar()        nivel de APAGADO antes y despues de pinMode(OUTPUT);
                       el arranque cuenta como cambio (1a orden tras 1 s)
      encender()       true si cambio o ya estaba asi; false si no han
      apagar()         pasado 1000 ms (el pin no se toca)
      estaEncendido()  estado actual
      faltaMs()        ms que faltan para aceptar otro cambio
      _nivelPara()     privado: (encendido != activoEnBajo) ? HIGH : LOW
      _cambiarA()      privado: aplica la regla del tiempo minimo
    Objetos: motor(7, false) activo en alto y led(6, true) activo en bajo.
    loop() sin delay(): leerSerie() junta la linea (max. 7 caracteres, en
      mayusculas) y procesar() la compara con strcmp():
      M1/M0 motor, L1/L0 LED, E estado; otra cosa -> "Comando desconocido".

Monitor Serie: 9600 baudios, con "Nueva linea".
Compilado con arduino-cli para "arduino:renesas_uno:unor4wifi": 20% de programa.
