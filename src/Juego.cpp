#include "Juego.h"
#include <stdexcept>

Juego::Juego()
    : nivelActual(nullptr),
      dificultad(1),
      nivelNum(0),
      estado(MENU)
{}

Juego::~Juego() {
    delete nivelActual;
}

void Juego::iniciarNivel(int num) {
    if (num < 1 || num > 2)
        throw std::invalid_argument("Numero de nivel invalido");
    delete nivelActual;
    nivelActual = new Nivel(num, dificultad);
    nivelNum    = num;
    estado      = JUGANDO;
}

void Juego::actualizar(float dt) {
    if (estado != JUGANDO || !nivelActual || dt <= 0.0f) return;
    nivelActual->actualizar(dt);
    if (nivelActual->estaTerminado())
        estado = (nivelNum == 2) ? FIN : RESULTADO;
}

void Juego::setDificultad(int d) {
    if (d < 1 || d > 3)
        throw std::invalid_argument("Dificultad invalida: debe ser 1, 2 o 3");
    dificultad = d;
}

Juego::Estado Juego::getEstado()         const { return estado; }
int           Juego::getDificultad()     const { return dificultad; }
int           Juego::getNivelActualNum() const { return nivelNum; }
Nivel*        Juego::getNivel()          const { return nivelActual; }
