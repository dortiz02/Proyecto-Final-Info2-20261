# Pokémon Swim Championship 🏊

Videojuego 2D desarrollado en C++ con Qt Creator como proyecto final de Informática II — Universidad de Antioquia, semestre 2026-1.

## Descripción

Pokémon Swim Championship es un videojuego de natación enmarcado en el universo Pokémon. El jugador controla a **Totodile** en una competencia acuática de dos niveles contra un rival controlado por inteligencia artificial: **Psyduck**, un agente autónomo que observa el rendimiento del jugador y adapta su estrategia en cada carrera.

## Niveles

### Nivel 1 — Piscina Olímpica de Cerulean City
Vista lateral con scroll horizontal. El jugador mantiene el ritmo de brazadas alternando las teclas **A/D** mientras esquiva Magikarps que saltan en trayectoria parabólica. La velocidad decae por resistencia hidrodinámica si se deja de bracear.

- **Física 1:** Resistencia hidrodinámica — `v(t) = v₀ · e^(−k·t)`
- **Física 2:** Movimiento parabólico del Magikarp — `y = y₀ + vy·t − ½·g·t²`

### Nivel 2 — Río Safari, Zona Safari
Vista cenital con timer descendente. El jugador navega un río con corrientes oscilatorias y remolinos que atraen al personaje en espiral. El objetivo es llegar a la meta antes de que el tiempo se agote.

- **Física 3:** Corriente oscilatoria — `Fx = A · sin(ω·t)` + componente de avance constante
- **Física adicional:** Atracción espiral de remolinos (fuerza centrípeta parametrizable)

## Personaje: Totodile

| Rasgo | Efecto |
|---|---|
| **Hiperactivo** | Burst de velocidad en los primeros 3 segundos de cada carrera |
| **Mordedor** | Destruye obstáculos cercanos, pero frena al personaje 2 segundos |

## Agente inteligente: Psyduck

Psyduck implementa un ciclo de cuatro componentes:

1. **Percepción** — registra el tiempo total del jugador al finalizar cada carrera
2. **Razonamiento** — calcula el promedio histórico y detecta si el jugador mejoró o empeoró
3. **Acción** — ajusta su velocidad base para la siguiente carrera según el análisis
4. **Aprendizaje** — almacena el historial de tiempos de la sesión para decisiones progresivamente más informadas

## Tecnologías

- **Lenguaje:** C++17
- **GUI:** Qt Creator (Qt 6)
- **Build:** qmake / `.pro` + Makefile
- **Contenedores:** STL (`std::vector`, `std::deque`)
- **RNG:** `std::mt19937` + `std::random_device`

## Requisitos técnicos

- Qt 6.x instalado con módulos `QtWidgets` y `QtMultimedia`
- Compilador `g++` con soporte C++17
- Sistema operativo: Linux / Windows (probado en Ubuntu 24)

## Estructura del repositorio

```
pokemon-swim-championship/
├── src/               # Código fuente (.h y .cpp)
├── assets/
│   ├── sprites/       # Sprites de personajes y obstáculos
│   └── sounds/        # Efectos de sonido y música de fondo
├── docs/              # Documentos de los tres momentos y diagramas
├── build/             # Ejecutable generado
├── pokemon_swim.pro   # Archivo de proyecto Qt
└── README.md
```

## Integrante

- David Salomon Ortiz Romero

## Curso

Informática II — Facultad de Ingeniería, Universidad de Antioquia  
Semestre 2026-1
