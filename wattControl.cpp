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

// CLASE BASE ABSTRACTA: Carga
// Molde general para los equipos eléctricos del negocio

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

// HERENCIA Y POLIMORFISMO: CargaCritica
// Cargas prioritarias que nunca se deben apagar

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

// HERENCIA Y POLIMORFISMO: CargaAplazable
// Cargas condicionadas a horario, precio de bolsa y embalses

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

// RELACIÓN 3: COMPOSICIÓN (Medidor dentro de Negocio)
// Registra la energía total consumida y los costos acumulados

class Medidor
{
private:
    float kWhAcumulados;  // Total energía consumida (kWh)
    float costoAcumulado; // Costo financiero acumulado (COP)

public:
    Medidor()
    {
        kWhAcumulados = 0.0;
        costoAcumulado = 0.0;
    }

    void registrarConsumo(float kw, float precio)
    {
        kWhAcumulados += kw;
        costoAcumulado += (kw * precio);
    }

    void reiniciar()
    {
        kWhAcumulados = 0.0;
        costoAcumulado = 0.0;
    }

    float getKWh() { return kWhAcumulados; }
    float getCosto() { return costoAcumulado; }
};

// RELACIÓN 4: ASOCIACIÓN (Mercado externo al Negocio)
// Ofrece los precios simulados del mercado eléctrico y estado embalses

class Mercado
{
public:
    float precioHora(int hora)
    {
        int minimo, maximo;
        if (hora <= 6)
        { // Madrugada
            minimo = 398;
            maximo = 640;
        }
        else if (hora <= 14)
        { // Mañana y mediodía
            minimo = 456;
            maximo = 970;
        }
        else if (hora <= 17)
        { // Tarde
            minimo = 596;
            maximo = 1040;
        }
        else if (hora <= 20)
        { // Hora pico
            minimo = 705;
            maximo = 1612;
        }
        else
        { // Noche
            minimo = 565;
            maximo = 970;
        }
        return minimo + rand() % (maximo - minimo + 1);
    }

    float nivelEmbalses()
    {
        return 38.5; // Simulación durante contingencia de El Niño (< 40%)
    }
};

// RELACIÓN 5: AGREGACIÓN (Negocio agrega Cargas externas)
// Reúne Medidor (composición), Mercado (asociación) y Cargas (agregación)

class Negocio
{
private:
    string nombre;             // Nombre del negocio
    Medidor medidor;           // COMPOSICIÓN: El medidor nace y muere con el negocio
    Carga *cargas[MAX_CARGAS]; // AGREGACIÓN: Arreglo de punteros a Cargas externas
    int numCargas;             // Cantidad de cargas agregadas
    Mercado *mercado;          // ASOCIACIÓN: Conexión externa con la red/mercado

public:
    Negocio(string n, Mercado *m)
    {
        nombre = n;
        mercado = m;
        numCargas = 0;
    }

    // Agrega cargas creadas exteriormente (Agregación)
    void agregarCarga(Carga *c)
    {
        if (numCargas < MAX_CARGAS)
        {
            cargas[numCargas] = c;
            numCargas++;
        }
    }

    void mostrarCargas()
    {
        cout << "--- Negocio: " << nombre << " ---" << endl;
        cout << "Potencia Contratada: " << POTENCIA_CONTRATADA << " kW" << endl;
        cout << "Cargas registradas en el gestor:" << endl;

        for (int i = 0; i < numCargas; i++)
        {
            cout << "  - " << setw(18) << left << cargas[i]->getNombre()
                 << right << cargas[i]->getPotenciaKW() << " kW" << endl;
        }
        cout << "---------------------------------------" << endl;
    }

    // Evalúa la decisión de encendido mediante Polimorfismo
    void evaluar(int hora, float precio, float embalses)
    {
        cout << "\n--- Evaluacion puntual (Hora: " << hora << "h | Precio: $"
             << precio << " | Embalses: " << embalses << "%) ---" << endl;

        for (int i = 0; i < numCargas; i++)
        {
            bool decide = cargas[i]->debeEncender(hora, precio, embalses);
            cout << "  " << setw(18) << left << cargas[i]->getNombre() << right
                 << (decide ? "ENCENDER (ON)" : "APLAZAR (OFF)") << endl;
        }
    }
};

int main()
{
    // Instanciación del Mercado (Asociación)
    Mercado mercado;

    // Creación externa de las Cargas (Herencia y Polimorfismo)
    CargaCritica refrigerador("Refrigerador", POTENCIA_REFRIGERADOR);
    CargaAplazable horno("Horno Industrial", POTENCIA_HORNO);
    CargaAplazable amasadora("Amasadora", POTENCIA_AMASADORA);

    // Creación del Negocio y Agregación de Cargas
    Negocio panaderia("Panaderia Belen", &mercado);
    panaderia.agregarCarga(&refrigerador);
    panaderia.agregarCarga(&horno);
    panaderia.agregarCarga(&amasadora);

    // Presentación de la infraestructura del negocio
    panaderia.mostrarCargas();

    // Prueba puntual de evaluación con datos de prueba
    panaderia.evaluar(19, 900.0, 50.0);

    return 0;
}