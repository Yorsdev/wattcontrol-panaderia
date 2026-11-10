
// WattControl v2 - Gestor Inteligente de Energia para el Negocio
// Evidencia de Aprendizaje 2 (EA2) - POO en C++
// Caso: Panaderia (Digito 0 - 1)

#include <iostream> // Para mostrar mensajes y recibir datos del teclado
#include <iomanip>  // Para organizar la tabla de resultados con columnas alineadas
#include <string>   // Para usar nombres en texto
#include <cstdlib>  // Para generar los precios aleatorios con rand()

using namespace std;

// --- Parametros generales del programa ------------------------------------
#define HORAS_DIA 24 // Un dia tiene 24 horas a simular
#define MAX_CARGAS 5 // Limite de equipos que podemos guardar en la lista
#define SEMILLA 2026 // Semilla para que los precios aleatorios sean siempre los mismos

// --- Valores y limites de nuestra Panaderia --------------------------------
const float POTENCIA_REFRIGERADOR = 1.2; // kW - Nevera (no se puede apagar)
const float POTENCIA_HORNO = 7.0;        // kW - Horno (se puede aplazar, es el mas potente)
const float POTENCIA_AMASADORA = 2.2;    // kW - Amasadora (se puede aplazar)

const float POTENCIA_CONTRATADA = 9.0; // kW - Lo maximo que soporta el medidor antes de fallar

const float PRECIO_CARO = 800.0;     // COP/kWh - Si pasa de este valor, la energia es costosa
const float PRECIO_ESCASEZ = 1500.0; // COP/kWh - Si pasa de aqui, entramos en emergencia
const float EMBALSES_ALERTA = 40.0;  // % - Umbral critico de agua en las represas

const int PICO_INICIO = 18; // 6:00 PM (Empieza la hora pico)
const int PICO_FIN = 20;    // 8:00 PM (Termina la hora pico)

const int HORAS_MIN_HORNO = 6;     // Minimo de tiempo que debe funcionar el horno al dia
const int HORAS_MIN_AMASADORA = 4; // Minimo de tiempo que debe funcionar la amasadora

const int FALLA_CADA = 20; // Cada 20 horas hay 1 probabilidad de falla de señal

// =============================================================
// CLASE BASE: Carga (Representa cualquier equipo electrico)
// =============================================================
class Carga
{
protected:
    string nombre;    // Nombre del equipo
    float potenciaKW; // Potencia en Kilovatios (kW)
    bool encendida;   // ¿Esta encendido en este momento? (true / false)
    int horasUso;     // Cuantas horas ha trabajado en total

public:
    Carga(string n, float p)
    {
        nombre = n;
        potenciaKW = p;
        encendida = false;
        horasUso = 0;
    }

    virtual ~Carga() {}

    // Cada equipo dira si puede encender segun su funcion
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

    // Funciones simples para consultar datos
    string getNombre() { return nombre; }
    float getPotenciaKW() { return potenciaKW; }
    bool isEncendida() { return encendida; }
    int getHorasUso() { return horasUso; }
};

// =============================================================
// CARGA CRiTICA: Refrigerador
// Este equipo NUNCA debe apagarse porque se daña la materia prima
// =============================================================
class CargaCritica : public Carga
{
public:
    CargaCritica(string n, float p) : Carga(n, p) {}

    // Regla 1: Devuelve siempre true (siempre se prende)
    bool debeEncender(int hora, float precio, float embalses) override
    {
        return true;
    }
};

// =============================================================
// CARGA APLAZABLE: Horno y Amasadora
// Equipos que se pueden apagar para ahorrar si las condiciones son malas
// =============================================================
class CargaAplazable : public Carga
{
public:
    CargaAplazable(string n, float p) : Carga(n, p) {}

