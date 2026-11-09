// =============================================================
// WattControl v2 - Gestor Inteligente de Energía para un Negocio
// Evidencia de Aprendizaje 2 (EA2) - POO con Herencia y Polimorfismo
// Negocio: Panadería (Dígito 0 - 1)
// =============================================================

// --- Librerías necesarias ----------------------------------------------------
#include <iostream> // Entradas y salidas por consola (cin, cout)
#include <iomanip>  // Formato de salida (setw, setprecision, fixed)
#include <string>   // Manejo de cadenas de texto (std::string)
#include <cstdlib>  // Generación de números aleatorios (rand, srand)

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

// =============================================================
// HERENCIA Y POLIMORFISMO: CargaCritica
// Cargas prioritarias que nunca se apagan (Ej: Refrigeración)
// =============================================================
class CargaCritica : public Carga
{
public:
    CargaCritica(string n, float p) : Carga(n, p) {}

    // Implementación polimórfica: Siempre devuelve true
    bool debeEncender(int hora, float precio, float embalses) override
    {
        return true; // Nunca se aplaza ni se apaga
    }
};

// =============================================================
// HERENCIA Y POLIMORFISMO: CargaAplazable
// Cargas condicionadas a horario, precio de bolsa y embalses
// =============================================================
class CargaAplazable : public Carga
{
public:
    CargaAplazable(string n, float p) : Carga(n, p) {}

    // Implementación polimórfica con condición compuesta
    bool debeEncender(int hora, float precio, float embalses) override
    {
        bool esHoraPico = (hora >= PICO_INICIO && hora <= PICO_FIN);
        bool precioElevado = (precio > PRECIO_CARO);
        bool embalsesBajos = (embalses < EMBALSES_ALERTA);

        // Regla compuesta: Se aplaza si ocurre CUALQUIERA de las condiciones
        if (esHoraPico || precioElevado || embalsesBajos)
        {
            return false; // Decisión: Apagar / Aplazar
        }

        return true; // Decisión: Encender
    }
};

int main()
{
    cout << "=== WattControl: Prueba de Herencia y Polimorfismo ===" << endl;

    // Instanciación de cargas usando punteros a la clase base
    Carga *refri = new CargaCritica("Refrigerador", POTENCIA_REFRIGERADOR);
    Carga *horno = new CargaAplazable("Horno Industrial", POTENCIA_HORNO);

    // Lectura de prueba: Hora pico (19h), Precio caro ($900), Embalses normales (50%)
    int hora = 19;
    float precio = 900.0;
    float embalses = 50.0;

    cout << "Lectura -> Hora: " << hora << "h | Precio: $" << precio << " | Embalses: " << embalses << "%" << endl;
    cout << refri->getNombre() << ": " << (refri->debeEncender(hora, precio, embalses) ? "ENCENDER" : "APLAZAR") << endl;
    cout << horno->getNombre() << ": " << (horno->debeEncender(hora, precio, embalses) ? "ENCENDER" : "APLAZAR") << endl;

    delete refri;
    delete horno;

    return 0;
}