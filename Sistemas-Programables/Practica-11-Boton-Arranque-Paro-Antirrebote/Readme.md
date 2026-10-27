# Nombre del proyecto
Botón de arranque/paro a prueba de rebotes y apagones

## Descripción

En esta práctica (4.1.2) se programó el arranque y paro de un motor con un solo botón: una pulsación arranca el motor y la siguiente lo apaga. El sistema se defiende de dos fenómenos físicos reales. El primero es el rebote de los contactos del botón, que hace que el Arduino vea varias pulsaciones donde solo hubo una. El segundo son los reinicios por caída de voltaje, que pueden hacer que la máquina haga algo que nadie le ordenó.

Contra el rebote se programó un filtro no bloqueante dentro de la clase `Boton`. Solo acepta un cambio de la señal cuando se queda quieta 25 ms, medidos con `millis()`, y solo cuenta la pulsación cuando el botón pasa de suelto a oprimido. Contra los apagones, `setup()` deja el motor apagado antes que nada, avisa del reinicio en la matriz LED y en el Monitor Serie, y el motor solo vuelve a arrancar con una pulsación nueva. Además, el motor arranca con una rampa de PWM de 1.5 s para pedir menos corriente al arrancar, y se detiene de inmediato.

Como no se contaba con el Arduino físico, el circuito se armó y se probó en Tinkercad. Tinkercad no tiene el UNO R4 WiFi ni el módulo L298N, así que se usó un UNO R3 con el chip L293D (otro puente H que se controla igual) y una pantalla LCD 16x2 en lugar de la matriz LED. El programa para la placa real y el de la simulación son el mismo; solo cambia la parte de la pantalla.

## Objetivos

* Identificar fenómenos físicos que alteran el comportamiento de un sistema programable: el rebote y las caídas de voltaje.
* Implementar un filtro de rebotes no bloqueante con `millis()`, sin `delay()`.
* Detectar el flanco de bajada de la señal del botón (de suelto a oprimido).
* Diseñar un arranque seguro: después de un reinicio, el sistema no hace nada que nadie haya ordenado.
* Aplicar una rampa de arranque con PWM para reducir el pico de corriente del motor.

## Herramientas y material utilizado

### Hardware (armado real)

* Arduino UNO R4 WiFi y cable USB-C
* Módulo puente H L298N
* Motorreductor DC (tipo TT, 3–6 V)
* 2 pilas 18650 con portapilas en serie (≈ 7.4 V)
* Pulsador
* Protoboard
* Cables jumper macho-macho y macho-hembra

### Simulación (Tinkercad)

* Arduino UNO R3
* Chip puente H L293D
* Motor de CC
* Pila de 9 V
* Pulsador
* LCD 16x2 con interfaz I2C (en lugar de la matriz LED)

### Software

* Arduino IDE
* Monitor Serie
* Tinkercad

## Diagrama

### Conexiones

El Arduino solo da las órdenes; la energía del motor sale de las pilas y la conmuta el L298N.

| Parte de potencia (motor) | Conexión |
| ------------------------- | -------- |
| Positivo del portapilas   | +12V del L298N |
| Negativo del portapilas   | GND del L298N |
| GND del L298N             | GND del Arduino (tierra común, obligatoria) |
| Motorreductor             | OUT1 y OUT2 |
| Jumper 5V-EN              | Puesto |
| Jumper ENA                | Quitado, para controlar la velocidad con PWM |
| +5V del L298N             | Sin conectar |

| Parte de señal | Conexión |
| -------------- | -------- |
| D9 del Arduino | ENA del L298N (velocidad con PWM, para la rampa) |
| D7 del Arduino | IN1 del L298N (encendido) |
| IN2 del L298N  | GND (sentido fijo) |
| Pulsador       | Una pata a D2 y la opuesta a GND (sin resistencia: se usa `INPUT_PULLUP`) |

![Diagrama de conexión](Diagrama/diagrama-conexion-l298n.jpeg)

### Circuito en Tinkercad

| Tinkercad (UNO R3)         | Equivale en el armado real |
| -------------------------- | -------------------------- |
| D9 → pata 1 del L293D (EN1,2) | D9 → ENA |
| D7 → pata 2 (IN1)          | D7 → IN1 |
| Pata 7 (IN2) → GND         | IN2 → GND |
| Patas 3 y 6 (OUT1, OUT2) → motor | OUT1 y OUT2 → motor |
| Pata 8 (VCC2) → pila de 9 V | +12V → pilas 18650 |
| Pata 16 (VCC1) → 5V        | (el L298N lo resuelve con el jumper 5V-EN) |
| Patas 4 y 5 → GND          | GND del L298N → GND |
| LCD: SDA → A4, SCL → A5, VCC → 5V, GND → GND | Matriz LED integrada |
| Pulsador entre D2 y GND    | Pulsador entre D2 y GND |

