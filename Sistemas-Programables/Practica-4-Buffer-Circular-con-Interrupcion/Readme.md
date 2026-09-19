# Nombre del proyecto
Buffer circular alimentado por interrupción externa (contador de piezas)

## Descripción
Esta práctica simula el sensor de piezas de una banda transportadora: un pulsador
conectado a una interrupción externa avisa al Arduino cada vez que "pasa una
pieza", mientras el `loop()` dibuja una animación continua en la matriz de LEDs
integrada del UNO R4 WiFi. Un buffer circular guarda los eventos de la ISR para que
el `loop()` los procese sin que se pierda ninguno ni se detenga la animación.

## Objetivos
- Usar `attachInterrupt()` y entender las limitaciones de una ISR.
- Implementar un buffer circular con índices de escritura y lectura, resolviendo la
  ambigüedad lleno/vacío.
- Filtrar el rebote del pulsador dentro de la ISR.
- Aplicar el patrón productor–consumidor entre la ISR y el `loop()`.
- Usar `volatile` en las variables compartidas.

## Herramientas y material utilizado
- Arduino UNO R4 WiFi (matriz de LEDs 12×8 integrada)
- Pulsador momentáneo — simula el sensor de piezas
- Protoboard y cables Dupont
- Arduino IDE

## Diagrama
El diagrama muestra el pulsador conectado al pin 2 (`INPUT_PULLUP`, interrupción
`FALLING`); la matriz de LEDs está integrada en la placa y no requiere cableado
externo.

![Diagrama de conexión](Diagrama/diagrama-tinkercad.jpg)

## Código
La ISR anota el instante (`millis()`) de cada pulsación en un buffer circular de 8
lugares; el `loop()` vacía el buffer, reporta por serial y mantiene la animación de
la matriz sin detenerse.

[Ver código](Codigo/BufferCircularISR.ino)

## Reporte
El reporte contiene la metodología, la tabla de pruebas, la tabla de intervalos
medidos, la captura del Monitor Serie y las conclusiones técnicas.

[Ver Reporte](Reporte/Reporte-Buffer-Circular-ISR.pdf)

## Resultados
Se registraron 80 pulsaciones sin saltos ni repeticiones, incluida una ráfaga de 7
pulsaciones en 1.4 s. El contador de pérdidas se mantuvo en 0 en todas las pruebas y
la animación de la matriz no se detuvo en ningún momento.

## Video
El video muestra el conteo de piezas por interrupción y la animación continua en la
matriz de LEDs.

[Ver video](https://youtube.com/shorts/ujhbv712WJ4) · [Ver carpeta Video](Video/)

## Conclusiones
La interrupción externa resuelve algo que el sondeo en el `loop()` no puede: atender
un evento en el instante en que ocurre, sin importar qué esté haciendo el programa
en ese momento. Mantener la ISR mínima (solo `millis()`, una comparación y un
arreglo) fue clave para que la animación nunca se detuviera.

El buffer circular resolvió la comunicación entre dos ritmos distintos sin
condiciones de carrera, dando un único dueño a cada índice (la ISR escribe la
cabeza, el `loop()` la cola). El antirrebote dentro de la ISR también es distinto al
del `loop()`: se acepta el primer flanco y se ignoran los siguientes durante 50 ms,
en vez de esperar a que la señal se estabilice.
