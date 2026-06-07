#include "AgenteRival.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

AgenteRival::AgenteRival(float x, float y)
    : Personaje(x, y, 0.0f),
      velocidadBase(120.0f),
      promedioActual(0.0f),
      ultimoTiempo(0.0f),
      progreso(0.0f),
      v0(0.0f),
      tiempoSinBracear(0.0f),
      timerBraceada(0.0f),
      k(1.2f)
{}

AgenteRival::AgenteRival(const AgenteRival& otro)
    : Personaje(otro),
      historialTiempos(otro.historialTiempos),
      velocidadBase(otro.velocidadBase),
      promedioActual(otro.promedioActual),
      ultimoTiempo(otro.ultimoTiempo),
      progreso(otro.progreso),
      v0(otro.v0),
      tiempoSinBracear(otro.tiempoSinBracear),
      timerBraceada(otro.timerBraceada),
      k(otro.k)
{}

AgenteRival::~AgenteRival() {}

void AgenteRival::percibir(float tiempoJugador) {
    if (tiempoJugador < 0.0f)
        throw std::invalid_argument("El tiempo del jugador no puede ser negativo");
    ultimoTiempo = tiempoJugador;
    razonar();
    actuar();
    aprender();
}

void AgenteRival::razonar() {
    if (historialTiempos.empty()) {
        promedioActual = ultimoTiempo;
        return;
    }
    int n = std::min((int)historialTiempos.size(), 3);
    float suma = 0.0f;
    for (int i = (int)historialTiempos.size()-n; i < (int)historialTiempos.size(); i++)
        suma += historialTiempos[i];
    promedioActual = suma / n;
}

void AgenteRival::actuar() {
    if (promedioActual <= 0.0f) return;
    float diferencia = ultimoTiempo - promedioActual;
    float ajuste = -diferencia * 1.5f;
    velocidadBase = std::clamp(velocidadBase + ajuste, 70.0f, 200.0f);
    // k se ajusta segun dificultad acumulada: mas braceadas = menos decaimiento
    k = std::clamp(2.0f - (velocidadBase - 50.0f) / 130.0f * 0.8f, 0.8f, 2.0f);
}

void AgenteRival::aprender() {
    historialTiempos.push_back(ultimoTiempo);
}

// El agente hace braceadas automaticas a intervalos basados en su velocidadBase
// Intervalo mas corto = braceadas mas frecuentes = va mas rapido
void AgenteRival::actualizar(float dt) {
    if (dt <= 0.0f) return;

    // Calcular intervalo entre braceadas segun velocidadBase
    // velocidadBase=50 => cada 0.6s | velocidadBase=180 => cada 0.18s
    float intervalo = 0.6f - (velocidadBase - 50.0f) / 130.0f * 0.42f;
    intervalo = std::clamp(intervalo, 0.18f, 0.6f);

    timerBraceada += dt;
    if (timerBraceada >= intervalo) {
        timerBraceada = 0.0f;
        // Bracear: impulso proporcional a velocidadBase
        float impulso = velocidadBase * 0.016f;
        velocidad = std::min(velocidad + impulso, velocidadBase * 1.2f);
        v0 = velocidad;
        tiempoSinBracear = 0.0f;
    }

    // Mismo decaimiento exponencial que el jugador: v = v0 * e^(-k*t)
    tiempoSinBracear += dt;
    velocidad = v0 * std::exp(-k * tiempoSinBracear);
    if (velocidad < 0.0f) velocidad = 0.0f;

    progreso += velocidad * dt;
}

float AgenteRival::getProgreso()       const { return progreso; }
float AgenteRival::getVelocidadBase()  const { return velocidadBase; }
int   AgenteRival::getCarreras()       const { return (int)historialTiempos.size(); }
float AgenteRival::getVelocidad()      const { return velocidad; }