![Circuito en Tinkercad](Diagrama/circuito-tinkercad.jpeg)

[Ver carpeta Diagrama](Diagrama/)

## Código

### Clase `Boton`

El filtro de rebotes vive en la clase `Boton`, para poder reutilizarlo con cualquier botón en prácticas posteriores. Guarda como datos privados el pin, la lectura anterior, el estado aceptado, la hora del último cambio, la ventana de estabilidad (25 ms) y la bandera de pulsación.

| Método | Qué hace |
| ------ | -------- |
| `iniciar()` | Configura el pin con `INPUT_PULLUP` y toma la lectura actual como estado de partida |
| `actualizar()` | Se llama en cada vuelta del `loop()`. Si la lectura cambió, reinicia el cronómetro; si lleva 25 ms quieta y es distinta del estado aceptado, la acepta. Si el nuevo estado es `LOW` (oprimido), enciende la bandera |
| `fuePresionado()` | Regresa `true` una sola vez por cada pulsación real y apaga la bandera |

Cada rebote reinicia el cronómetro, así que un rebote de 1 a 10 ms nunca llega a los 25 ms y no pasa el filtro. Como solo se enciende la bandera cuando el estado aceptado pasa a `LOW`, soltar el botón no cuenta. En `iniciar()` el estado de partida es la lectura real del pin: si alguien tenía el dedo en el botón durante un reinicio, eso no se toma como una pulsación nueva y el motor no arranca solo.

### Motor y arranque seguro

* **Primera acción de `setup()`:** PWM en 0 e IN1 en `LOW`, antes de configurar cualquier otra cosa.
* **Aviso de reinicio:** la matriz LED muestra "RST" durante 3 s y el Monitor Serie imprime `*** SISTEMA REINICIADO ***`. Después, la matriz muestra "ON" u "OFF" y, en la fila de abajo, una barra con la velocidad.
* **Rampa:** al arrancar, el PWM sube de 0 a 255 en 1.5 s, en proporción al tiempo medido con `millis()` desde el inicio de la rampa.
* **Paro:** inmediato, PWM a 0 e IN1 en `LOW`.
* El programa no usa `delay()`: el `loop()` actualiza el botón, la rampa, la pantalla y el Monitor Serie en cada vuelta.

### Comandos del Monitor Serie (115200 baudios)

| Comando | Acción |
| ------- | ------ |
| `V n`   | Cambia la ventana del filtro a n ms (para probar 2, 25 y 300 ms sin volver a subir el programa) |
| `C`     | Pone el contador de pulsaciones en 0 |
| `G`     | Alterna el arranque entre rampa y golpe (PWM directo a 255), para compararlos |

### Archivos

* `Boton_Arranque_Paro`: versión final para el UNO R4 WiFi (matriz LED).
* `Boton_Sin_Filtro`: versión del paso 2 del desarrollo, que cuenta cada flanco sin filtrar.
* `Boton_Arranque_Paro_Tinkercad`: la misma versión final para el UNO R3 de Tinkercad, con la LCD I2C en lugar de la matriz.

[Ver código Boton_Arranque_Paro.ino](Codigo/Boton_Arranque_Paro/Boton_Arranque_Paro.ino)

[Ver carpeta Código](Codigo/)

## Pruebas

1. **Arranque seguro:** al iniciar, el motor queda apagado y aparece el aviso de reinicio.
2. **Arranque y paro:** una pulsación arranca el motor con rampa y la siguiente lo detiene de inmediato.
3. **Prueba del apagón:** con el motor girando se corta la alimentación (en la simulación, deteniendo e iniciando de nuevo la simulación). Al volver, el motor debe quedar apagado, aparecer el aviso y arrancar solo con una pulsación nueva.
4. **Rampa contra golpe:** con `G` se compara el arranque de golpe contra el arranque con rampa.
5. **Ventana del filtro:** con `V 300` y con `V 25` se repite la misma pulsación corta.
6. **Prueba de rebote:** oprimir el botón 10 veces, despacio, con la versión sin filtro y con el filtro.

## Resultados

Las pruebas se hicieron en la simulación de Tinkercad, con el mismo código adaptado al UNO R3.

