# Pokémon Swim Championship

Videojuego 2D desarrollado en C++ y Qt Creator como proyecto final del curso Informática II de la Universidad de Antioquia (semestre 2026-1).

## Descripción

Pokémon Swim Championship es un juego de carreras acuáticas inspirado en el universo Pokémon. El jugador controla a **Totodile** y debe superar diferentes desafíos de natación mientras compite contra **Psyduck**, un rival controlado por el computador.

El proyecto busca aplicar conceptos vistos durante el semestre, como programación orientada a objetos, estructuras de datos, física básica para videojuegos e inteligencia artificial sencilla.

## Nivel 1: Piscina Olímpica de Cerulean City

En este nivel la cámara sigue al personaje mientras avanza horizontalmente por una piscina olímpica.

El jugador debe mantener la velocidad alternando las teclas de movimiento para simular las brazadas de natación. Durante la carrera aparecen Magikarps que saltan fuera del agua y pueden obstaculizar el avance.

Características:

* Movimiento horizontal con desplazamiento de cámara.
* Pérdida gradual de velocidad cuando el jugador deja de nadar.
* Magikarps con trayectorias parabólicas.
* Competencia directa contra Psyduck.

## Nivel 2: Río Safari

El segundo nivel se desarrolla en un río de la Zona Safari.

A diferencia del primer nivel, la vista es cenital y el objetivo es llegar a la meta antes de que el tiempo se agote. Durante el recorrido aparecen corrientes y remolinos que modifican el movimiento del personaje.

Características:

* Temporizador descendente.
* Corrientes que afectan la dirección de movimiento.
* Remolinos que atraen al jugador.
* Mayor dificultad respecto al primer nivel.

## Personaje principal: Totodile

Totodile cuenta con dos habilidades especiales:

| Habilidad   | Descripción                                                                   |
| ----------- | ----------------------------------------------------------------------------- |
| Hiperactivo | Obtiene un aumento de velocidad durante los primeros segundos de la carrera.  |
| Mordedor    | Puede destruir obstáculos cercanos, aunque reduce temporalmente su velocidad. |

## Inteligencia artificial de Psyduck

Psyduck funciona como un rival adaptable.

Al finalizar cada carrera registra el tiempo obtenido por el jugador. Con base en los resultados anteriores ajusta su velocidad para la siguiente competencia, haciendo que las carreras sean más competitivas.

Los tiempos se almacenan durante la sesión actual para que el comportamiento del rival cambie a medida que se juegan más partidas.

## Tecnologías utilizadas

* Lenguaje: C++17
* Framework gráfico: Qt 6
* Entorno de desarrollo: Qt Creator
* Estructuras de datos: STL (`vector`, `deque`)
* Generación aleatoria: `std::mt19937`

## Requisitos

* Qt 6.x
* Módulos QtWidgets y QtMultimedia
* Compilador compatible con C++17
* Windows o Linux

## Compilación y ejecución

1. Abrir el archivo `pokemon_swim.pro` en Qt Creator.
2. Configurar un kit compatible con Qt 6.
3. Compilar el proyecto.
4. Ejecutar el juego desde Qt Creator.

## Estructura del proyecto

```text
pokemon-swim-championship/
├── src/
├── assets/
│   ├── sprites/
│   └── sounds/
├── docs/
├── build/
├── pokemon_swim.pro
└── README.md
```

## Limitaciones actuales

* El historial utilizado por Psyduck solo se conserva durante la ejecución actual del juego.
* El proyecto ha sido probado principalmente en Ubuntu 24.
* Algunas mecánicas pueden requerir ajustes adicionales para equilibrar la dificultad.

## Integrante

David Salomon Ortiz Romero

## Curso

Informática II
Facultad de Ingeniería
Universidad de Antioquia
Semestre 2026-1
