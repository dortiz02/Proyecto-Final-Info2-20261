#include "Nivel.h"
#include <cmath>
#include <random>
#include <stdexcept>
#include <algorithm>

static std::mt19937 rng(std::random_device{}());

Nivel::Nivel(int numeroNivel, int dificultad)
    : numeroNivel(numeroNivel), dificultad(dificultad),
      timerNivel(numeroNivel == 2 ? TIMER_NIVEL2 : 0.0f),
      terminado(false), ganador(-1),
      tiempoTranscurrido(0.0f), timerGeneracion(3.0f),
      jugadorMundoX(340.0f), jugadorMundoY(750.0f),
      jugadorCarrilY(0.5f)
{
    if (numeroNivel < 1 || numeroNivel > 2)
        throw std::invalid_argument("Numero de nivel invalido");
    if (dificultad < 1 || dificultad > 3)
        throw std::invalid_argument("Dificultad invalida");

    jugador = new Jugador(0.0f, 0.0f, dificultad);
    rival   = new AgenteRival(0.0f, 0.0f);

    if (numeroNivel == 2) {
        float p1[] = {400.0f, 250.0f, 1800.0f, 2.0f};
        float p2[] = {220.0f, 520.0f, 1800.0f, 2.0f};
        float p3[] = {490.0f, 750.0f, 2200.0f, 2.5f};
        obstaculos.push_back(new Obstaculo(400.0f, 250.0f, 1, p1, 4));
        obstaculos.push_back(new Obstaculo(220.0f, 520.0f, 1, p2, 4));
        obstaculos.push_back(new Obstaculo(490.0f, 750.0f, 1, p3, 4));
    }
}

Nivel::~Nivel() {
    delete jugador;
    delete rival;
    for (Obstaculo* o : obstaculos) delete o;
    obstaculos.clear();
}

void Nivel::actualizar(float dt) {
    if (terminado || dt <= 0.0f) return;
    tiempoTranscurrido += dt;
    jugador->actualizar(dt);
    rival->actualizar(dt);
    for (Obstaculo* o : obstaculos) o->actualizar(dt);

    if (numeroNivel == 1) {
        timerGeneracion -= dt;
        if (timerGeneracion <= 0.0f) {
            generarObstaculoNivel1();
            float base = 3.5f - dificultad * 0.7f;
            std::uniform_real_distribution<float> dist(base * 0.7f, base * 1.3f);
            timerGeneracion = std::max(0.5f, dist(rng));
        }
        if (jugador->getProgreso() >= META_PROGRESO && ganador == -1) {
            terminado = true; ganador = 0;
            rival->percibir(tiempoTranscurrido);
        } else if (rival->getProgreso() >= META_PROGRESO && ganador == -1) {
            terminado = true; ganador = 1;
            rival->percibir(tiempoTranscurrido);
        }
    }

    if (numeroNivel == 2) {
        aplicarCorrienteNivel2(dt);
        timerNivel -= dt;
        if (timerNivel <= 0.0f) { timerNivel = 0.0f; terminado = true; ganador = 1; }
        if (jugadorMundoY <= META_Y_NIVEL2) { terminado = true; ganador = 0; }
    }

    verificarColisiones();
    limpiarObstaculos();
}

void Nivel::verificarColisiones() {
    for (Obstaculo* o : obstaculos) {
        if (!o->estaActivo()) continue;

        if (o->getTipo() == 0 && numeroNivel == 1) {
            float magMundoX = o->getX();
            // mundoY del Magikarp: 0=apex, 550=fondo
            // El jugador ocupa su carrilY dentro del carril
            // La colision ocurre cuando el Magikarp esta cerca del apex
            // y la Y del Magikarp coincide con la Y del jugador en mundo piscina
            // Convertir carrilY del jugador a mundo piscina:
            // carrilY=0 => mundoPiscinaY=0 (tope carril), carrilY=1 => mundoPiscinaY=550 (fondo)
            float jugPiscinaY = jugadorCarrilY * 550.0f;

            float dx   = magMundoX - jugador->getProgreso();
            float dy   = o->getY() - jugPiscinaY;
            float dist = std::sqrt(dx*dx + dy*dy);

            if (dist < RADIO_COL) {
                jugador->setVelocidad(jugador->getVelocidad() * 0.35f);
                o->desactivar();
            }
            if (jugador->esMordedorActivo() && dist < RADIO_COL * 3.0f) {
                o->desactivarMordido();
            }
        }

        if (o->getTipo() == 1 && numeroNivel == 2) {
            float dx = o->getX() - jugadorMundoX;
            float dy = o->getY() - jugadorMundoY;
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist < RADIO_REMOL && dist > 1.0f) {
                float fx, fy;
                o->fuerzaSobre(jugadorMundoX, jugadorMundoY, fx, fy);
                jugadorMundoX = std::clamp(jugadorMundoX + fx * 0.016f, 165.0f, 520.0f);
                jugadorMundoY = std::clamp(jugadorMundoY + fy * 0.016f, 0.0f,   900.0f);
            }
            if (jugador->esMordedorActivo() && dist < RADIO_REMOL && dist > 1.0f) {
                jugadorMundoX += (jugadorMundoX - o->getX()) / dist * 20.0f;
                jugadorMundoY += (jugadorMundoY - o->getY()) / dist * 20.0f;
            }
        }
    }
}

