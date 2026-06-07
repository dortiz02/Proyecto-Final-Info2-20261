#pragma once
#include "Personaje.h"
#include <vector>

class AgenteRival : public Personaje {
public:
    AgenteRival(float x, float y);
    AgenteRival(const AgenteRival& otro);
    ~AgenteRival() override;

    void percibir(float tiempoJugador);
    void actualizar(float dt) override;

    float getProgreso()      const;
    float getVelocidadBase() const;
    float getVelocidad()     const;
    int   getCarreras()      const;

private:
    std::vector<float> historialTiempos;
    float velocidadBase;
    float promedioActual;
    float ultimoTiempo;
    float progreso;
    float v0;
    float tiempoSinBracear;
    float timerBraceada;
    float k;

    void razonar();
    void actuar();
    void aprender();
};
