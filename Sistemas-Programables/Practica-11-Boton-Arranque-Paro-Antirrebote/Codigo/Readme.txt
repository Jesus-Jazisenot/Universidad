Codigo fuente de la practica (4.1.2).

  Boton_Arranque_Paro/Boton_Arranque_Paro.ino
    Version final para el UNO R4 WiFi (matriz LED + L298N).
    class Boton(pin, ventana = 25)
      iniciar()        INPUT_PULLUP; la lectura actual es el estado de partida
                       (un dedo en el boton durante el reinicio no arranca el motor)
      actualizar()     filtro no bloqueante: acepta el cambio si la senal lleva
                       25 ms quieta; solo cuenta el paso de suelto a oprimido
      fuePresionado()  true una sola vez por cada pulsacion real
    Motor: rampa de PWM de 0 a 255 en 1.5 s; paro inmediato.
    setup(): PRIMERA ACCION motor apagado, aviso "RST" en la matriz 3 s.

  Boton_Arranque_Paro_Tinkercad/Boton_Arranque_Paro_Tinkercad.ino
    La misma version para el UNO R3 de Tinkercad (L293D y LCD 16x2 I2C).

  Boton_Sin_Filtro/Boton_Sin_Filtro.ino
    Paso 2 del desarrollo: cuenta cada flanco sin filtrar (prueba de rebote).

Monitor Serie: 115200 baudios. Comandos: V n (ventana en ms), C (contador a 0),
G (arranque con rampa / de golpe). No se usa delay().
