#pragma once
#include "Nivel.h"

class Juego {
public:
    enum Estado { MENU, JUGANDO, RESULTADO, FIN };

    Juego();
    Juego(const Juego& otro) = delete;  // sin copia: gestiona recursos unicos
    ~Juego();

    void iniciarNivel(int num);
    void actualizar(float dt);
    void setDificultad(int d);

    Estado getEstado()          const;
    int    getDificultad()      const;
    int    getNivelActualNum()  const;
    Nivel* getNivel()           const;

private:
    Nivel*  nivelActual;
    int     dificultad;
    int     nivelNum;
    Estado  estado;
};