    // Reglas 2 a 5: Se apaga si es hora pico, si el precio es caro o si no hay suficiente agua
    bool debeEncender(int hora, float precio, float embalses) override
    {
        bool esHoraPico = (hora >= PICO_INICIO && hora <= PICO_FIN);
        bool precioElevado = (precio > PRECIO_CARO);
        bool embalsesBajos = (embalses < EMBALSES_ALERTA);

        // Si se cumple CUALQUIERA de las tres condiciones malas, apagamos el equipo
        if (esHoraPico || precioElevado || embalsesBajos)
        {
            return false;
        }

        return true; // Si todo esta bien, encendemos
    }
};

// =============================================================
// MEDIDOR: Se encarga de sumar el consumo en kWh y el valor en dinero
// =============================================================
class Medidor
{
private:
    float kWhAcumulados;  // Total de energia consumida
    float costoAcumulado; // Total a pagar en COP

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

// =============================================================
// MERCADO: Simula los precios de la luz segun la hora del dia
// =============================================================
class Mercado
{
public:
    float precioHora(int hora)
    {
        int minimo, maximo;
        if (hora <= 6)
        { // Madrugada (precio bajo)
            minimo = 398;
            maximo = 640;
        }
        else if (hora <= 14)
        { // Mañana (precio medio)
            minimo = 456;
            maximo = 970;
        }
        else if (hora <= 17)
        { // Tarde
            minimo = 596;
            maximo = 1040;
        }
        else if (hora <= 20)
        { // Hora pico (precio mas alto)
            minimo = 705;
            maximo = 1612;
        }
        else
        { // Noche
            minimo = 565;
            maximo = 970;
        }
        // Devuelve un valor al azar dentro de la franja correspondiente
        return minimo + rand() % (maximo - minimo + 1);
    }

    float nivelEmbalses()
    {
        return 38.5; // Simulacion en temporada seca de El Niño (< 40%)
    }
};

// NEGOCIO: Panaderia que administra los equipos y coordina la simulacion

class Negocio
{
private:
    string nombre;
    Medidor medidor;           // El medidor esta incorporado al negocio (Composicion)
    Carga *cargas[MAX_CARGAS]; // Arreglo con la lista de equipos (Agregacion)
    int numCargas;
    Mercado *mercado; // Conexion al mercado electrico (Asociacion)

    // Variables para guardar los totales de la simulacion
    float costoCon;
    float costoSin;
    int horasSimuladas;
    int fallasLectura;

public:
    Negocio(string n, Mercado *m)
    {
        nombre = n;
        mercado = m;
        numCargas = 0;
        costoCon = 0.0;
        costoSin = 0.0;
        horasSimuladas = 0;
        fallasLectura = 0;
    }

    void agregarCarga(Carga *c)
    {
        if (numCargas < MAX_CARGAS)
        {
            cargas[numCargas] = c;
            numCargas++;
        }
    }

    // Revisa puntual que haria cada equipo con datos especificos y aplica la Regla 6
    void evaluar(int hora, float precio, float embalses)
    {
        float potenciaTotal = 0.0;

        // 1. Evaluación individual de cada carga (Reglas 1 a 5)
        for (int i = 0; i < numCargas; i++)
        {
            if (cargas[i]->debeEncender(hora, precio, embalses))
            {
                cargas[i]->encender();
                potenciaTotal += cargas[i]->getPotenciaKW();
            }
            else
            {
                cargas[i]->apagar();
            }
        }

        // 2. REGLA 6: Control de Potencia Contratada (Límite de 9.0 kW para Panadería)
        if (potenciaTotal > POTENCIA_CONTRATADA)
        {
            cout << "\n [ALERTA!]: Potencia encendida (" << potenciaTotal
                 << " kW) supera el limite contratado (" << POTENCIA_CONTRATADA << " kW)." << endl;
            cout << " -> Apagando la carga aplazable de mayor consumo (Horno Industrial)..." << endl;

            // Buscar y apagar la carga aplazable de mayor potencia (Horno)
            for (int i = 0; i < numCargas; i++)
            {
                if (cargas[i]->getNombre() == "Horno Industrial" || cargas[i]->getNombre() == "Horno")
                {
                    if (cargas[i]->isEncendida())
                    {
                        cargas[i]->apagar();
                        potenciaTotal -= cargas[i]->getPotenciaKW();
                    }
                    break;
                }
            }
        }

        // 3. Mostrar el estado final ajustado en consola
        cout << "\n--- Evaluacion puntual (Hora: " << hora << "h | Precio: $"
             << precio << " | Embalses: " << embalses << "%) ---" << endl;

        for (int i = 0; i < numCargas; i++)
        {
            cout << "  " << setw(18) << left << cargas[i]->getNombre() << right
                 << (cargas[i]->isEncendida() ? "ENCENDER (ON)" : "APAZAR / OFF") << endl;
        }
    }