* **Arranque seguro:** al iniciar, la LCD mostró "SISTEMA REINICIADO", el Monitor Serie imprimió `*** SISTEMA REINICIADO ***` y el motor quedó en 0 rpm. A los 3 s la LCD cambió a "Motor: OFF".
* **Arranque con rampa:** con la primera pulsación, la LCD mostró "Motor: RAMPA" y la barra fue subiendo mientras el motor aceleraba. Al terminar, el Monitor Serie imprimió `Rampa terminada: motor a velocidad maxima (PWM 255)` y la LCD cambió a "Motor: ON".
* **Paro:** con la siguiente pulsación el motor se detuvo de inmediato y la LCD volvió a "Motor: OFF".
* **Apagón:** con el motor girando se detuvo la simulación y se volvió a iniciar. El sistema arrancó en estado seguro, con el aviso de reinicio y el motor apagado, y el contador volvió a empezar en `Pulsacion #1` solo cuando se oprimió el botón otra vez.
* **Golpe:** con `G` el motor pasó directo a velocidad máxima ("Motor: ON" sin pasar por "RAMPA").
* **Ventana:** con `V 300`, una pulsación corta no se contó. Con `V 25`, la misma pulsación corta sí se contó (`Pulsacion #4`).

![Reinicio después del apagón](Resultados/reinicio-despues-del-apagon.jpeg)

![Resultados Terminal](Resultados/monitor-serie-pruebas.jpeg)

### Limitaciones de la simulación

**La prueba de rebote no se puede hacer en Tinkercad.** Su pulsador es ideal: cambia de estado una sola vez y no tiene la laminita de metal que rebota. Por eso, en la simulación la versión sin filtro cuenta lo mismo que la versión con filtro, y no se pueden llenar con datos reales las columnas de pulsaciones contadas.

Además, con la LCD I2C y el motor activos, Tinkercad corre mucho más lento que el tiempo real: en una de las pruebas avanzó 3.4 s de simulación en 134 s reales. Por eso, al intentar contar 10 pulsaciones seguidas, algunas se perdían por el simulador y no por el filtro. Esos conteos no se tomaron como resultados.

La tabla de la práctica queda para llenarse con un botón físico:

| Prueba | Pulsaciones reales | Pulsaciones contadas | Veces que el motor no obedeció |
| ------ | ------------------ | -------------------- | ------------------------------ |
| Sin filtro, intento 1 | 10 | Pendiente (botón físico) | Pendiente |
| Sin filtro, intento 2 | 10 | Pendiente (botón físico) | Pendiente |
| Sin filtro, intento 3 | 10 | Pendiente (botón físico) | Pendiente |
| Ventana 2 ms   | 10 | Pendiente (botón físico) | Pendiente |
| Ventana 25 ms  | 10 | Pendiente (botón físico) | Pendiente |
| Ventana 300 ms | 10 | Pendiente (botón físico) | Pendiente |

## Preguntas de reflexión

**1. Explica con tus palabras qué es el rebote y por qué el Arduino lo detecta aunque tú no lo notes.**

Dentro del botón hay una laminita de metal que, al oprimirla, choca contra los contactos y rebota varias veces antes de quedarse pegada, como una pelota que cae al piso. Todo pasa en unos pocos milisegundos (de 1 a 10 ms), así que una persona siente un solo clic. Pero el Arduino lee el pin miles de veces por segundo, así que alcanza a ver cada separación de la laminita como un cambio de la señal: oprimido, suelto, oprimido, suelto… Para él son varias pulsaciones.

**2. ¿Por qué el contador debe sumar solo al pasar de suelto a oprimido y no en cualquier cambio de la señal?**

Porque cada pulsación completa tiene dos cambios: uno al oprimir (de `HIGH` a `LOW`) y otro al soltar (de `LOW` a `HIGH`). Si el contador sumara en cualquier cambio, contaría 2 por cada pulsación aunque no hubiera ningún rebote, y el motor se prendería y se apagaría con un solo clic. Al sumar solo en el paso de suelto a oprimido, una pulsación vale exactamente 1.

**3. ¿Por qué el rebote al soltar el botón también puede sumar pulsaciones?**

Porque al soltar, la laminita también rebota antes de separarse por completo. La señal se ve así: `HIGH`, `LOW`, `HIGH`. Esa bajada falsa a `LOW` es un paso de suelto a oprimido, así que un programa sin filtro la cuenta como si el usuario hubiera oprimido otra vez, aunque en realidad lo estaba soltando. El filtro lo evita porque ese `LOW` dura menos de 25 ms y nunca se acepta.

**4. ¿Qué problema tendría usar `delay(50)` para filtrar el rebote en lugar de `millis()`?**

