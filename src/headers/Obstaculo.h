#pragma once

class Obstaculo {
public:
    // tipo 0 = Magikarp (parabola), tipo 1 = remolino (espiral), tipo 2 = tronco (estatico)
    // params minimo 4 elementos: [x0, y0, param3, param4]
    Obstaculo(float x, float y, int tipo, float* params, int nParams);
    Obstaculo(const Obstaculo& otro);           // constructor de copia (deep copy)
    ~Obstaculo();

    void actualizar(float dt);

    float getX()          const;
    float getY()          const;
    int   getTipo()       const;
    bool  estaActivo()    const;   // false = ya no colisiona, pero puede aún renderizarse
    bool  listoPaLimpiar() const;  // true = se puede borrar de memoria
    void  desactivar();
    void  desactivarMordido();     // desactiva marcando como mordido
    bool  fueMordido()    const;   // true si fue golpeado por mordedor
    bool  esBajando()     const;   // true si el Magikarp ya paso el apex y cae

    // Para remolinos: fuerza que ejercen sobre el jugador en (jx, jy)
    void fuerzaSobre(float jx, float jy, float& fx, float& fy) const;

private:
    float  x;
    float  y;
    int    tipo;
    bool   activo;
    bool   mordido;       // fue golpeado por mordedor
    float  timerMuerte;   // cuenta regresiva para poder borrarlo (0 = listo)
    float  t;
    float* params;
    int    nParams;

    void actualizarMagikarp();
};