    // Corre el dia completo hora por hora (Simulacion de 24 Horas)
    void simularDia()
    {
        medidor.reiniciar();
        costoCon = 0.0;
        costoSin = 0.0;
        horasSimuladas = 0;
        fallasLectura = 0;

        for (int i = 0; i < numCargas; i++)
        {
            cargas[i]->reiniciarHoras();
        }

        cout << "\n=================== INICIO DE LA SIMULACIoN (24h) ===================" << endl;
        cout << setw(6) << "Hora" << setw(10) << "Precio" << setw(10) << "Embalse"
             << setw(14) << "Refri" << setw(10) << "Horno" << setw(12) << "Amasadora"
             << setw(14) << "Costo Hora" << endl;
        cout << "---------------------------------------------------------------------" << endl;

        for (int h = 0; h < HORAS_DIA; h++)
        {
            float precio = mercado->precioHora(h);
            float embalses = mercado->nivelEmbalses();

            // USO DE CONTINUE: Si falla la señal, solo dejamos encendida la nevera y saltamos la hora
            if (h > 0 && h % FALLA_CADA == 0)
            {
                fallasLectura++;
                cout << setw(6) << h << "  [FALLA DE LECTURA] - Solo se mantiene Carga Critica" << endl;
                cargas[0]->encender(); // Nevera ON
                cargas[1]->apagar();   // Horno OFF
                cargas[2]->apagar();   // Amasadora OFF

                costoCon += cargas[0]->costoHora(precio);
                costoSin += (POTENCIA_REFRIGERADOR + POTENCIA_HORNO + POTENCIA_AMASADORA) * precio;
                horasSimuladas++;
                continue; // Pasa directo a la siguiente hora sin evaluar reglas
            }

            // USO DE BREAK: Si el precio es desorbitado y los embalses estan muy bajos, cortamos todo
            if (precio > PRECIO_ESCASEZ && embalses < EMBALSES_ALERTA)
            {
                cout << "\n!!! EMERGENCIA EN HORA " << h << " !!!" << endl;
                cout << "Precio critico ($" << precio << ") y Embalses en alerta (" << embalses << "%)." << endl;
                cout << "Se detiene la simulacion por racionamiento." << endl;
                break; // Cancela el ciclo de 24 horas inmediatamente
            }

            // 1. Cada equipo consulta su regla individual
            for (int i = 0; i < numCargas; i++)
            {
                if (cargas[i]->debeEncender(h, precio, embalses))
                {
                    cargas[i]->encender();
                }
                else
                {
                    cargas[i]->apagar();
                }
            }

            // 2. REGLA 6: Control del limite de potencia contratada
            float potenciaTotalEncendida = 0.0;
            for (int i = 0; i < numCargas; i++)
            {
                if (cargas[i]->isEncendida())
                {
                    potenciaTotalEncendida += cargas[i]->getPotenciaKW();
                }
            }

            // Si sobrepasamos el limite, apagamos el equipo aplazable con mas potencia (Horno)
            if (potenciaTotalEncendida > POTENCIA_CONTRATADA)
            {
                if (cargas[1]->isEncendida())
                {
                    cargas[1]->apagar();
                    cout << "  -> [ALERTA POTENCIA HORA " << h << "]: Limite superado ("
                         << potenciaTotalEncendida << " kW). Horno Apagado por seguridad." << endl;
                }
            }

            // Calculamos el valor acumulado con y sin el gestor
            float costoHoraActual = 0.0;
            float potenciaHoraCon = 0.0;

            for (int i = 0; i < numCargas; i++)
            {
                if (cargas[i]->isEncendida())
                {
                    costoHoraActual += cargas[i]->costoHora(precio);
                    potenciaHoraCon += cargas[i]->getPotenciaKW();
                }
            }

            medidor.registrarConsumo(potenciaHoraCon, precio);
            costoCon += costoHoraActual;

            // En la opcion sin gestor, se asume que todos los equipos estuvieron encendidos
            float potenciaSin = POTENCIA_REFRIGERADOR + POTENCIA_HORNO + POTENCIA_AMASADORA;
            costoSin += (potenciaSin * precio);

            horasSimuladas++;

            // Muestra el resumen de la hora en formato de tabla
            cout << setw(6) << h << setw(10) << fixed << setprecision(0) << precio
                 << setw(9) << setprecision(1) << embalses << "%"
                 << setw(14) << (cargas[0]->isEncendida() ? "ON" : "OFF")
                 << setw(10) << (cargas[1]->isEncendida() ? "ON" : "OFF")
                 << setw(12) << (cargas[2]->isEncendida() ? "ON" : "OFF")
                 << setw(14) << setprecision(0) << costoHoraActual << endl;
        }
        cout << "=================== SIMULACIoN FINALIZADA ===================\n"
             << endl;
    }

