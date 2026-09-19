# Nombre del proyecto
Temporización no bloqueante: `delay()` contra `millis()`

## Descripción
Práctica en dos partes sobre el mismo circuito de tres LEDs que deben parpadear a
ritmos distintos (500, 1000 y 1500 ms). La Parte 1 usa `delay()` y muestra por qué
ese enfoque no logra los ritmos pedidos. La Parte 2 resuelve el mismo problema con
`millis()`, permitiendo que los tres LEDs —y una tarea extra por serial— corran de
forma independiente sin bloquear el programa.

## Objetivos
- Comprobar que `delay()` bloquea por completo la ejecución del programa.
- Sustituir `delay()` por comparaciones con `millis()`
  (`ahora - ultimoCambio >= periodo`).
- Organizar varias tareas periódicas en un `struct` y un arreglo.
- Comparar ambos enfoques sobre el mismo hardware.

## Herramientas y material utilizado
- Arduino UNO R4 WiFi
- 3 LEDs
- 3 resistencias de 220 Ω
- Protoboard y cables Dupont
- Arduino IDE

## Diagrama
El diagrama muestra los tres LEDs conectados a los pines 8, 9 y 10 con resistencia a
GND; es el mismo circuito para las dos partes.

![Diagrama de conexión](Diagrama/diagrama-3-leds.png)

## Código
Dos programas sobre el mismo circuito: uno con `delay()` (antipatrón) y otro con
`millis()` (solución no bloqueante), este último con una tarea extra que reporta por
serial cada 3 s.

[Ver código Parte 1 — delay()](Codigo/ParpadeoDelay/ParpadeoDelay.ino) ·
[Ver código Parte 2 — millis()](Codigo/ParpadeoMillis/ParpadeoMillis.ino)

## Reporte
El reporte incluye la metodología de ambas partes, las tablas de tiempos, la salida
del Monitor Serie y las conclusiones técnicas.

[Ver Reporte](Reporte/Reporte-Temporizacion-delay-y-millis.pdf)

## Resultados
Con `delay()` los LEDs quedaron encadenados en una sola secuencia de 6 s sin cumplir
sus periodos; con `millis()` cada LED cumplió su ritmo de forma independiente,
incluso con varios encendidos a la vez.

## Video
El video muestra el parpadeo de los LEDs con `delay()` y con `millis()`.

[Ver video Parte 1](https://www.youtube.com/shorts/sUPkjtxan6g) ·
[Ver video Parte 2](https://www.youtube.com/shorts/pJ0OG_M3bCs) ·
[Ver carpeta Video](Video/)

## Conclusiones
`delay()` no espera solo para un LED: detiene todo el programa, así que ningún otro
LED, botón o puerto serie puede atenderse mientras tanto. Eso explica que en la
Parte 1 los tres LEDs terminaran encadenados en un solo ciclo de 6 s.

`millis()` cambia la pregunta de "espera X ms" a "¿ya toca?", así que el `loop()`
nunca se detiene y cada tarea lleva su propio ritmo. Agregar la tarea del reto
(mensaje serial cada 3 s) no requirió tocar el código de los LEDs, lo que confirma
que el patrón escala sin reescribir el programa.
