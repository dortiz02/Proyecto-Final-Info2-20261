#pragma once
#include "Personaje.h"

class Jugador : public Personaje {
public:
    Jugador(float x, float y, int dificultad);
    Jugador(const Jugador& otro);               // constructor de copia
    ~Jugador() override;

    void bracear();
    void activarMordedor();
    void actualizar(float dt) override;

    float getStamina()          const;
    bool  esMordedorActivo()    const;
    bool  esHiperactivoActivo() const;
    float getProgreso()         const;
    float getK()                const;  // para sustentacion: exponer coeficiente

private:
    float stamina;
    bool  mordedorActivo;
    float timerMordedor;
    bool  hiperactivoActivo;
    float timerHiperactivo;
    float k;                // coeficiente de resistencia hidrodinamica
    float progreso;
    float v0;               // velocidad al soltar tecla (inicio del decaimiento)
    float tiempoSinBracear;
};