`delay()` detiene todo el programa mientras espera. Durante esos 50 ms el Arduino no puede subir la rampa del motor, actualizar la pantalla, leer el Monitor Serie ni revisar otras entradas, y con cada pulsación el sistema se "congela" un momento. Además, `delay(50)` no filtra de verdad: solo espera un tiempo fijo después del primer cambio, sin comprobar que la señal se haya quedado quieta, así que un rebote que llegue después de la espera (por ejemplo, al soltar) se sigue contando. Con `millis()` el programa solo anota la hora y sigue trabajando, y acepta el cambio cuando la señal lleva el tiempo completo sin moverse.

**5. Según tus pruebas, ¿qué pasa si la ventana de estabilidad es muy corta? ¿Y si es muy larga?**

Con una ventana muy larga (300 ms) se pierden las pulsaciones rápidas. En la simulación, una pulsación corta no se contó con `V 300`, y la misma pulsación sí se contó con `V 25`. El botón también se siente lento, porque el sistema tarda en reaccionar. Con una ventana muy corta (2 ms) no se puede medir en Tinkercad, porque su botón no rebota. Con un botón real se esperaría que se colaran rebotes, porque duran de 1 a 10 ms y muchos superan los 2 ms. Por eso 25 ms es un buen punto medio: es más largo que cualquier rebote y mucho más corto que una pulsación humana.

**6. ¿Por qué es peligroso que un sistema reinicie con el motor encendido? Da un ejemplo industrial.**

Porque al volver la energía, una máquina podría arrancar sola sin que nadie lo ordenara, justo cuando alguien la está revisando o tiene las manos cerca. Por ejemplo, en una banda transportadora, una sierra o una prensa, un operador puede acercarse a destrabar una pieza después de un parpadeo de luz. Si la máquina se reanuda sola al regresar la energía, lo puede lastimar. Por eso las máquinas industriales exigen que alguien vuelva a oprimir el botón de arranque después de un corte.

**7. ¿Por qué la rampa de arranque ayuda a evitar caídas de voltaje?**

Porque un motor quieto pide mucha más corriente al arrancar que cuando ya gira: todavía no genera la fuerza contraelectromotriz que limita la corriente. Si el PWM pasa de 0 a 255 de golpe, el motor jala ese pico de una vez, la fuente no lo alcanza a dar y el voltaje baja, lo que puede reiniciar al Arduino. Con la rampa, el voltaje promedio sobre el motor sube poco a poco, el motor gana velocidad mientras la corriente sube, y el pico es mucho menor.

**8. Menciona dos medidas de hardware que ayudan a evitar caídas de voltaje y que el software no puede reemplazar.**

* **Alimentar el motor con una fuente separada y suficiente** (las pilas 18650 o una fuente con la corriente adecuada), nunca desde el pin 5V del Arduino, con las tierras unidas.
* **Poner un capacitor electrolítico** (por ejemplo, de 470 a 1000 µF) cerca de la alimentación del driver, que entrega la corriente del pico de arranque y amortigua la caída.

También ayudan los cables gruesos y cortos y las conexiones firmes, porque los cables delgados o flojos y los malos contactos de la protoboard provocan caídas de voltaje que ningún programa puede corregir.

## Reporte

En el reporte se explica el filtro de rebotes de la clase `Boton`, la detección del flanco de bajada, el arranque seguro, la rampa de PWM y las pruebas realizadas en la simulación, junto con las respuestas a las preguntas de reflexión.

[Ver reporte](Reporte/Reporte-Boton-Arranque-Paro.pdf)

## Video

En el siguiente video se muestra el circuito en Tinkercad, el arranque seguro con el aviso de reinicio, el arranque con rampa, el paro inmediato, la prueba del apagón, la comparación entre arranque de golpe y con rampa, y la prueba de la ventana del filtro.

[Ver video de la práctica](https://youtu.be/QTUOvEkSXTw)

[Ver carpeta Video](Video/)

## Conclusiones

La práctica mostró que un programa que funciona "en teoría" puede fallar en el mundo físico. El botón no cambia una sola vez, sino que rebota, y el voltaje no siempre es estable. El filtro de rebotes resolvió el primer problema dentro del software, sin detener el programa: basta con no creerle a la señal hasta que se quede quieta un tiempo, y contar solo el paso de suelto a oprimido.

También se comprendió la importancia del arranque seguro. Un reinicio por sí solo no es peligroso; lo peligroso es que el sistema haga algo que nadie ordenó al volver. Dejar el motor apagado como primera acción, avisar del reinicio y esperar una orden nueva hace que el sistema sea predecible.

Finalmente, la simulación permitió comprobar el arranque seguro, la rampa, el paro y el efecto de la ventana del filtro, pero también mostró sus límites: el botón de Tinkercad no rebota y el simulador se vuelve lento con la pantalla, así que la medición del rebote necesita un botón físico. El software ayuda, pero no reemplaza una buena alimentación ni buenas conexiones.
