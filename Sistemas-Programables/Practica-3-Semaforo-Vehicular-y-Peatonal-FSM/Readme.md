# Nombre del proyecto
Semáforo vehicular y peatonal con máquina de estados finitos (FSM)

## Descripción
Esta práctica implementa el control de un semáforo vehicular y peatonal mediante una
máquina de estados finitos (FSM) programada en un Arduino UNO R4 WiFi. El semáforo
vehicular cicla en verde, amarillo y rojo; un pulsador arma una solicitud de cruce
peatonal que se atiende de forma segura al llegar al rojo. Toda la temporización se
hace con `millis()`, sin usar `delay()`.

## Objetivos
- Comprender el funcionamiento de una máquina de estados finitos (FSM).
- Representar los estados con un `enum class` y controlarlos con un `switch`.
- Manejar una solicitud asíncrona (el botón) sin romper la secuencia del semáforo.
- Leer un pulsador con antirrebote no bloqueante.
- Centralizar las salidas en una sola función para evitar estados inconsistentes.

## Herramientas y material utilizado
- Arduino UNO R4 WiFi
- 3 LEDs (rojo, amarillo, verde) — semáforo vehicular
- 2 LEDs (rojo, verde) — semáforo peatonal
- Pulsador momentáneo — solicitud de cruce
- 5 resistencias de 220 Ω
- Protoboard y cables Dupont
- Arduino IDE

## Diagrama
El diagrama muestra las conexiones del semáforo vehicular (pines 11/12/13), el
semáforo peatonal (pines 7/8) y el pulsador (pin 2, `INPUT_PULLUP`).

[Ver carpeta Diagrama](Diagrama/)

## Código
El programa modela los cuatro estados del semáforo en un `enum class` y controla las
transiciones por tiempo y por la solicitud del botón, sin usar `delay()` en ningún
punto (ni en el antirrebote).

[Ver código](Codigo/SemaforoFSM/SemaforoFSM.ino)

## Reporte
El reporte contiene la metodología, la tabla de estados de la FSM, la tabla de
pruebas, la salida del Monitor Serie y las conclusiones técnicas.

[Ver Reporte](Reporte/Reporte-Semaforo-FSM.pdf)

## Resultados
El ciclo vehicular (6/2/5 s) corrió de forma autónoma y la solicitud peatonal se
atendió únicamente al llegar al rojo, sin importar en qué momento se presionara el
botón durante el verde o el amarillo.

[Ver Resultados](Resultados/Resultados-Semaforo-FSM.pdf)

## Video
El video muestra el ciclo del semáforo vehicular y la atención de la solicitud
peatonal.

[Ver video](https://youtube.com/shorts/jQlzccY6zho) · [Ver carpeta Video](Video/)

## Conclusiones
Modelar el semáforo como una máquina de estados simplificó el diseño: cada estado
define qué luces van encendidas y qué lo saca de ahí, y el código queda como una
tabla de transiciones escrita en un `switch`. El uso de `enum class` evita comparar
estados por error, ya que el compilador rechaza comparaciones fuera de su tipo.

La solicitud peatonal "armada" es la pieza clave: el botón no interrumpe el ciclo en
el instante en que se presiona, solo levanta una bandera que se atiende en el
momento seguro. Con `millis()` el botón se puede leer en cada vuelta del `loop()`,
algo que `delay()` no permitiría sin perder la pulsación.
