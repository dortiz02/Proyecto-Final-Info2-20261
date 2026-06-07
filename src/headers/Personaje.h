#pragma once

class Personaje {
public:
    Personaje(float x, float y, float velocidad);
    Personaje(const Personaje& otro);           // constructor de copia
    virtual ~Personaje();

    float getX()         const;
    float getY()         const;
    float getVelocidad() const;
    bool  estaActivo()   const;
    void  setVelocidad(float v);

    virtual void actualizar(float dt) = 0;

protected:
    float x;
    float y;
    float velocidad;
    bool  activo;
};
