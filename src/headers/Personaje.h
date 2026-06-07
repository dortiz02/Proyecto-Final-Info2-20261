#include "Personaje.h"

Personaje::Personaje(float x, float y, float velocidad)
    : x(x), y(y), velocidad(velocidad), activo(true) {}

Personaje::Personaje(const Personaje& otro)
    : x(otro.x), y(otro.y), velocidad(otro.velocidad), activo(otro.activo) {}

Personaje::~Personaje() {}

float Personaje::getX()         const { return x; }
float Personaje::getY()         const { return y; }
float Personaje::getVelocidad() const { return velocidad; }
bool  Personaje::estaActivo()   const { return activo; }

void Personaje::setVelocidad(float v) {
    if (v < 0.0f) v = 0.0f;
    velocidad = v;
}