void Nivel::generarObstaculoNivel1() {
    // Rango horizontal reducido: aparece cerca del jugador en X
    std::uniform_real_distribution<float> distOffset(40.0f, 180.0f);
    // Apex entre 0 y 200: siempre llega a la zona peligrosa superior del carril
    std::uniform_real_distribution<float> distApex(0.0f, 200.0f);

    float xPos = jugador->getProgreso() + distOffset(rng);
    float apex = distApex(rng);
    float y0   = 550.0f;
    float g    = 420.0f;
    float diff = y0 - apex;
    if (diff <= 0.0f) diff = 10.0f;
    float vy   = -std::sqrt(2.0f * g * diff);

    float p[] = {xPos, y0, vy, g};
    obstaculos.push_back(new Obstaculo(xPos, y0, 0, p, 4));
}

void Nivel::aplicarCorrienteNivel2(float dt) {
    float A     = 22.0f + dificultad * 7.0f;
    float omega = 0.7f;
    float C     = -(25.0f + dificultad * 4.0f);
    float fx = A * std::sin(omega * tiempoTranscurrido) * dt;
    float fy = C * dt;
    jugadorMundoX = std::clamp(jugadorMundoX + fx, 165.0f, 520.0f);
    jugadorMundoY = std::clamp(jugadorMundoY + fy, 0.0f,   900.0f);
}

void Nivel::limpiarObstaculos() {
    auto it = obstaculos.begin();
    while (it != obstaculos.end()) {
        if ((*it)->getTipo() == 0 && (*it)->listoPaLimpiar()) {
            delete *it; it = obstaculos.erase(it);
        } else { ++it; }
    }
}

void Nivel::moverJugador(float dx, float dy) {
    jugadorMundoX = std::clamp(jugadorMundoX + dx, 165.0f, 520.0f);
    jugadorMundoY = std::clamp(jugadorMundoY + dy, 0.0f,   900.0f);
}

void Nivel::setJugadorCarrilY(float y) {
    jugadorCarrilY = std::clamp(y, 0.0f, 1.0f);
}

void  Nivel::reiniciar() {
    delete jugador;
    jugador = new Jugador(0.0f, 0.0f, dificultad);
    auto it = obstaculos.begin();
    while (it != obstaculos.end()) {
        if ((*it)->getTipo() == 0) { delete *it; it = obstaculos.erase(it); }
        else ++it;
    }
    terminado = false; ganador = -1;
    tiempoTranscurrido = 0.0f; timerGeneracion = 3.0f;
    jugadorMundoX = 340.0f; jugadorMundoY = 750.0f;
    jugadorCarrilY = 0.5f;
}

void  Nivel::procesarBrazada()  { jugador->bracear(); }
void  Nivel::procesarMordedor() { jugador->activarMordedor(); }
bool  Nivel::estaTerminado()    const { return terminado; }
int   Nivel::getGanador()       const { return ganador; }
float Nivel::getTimer()         const { return timerNivel; }
float Nivel::getTiempoTranscurrido() const { return tiempoTranscurrido; }
Jugador*     Nivel::getJugador()    const { return jugador; }
AgenteRival* Nivel::getRival()      const { return rival; }
float Nivel::getJugadorMundoX()     const { return jugadorMundoX; }
float Nivel::getJugadorMundoY()     const { return jugadorMundoY; }
float Nivel::getJugadorCarrilY()    const { return jugadorCarrilY; }
const std::vector<Obstaculo*>& Nivel::getObstaculos() const { return obstaculos; }