    // Muestra un resumen de la ultima simulacion realizada

    void resumen()
    {
        if (horasSimuladas == 0)
        {
            cout << "\n[!] No hay simulacion previa registrada. Seleccione la opcion 3 en el menu." << endl;
            return;
        }

        cout << fixed << setprecision(0);
        cout << "\n=======================================================" << endl;
        cout << "         RESUMEN DE OPERACION Y AHORRO ENERGETICO       " << endl;
        cout << "=======================================================" << endl;
        cout << "Negocio: " << nombre << endl;
        cout << "Horas Simuladas: " << horasSimuladas << " / " << HORAS_DIA
             << " | Fallas de Lectura: " << fallasLectura << endl;
        cout << "-------------------------------------------------------" << endl;
        cout << "Detalle por Carga:" << endl;
        for (int i = 0; i < numCargas; i++)
        {
            float kwh = cargas[i]->getHorasUso() * cargas[i]->getPotenciaKW();
            cout << "  * " << setw(18) << left << cargas[i]->getNombre()
                 << right << cargas[i]->getHorasUso() << " h usadas ("
                 << setprecision(1) << kwh << " kWh)" << endl;
        }

        cout << "-------------------------------------------------------" << endl;
        cout << "VERIFICACION DE HORAS MINIMAS REQUERIDAS:" << endl;
        bool produccionIncompleta = false;

        if (cargas[1]->getHorasUso() < HORAS_MIN_HORNO)
        {
            cout << "  [ALERTA]: Horno opero " << cargas[1]->getHorasUso()
                 << "h (Requerido min: " << HORAS_MIN_HORNO << "h) -> produccion incompleta" << endl;
            produccionIncompleta = true;
        }
        if (cargas[2]->getHorasUso() < HORAS_MIN_AMASADORA)
        {
            cout << "  [ALERTA]: Amasadora opero " << cargas[2]->getHorasUso()
                 << "h (Requerido min: " << HORAS_MIN_AMASADORA << "h) -> produccion incompleta" << endl;
            produccionIncompleta = true;
        }
        if (!produccionIncompleta)
        {
            cout << "  [OK] Todas las cargas cumplieron su tiempo mInimo de produccion." << endl;
        }

        cout << "-------------------------------------------------------" << endl;
        cout << setprecision(0);
        cout << "Costo TOTAL Sin Gestor:  $" << costoSin << " COP" << endl;
        cout << "Costo TOTAL Con Gestor:  $" << costoCon << " COP" << endl;

        float ahorroPesos = costoSin - costoCon;
        float ahorroPorcentaje = (costoSin > 0) ? (ahorroPesos / costoSin) * 100.0 : 0.0;

        cout << "AHORRO ALCANZADO:        $" << ahorroPesos << " COP ("
             << setprecision(1) << ahorroPorcentaje << " %)" << endl;
        cout << "=======================================================\n"
             << endl;
    }
};

// PROGRAMA PRINCIPAL: Menu Interactivo con Validacion

int main()
{
    srand(SEMILLA); // Fijamos la semilla para la generacion de precios

    Mercado mercado;

    CargaCritica refrigerador("Refrigerador", POTENCIA_REFRIGERADOR);
    CargaAplazable horno("Horno Industrial", POTENCIA_HORNO);
    CargaAplazable amasadora("Amasadora", POTENCIA_AMASADORA);

    Negocio panaderia("Panaderia El Buen Pan", &mercado);
    panaderia.agregarCarga(&refrigerador);
    panaderia.agregarCarga(&horno);
    panaderia.agregarCarga(&amasadora);

    int horaManual = 12;
    float precioManual = 500.0;
    float embalsesManual = 50.0;

    int opcion = -1;

    // Menu repetitivo con do-while
    do
    {
        cout << "\n========== WATTCONTROL - MENU PRINCIPAL ==========" << endl;
        cout << "1. Ingresar lectura manual (hora, precio, embalses)" << endl;
        cout << "2. Evaluar decision de cada carga" << endl;
        cout << "3. Simular un dia completo (24 h)" << endl;
        cout << "4. Ver resumen de la ultima simulacion" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion)
        {
        case 1:
        {
            cout << "\n--- INGRESO DE LECTURA MANUAL ---" << endl;

            // Validacion de la hora (entre 0 y 23)
            do
            {
                cout << "Ingrese la hora (0 - 23): ";
                cin >> horaManual;
                if (horaManual < 0 || horaManual > 23)
                {
                    cout << "  [!] Hora incorrecta. Debe estar entre 0 y 23." << endl;
                }
            } while (horaManual < 0 || horaManual > 23);

            // Validacion del precio (mayor a 0)
            do
            {
                cout << "Ingrese el precio de energia (COP/kWh): ";
                cin >> precioManual;
                if (precioManual <= 0)
                {
                    cout << "  [!] Precio incorrecto. Debe ser un valor mayor a 0." << endl;
                }
            } while (precioManual <= 0);

            // Validacion del nivel de embalses (entre 0% y 100%)
            do
            {
                cout << "Ingrese el porcentaje de embalses (0% - 100%): ";
                cin >> embalsesManual;
                if (embalsesManual < 0 || embalsesManual > 100)
                {
                    cout << "  [!] Porcentaje incorrecto. Debe estar entre 0 y 100." << endl;
                }
            } while (embalsesManual < 0 || embalsesManual > 100);

            cout << "-> Datos guardados correctamente." << endl;
            break;
        }
        case 2:
            panaderia.evaluar(horaManual, precioManual, embalsesManual);
            break;

        case 3:
            panaderia.simularDia();
            break;

        case 4:
            panaderia.resumen();
            break;

        case 0:
            cout << "\nSaliendo del programa WattControl..." << endl;
            break;

        default:
            cout << "\n[!] Opcion no valida. Por favor elija un numero de la lista." << endl;
            break;
        }

    } while (opcion != 0);

    return 0;
}