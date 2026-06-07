#pragma once
#include "Jugador.h"
#include "AgenteRival.h"
#include "Obstaculo.h"
#include <vector>

class Nivel {
public:
    Nivel(int numeroNivel, int dificultad);
    Nivel(const Nivel& otro) = delete;
    ~Nivel();

    void actualizar(float dt);
    void verificarColisiones();
    void generarObstaculoNivel1();

    bool  estaTerminado() const;
    int   getGanador()    const;
    float getTimer()      const;
    float getTiempoTranscurrido() const;

    Jugador*     getJugador() const;
    AgenteRival* getRival()   const;
    const std::vector<Obstaculo*>& getObstaculos() const;

    void procesarBrazada();
    void procesarMordedor();
    void moverJugador(float dx, float dy);
    void setJugadorCarrilY(float y);  // nivel 1: posicion vertical normalizada 0-1

    float getJugadorMundoX()  const;
    float getJugadorMundoY()  const;
    float getJugadorCarrilY() const;

    void reiniciar();

private:
    int   numeroNivel, dificultad;
    float timerNivel, tiempoTranscurrido, timerGeneracion;
    bool  terminado;
    int   ganador;

    float jugadorMundoX, jugadorMundoY; // nivel 2
    float jugadorCarrilY;               // nivel 1: posicion vertical 0=tope, 1=fondo

    Jugador*     jugador;
    AgenteRival* rival;
    std::vector<Obstaculo*> obstaculos;

    void aplicarCorrienteNivel2(float dt);
    void limpiarObstaculos();

    static constexpr float META_PROGRESO = 3000.0f;
    static constexpr float TIMER_NIVEL2  = 60.0f;
    static constexpr float RADIO_COL     = 35.0f;
    static constexpr float RADIO_REMOL   = 80.0f;
    static constexpr float META_Y_NIVEL2 = 50.0f;
};
