#include "Jugador.h"
#include <cmath>
#include <stdexcept>
#include <algorithm>

Jugador::Jugador(float x, float y, int dificultad)
    : Personaje(x, y, 0.0f),
      stamina(100.0f),
      mordedorActivo(false),
      timerMordedor(0.0f),
      hiperactivoActivo(true),
      timerHiperactivo(3.0f),
      progreso(0.0f),
      v0(0.0f),
      tiempoSinBracear(0.0f)
{
    if (dificultad < 1 || dificultad > 3)
        throw std::invalid_argument("Dificultad debe ser 1, 2 o 3");

    switch (dificultad) {
        case 1: k = 0.8f;  break;
        case 2: k = 1.4f;  break;
        case 3: k = 2.2f;  break;
        default: k = 1.4f; break;
    }
}

Jugador::Jugador(const Jugador& otro)
    : Personaje(otro),
      stamina(otro.stamina),
      mordedorActivo(otro.mordedorActivo),
      timerMordedor(otro.timerMordedor),
      hiperactivoActivo(otro.hiperactivoActivo),
      timerHiperactivo(otro.timerHiperactivo),
      k(otro.k),
      progreso(otro.progreso),
      v0(otro.v0),
      tiempoSinBracear(otro.tiempoSinBracear)
{}

Jugador::~Jugador() {}

void Jugador::bracear() {
    if (stamina <= 0.0f) return;
    float impulso = hiperactivoActivo ? 180.0f : 120.0f;
    velocidad = std::min(velocidad + impulso * 0.016f, 300.0f);
    stamina  -= 3.0f;
    if (stamina < 0.0f) stamina = 0.0f;
    v0 = velocidad;
    tiempoSinBracear = 0.0f;
}

void Jugador::activarMordedor() {
    if (mordedorActivo) return;  // no reactivar si ya esta activo
    mordedorActivo  = true;
    timerMordedor   = 2.0f;
    velocidad      *= 0.3f;
    v0 = velocidad;
    tiempoSinBracear = 0.0f;
}

void Jugador::actualizar(float dt) {
    if (dt <= 0.0f) return;

    if (hiperactivoActivo) {
        timerHiperactivo -= dt;
        if (timerHiperactivo <= 0.0f) hiperactivoActivo = false;
    }

    if (mordedorActivo) {
        timerMordedor -= dt;
        if (timerMordedor <= 0.0f) mordedorActivo = false;
    }

    // Fisica: v(t) = v0 * e^(-k * tiempoSinBracear)
    tiempoSinBracear += dt;
    velocidad = v0 * std::exp(-k * tiempoSinBracear);
    if (velocidad < 0.0f) velocidad = 0.0f;

    progreso += velocidad * dt;

    stamina += 5.0f * dt;
    if (stamina > 100.0f) stamina = 100.0f;
}

float Jugador::getStamina()          const { return stamina; }
bool  Jugador::esMordedorActivo()    const { return mordedorActivo; }
bool  Jugador::esHiperactivoActivo() const { return hiperactivoActivo; }
float Jugador::getProgreso()         const { return progreso; }
float Jugador::getK()                const { return k; }
