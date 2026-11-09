# ⚡ WattControl - Gestor Inteligente de Energía para Negocios

> **Programa:** Ingeniería de Software y Datos — IU Digital de Antioquia (2026-2)  
> **Asignatura:** Programación Orientada a Objetos (POO) - PREICA2602B020043
> **Evidencia:** EA2. Clases con Herencia, Polimorfismo y Relaciones  
> **Caso de Estudio:** Panadería

---

## 📌 Contexto del Proyecto

Frente a la contingencia energética provocada por el fenómeno de **El Niño 2026** y los incrementos drásticos en los precios de la bolsa de energía en Colombia, las micro y pequeñas empresas sufren fuertes impactos en sus costos operativos.

**WattControl** es un sistema de gestión energética desarrollado en C++ bajo el paradigma de la **Programación Orientada a Objetos (POO)**. Su propósito principal es tomar decisiones automatizadas por hora sobre el encendido, apagado o aplazamiento de cargas eléctricas en una **Panadería**, protegiendo la producción básica, respetando la potencia contratada y maximizando el ahorro financiero del negocio.

---

## 📐 Arquitectura POO y Relaciones entre Clases

El sistema se diseñó integrando las 5 relaciones fundamentales de la Programación Orientada a Objetos:

| Clase | Descripción / Rol en el Sistema | Relación POO |
| :--- | :--- | :--- |
| `Carga` | **Clase base abstracta** con atributos generales (`nombre`, `potenciaKW`, `encendida`, `horasUso`) y método virtual puro `debeEncender()`. | — |
| `CargaCritica` | Hereda de `Carga`. Modela el **Refrigerador (2.5 kW)**. Garantiza que la nevera permanezca encendida para preservar los insumos (Regla 1). | **Herencia / Polimorfismo** |
| `CargaAplazable` | Hereda de `Carga`. Modela el **Horno Industrial (6.0 kW)** y la **Amasadora (3.5 kW)**. Evalúa condiciones lógicas compuestas para tomar decisiones. | **Herencia / Polimorfismo** |
| `Medidor` | Nace y vive dentro de `Negocio`. Registra el consumo acumulado en `kWh` y los costos operativos financieros. | **Composición** |
| `Mercado` | Provee datos simulados del mercado eléctrico de bolsa por franjas horarias y el estado del embalse. | **Asociación** |
| `Negocio` | Reúne la infraestructura, almacena las cargas agregadas externamente y coordina el flujo diario del negocio. | **Agregación** |

---

## ⚙️ Funcionalidades Principales

1. **Evaluación de Decisiones Lógicas Compuestas:**
   - La carga crítica (`Refrigerador`) permanece activa siempre.
   - Las cargas aplazables (`Horno` y `Amasadora`) se pausan automáticamente si se presenta **cualquiera** de estas condiciones:
     - 🕒 **Hora Pico:** Franja de 6:00 PM a 8:00 PM (18h a 20h).
     - 💰 **Precio Caro:** Precio de bolsa $> 800\text{ COP/kWh}$.
     - 💧 **Embalses Críticos:** Nivel global $< 40\%$.

2. **Control Estricto de Potencia Contratada (Regla 6):**
   - El negocio tiene un límite contratado de **$10.0\text{ kW}$**. Si en una hora la suma de potencias activas supera este umbral, el sistema apaga automáticamente la carga aplazable de mayor consumo (`Horno Industrial`) para evitar multas o disparos en el medidor.

3. **Simulación Operativa de 24 Horas:**
   - **Fluctuación de Mercado:** Precios aleatorios calculados dentro de los rangos reales por franja horaria.
   - **Salto por Falla (`continue`):** En 1 de cada 20 horas ($5\%$ probabilidad), ante una pérdida de lectura de mercado, se fuerza la desconexión preventible de cargas secundarias y se mantiene operativa únicamente la carga crítica.
   - **Salto por Emergencia (`break`):** Si el precio supera los $\$1.500\text{ COP/kWh}$ **y** los embalses caen por debajo del $40\%$, el programa declara emergencia por racionamiento y suspende inmediatamente la simulación.

4. **Validación de Entradas:**
   - Menú interactivo estructurado con `do-while` y `switch`. Control de rangos en lecturas manuales: hora ($0 - 23$), precio ($> 0\text{ COP}$) y nivel de embalses ($0\% - 100\%$).

5. **Balance de Ahorro y Horas Mínimas de Producción:**
   - Comprobación al final de la jornada de las horas mínimas justificadas ($6\text{ h}$ para el horno y $4\text{ h}$ para la amasadora). Si no se cumplen, emite la alerta `"produccion incompleta"`.
   - Comparación financiera entre el costo **con gestor** frente al escenario de referencia **sin gestor** (todas las cargas encendidas $24\text{ h}$), mostrando el **ahorro final en pesos ($) y en porcentaje (%)**.

---

## 🚀 Compilación y Ejecución

El proyecto está preparado para compilar con `g++` (MSYS2) en **Visual Studio Code** o cualquier terminal compatible.

### Compilar:
```bash
g++ wattControl.cpp -o wattControl.exe
```

### Ejecutar:
```bash
./wattControl.exe
```

---

## 🛠️ Estructura del Repositorio

```text
wattcontrol-panaderia/
├── .gitignore         # Configuración de exclusión de binarios (.exe) y .vscode
├── README.md          # Documentación del proyecto
└── wattControl.cpp    # Código fuente principal en C++
```
