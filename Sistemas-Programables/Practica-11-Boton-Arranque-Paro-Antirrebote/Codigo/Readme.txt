Programas de la practica (4.1.2).

  Boton_Arranque_Paro/Boton_Arranque_Paro.ino
    Version definitiva para el UNO R4 WiFi (L298N + matriz LED).
    class Boton(pin, ventana = 25)
      iniciar()        configura INPUT_PULLUP y toma la lectura del momento como
                       estado inicial (si el boton estaba presionado al reiniciar,
                       el motor no se enciende)
      actualizar()     filtro que no bloquea: valida el cambio cuando la senal
                       lleva 25 ms estable; solo registra la transicion de suelto
                       a presionado
      fuePresionado()  devuelve true una unica vez por pulsacion real
    Motor: rampa de PWM de 0 a 255 en 1.5 s; apagado instantaneo.
    setup(): LO PRIMERO es apagar el motor; muestra "RST" en la matriz por 3 s.

  Boton_Arranque_Paro_Tinkercad/Boton_Arranque_Paro_Tinkercad.ino
    Misma version adaptada al UNO R3 de Tinkercad (L293D y LCD 16x2 I2C).

  Boton_Sin_Filtro/Boton_Sin_Filtro.ino
    Paso 2 del desarrollo: registra todos los flancos sin filtro (prueba de rebote).

Monitor Serie a 115200 baudios. Comandos: V n (ventana en ms), C (reinicia el
contador), G (alterna arranque con rampa / directo). No se utiliza delay().
