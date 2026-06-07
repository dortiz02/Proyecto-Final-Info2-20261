#include "Obstaculo.h"
#include <cmath>
#include <stdexcept>

// params Magikarp : [x0, y0, vy, g]   vy negativo = sube, g positivo = gravedad
// params Remolino : [cx, cy, k, omega] cx,cy = centro, k = fuerza, omega = para animacion
Obstaculo::Obstaculo(float x, float y, int tipo, float* params, int nParams)
    : x(x), y(y), tipo(tipo), activo(true), mordido(false),
      timerMuerte(-1.0f), t(0.0f), nParams(nParams)
{
    if (params == nullptr)
        throw std::invalid_argument("Obstaculo: params no puede ser nulo");
    if (nParams < 4)
        throw std::invalid_argument("Obstaculo: se requieren al menos 4 parametros");
    if (tipo < 0 || tipo > 2)
        throw std::invalid_argument("Obstaculo: tipo debe ser 0, 1 o 2");

    this->params = new float[nParams];
    for (int i = 0; i < nParams; i++)
        this->params[i] = params[i];
}

Obstaculo::Obstaculo(const Obstaculo& otro)
    : x(otro.x), y(otro.y), tipo(otro.tipo), activo(otro.activo),
      mordido(otro.mordido), timerMuerte(otro.timerMuerte),
      t(otro.t), nParams(otro.nParams)
{
    params = new float[nParams];
    for (int i = 0; i < nParams; i++)
        params[i] = otro.params[i];
}

Obstaculo::~Obstaculo() {
    delete[] params;
}

void Obstaculo::actualizar(float dt) {
    if (dt <= 0.0f) return;

    // Si ya esta inactivo pero aun en timerMuerte, descontar el timer
    if (!activo) {
        if (timerMuerte > 0.0f) timerMuerte -= dt;
        return;
    }

    t += dt;
    if (tipo == 0) actualizarMagikarp();
    // tipo 1 y 2: t avanza para animacion, no hay movimiento propio
}

// y = y0 + vy*t + 0.5*g*t^2  (vy negativo = sube, g positivo = gravedad)
void Obstaculo::actualizarMagikarp() {
    float y0 = params[1];
    float vy = params[2];
    float g  = params[3];

    y = y0 + vy * t + 0.5f * g * t * t;

    // Cuando regresa al borde inferior, iniciar timer de muerte (0.35s = ~22 frames)
    if (t > 0.1f && y >= y0) {
        activo = false;
        timerMuerte = 0.35f;
    }
}

// Calcula la fuerza que el remolino ejerce sobre el jugador en (jx, jy)
void Obstaculo::fuerzaSobre(float jx, float jy, float& fx, float& fy) const {
    fx = fy = 0.0f;
    if (tipo != 1 || !activo) return;

    float cx = params[0], cy = params[1];
    float k  = params[2];
    float dx = cx - jx, dy = cy - jy;
    float r2 = dx*dx + dy*dy;
    if (r2 < 1.0f) return;
    float r  = std::sqrt(r2);
    fx = k * dx / (r * r2);
    fy = k * dy / (r * r2);
}

float Obstaculo::getX()           const { return x; }
float Obstaculo::getY()           const { return y; }
int   Obstaculo::getTipo()        const { return tipo; }
bool  Obstaculo::estaActivo()     const { return activo; }
bool  Obstaculo::listoPaLimpiar() const {
    // Listo cuando: inactivo Y timerMuerte ya llego a 0 (o no tenia timer = -1 inicial no usado)
    return !activo && timerMuerte <= 0.0f;
}
void  Obstaculo::desactivar() {
    activo = false;
    timerMuerte = 0.35f;  // dar 0.35s de visibilidad antes de limpiar
}
void  Obstaculo::desactivarMordido() {
    activo  = false;
    mordido = true;
    timerMuerte = 0.35f;
}
bool  Obstaculo::fueMordido() const { return mordido; }
bool  Obstaculo::esBajando()  const {
    if (tipo != 0 || nParams < 4) return false;
    float vy    = params[2];
    float g     = params[3];
    float tApex = -vy / g;
    return t > tApex;
}
