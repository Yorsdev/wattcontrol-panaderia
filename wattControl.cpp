/*

 Un edificio residencial de Medellín bombea agua desde un tanque subterráneo hasta un
 tanque elevado con una bomba de 5,5 kW. Hoy la bomba se prende por nivel,
 a cualquier hora, incluso a las 6 de la tarde, cuando la energía es más cara. Además, las
 luces de las zonas comunes (1,2 kW) se quedan prendidas todo el día.

 ¿cómo programar un gestor que llene el tanque en las horas de energía barata,
 evite la hora pico, nunca deje el edificio sin agua y prenda las luces solo de noche?

*/
// =============================================================
// WattControl v2 - Gestor Inteligente de Energía para un Negocio
// Evidencia de Aprendizaje 2 (EA2) - POO con Herencia y Polimorfismo
// Negocio: Panadería
// =============================================================

// --- Librerías necesarias ----------------------------------------------------
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib> // Para números aleatorios (rand, srand)

using namespace std;

// --- Directivas del preprocesador ------------------------------------------
#define HORAS_DIA 24 // Horas a simular en un día completo
#define MAX_CARGAS 5 // Capacidad máxima del arreglo de cargas
#define SEMILLA 2026 // Semilla fija para números aleatorios rand()

// --- Constantes del negocio (Panadería: Dígito 0 - 1) ----------------------
const float POTENCIA_REFRIGERADOR = 2.5; // kW - Carga Crítica
const float POTENCIA_HORNO = 6.0;        // kW - Carga Aplazable Mayor
const float POTENCIA_AMASADORA = 3.5;    // kW - Carga Aplazable Menor

const float POTENCIA_CONTRATADA = 10.0; // kW - Límite de potencia contratada

const float PRECIO_CARO = 800.0;     // COP/kWh: Umbral de energía costosa
const float PRECIO_ESCASEZ = 1500.0; // COP/kWh: Umbral para emergencia por racionamiento
const float EMBALSES_ALERTA = 40.0;  // %: Nivel crítico de embalses por El Niño 2026

const int PICO_INICIO = 18; // 6:00 PM
const int PICO_FIN = 20;    // 8:00 PM

const int HORAS_MIN_HORNO = 6;     // Requisito mínimo de operación
const int HORAS_MIN_AMASADORA = 4; // Requisito mínimo de operación

const int FALLA_CADA = 20; // 5% de probabilidad de falla de lectura

// =============================================================
// CLASE BASE ABSTRACTA: Carga
// Molde general para los equipos eléctricos del negocio
// =============================================================
class Carga
{
protected:
    string nombre;    // Nombre descriptivo de la carga
    float potenciaKW; // Potencia en Kilovatios
    bool encendida;   // Estado actual: true (ON), false (OFF)
    int horasUso;     // Horas acumuladas de uso

public:
    // Constructor
    Carga(string n, float p)
    {
        nombre = n;
        potenciaKW = p;
        encendida = false;
        horasUso = 0;
    }

    // Destructor virtual
    virtual ~Carga() {}

    // Método virtual puro (Polimorfismo en clases hijas)
    virtual bool debeEncender(int hora, float precio, float embalses) = 0;

    void encender()
    {
        encendida = true;
        horasUso++;
    }

    void apagar()
    {
        encendida = false;
    }

    void reiniciarHoras()
    {
        horasUso = 0;
        encendida = false;
    }

    float costoHora(float precio)
    {
        if (encendida)
        {
            return potenciaKW * precio;
        }
        return 0.0;
    }

    // Getters
    string getNombre() { return nombre; }
    float getPotenciaKW() { return potenciaKW; }
    bool isEncendida() { return encendida; }
    int getHorasUso() { return horasUso; }
};

int main()
{
    cout << "=== WattControl: Estructura Inicial Cargada ===" << endl;
    cout << "Potencia Contratada: " << POTENCIA_CONTRATADA << " kW" << endl;
    return 0;
}