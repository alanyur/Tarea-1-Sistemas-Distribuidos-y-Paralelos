# Tarea-1-Sistemas-Distribuidos-y-Paralelos

Implementaciones de MergeSort secuencial/paralelo y variantes k-way.

## Requisitos

- g++ con soporte C++17
- OpenMP (para las versiones paralelas)
- make (opcional, para facilitar compilacion)

Para análisis experimental con python

- jupyter notebook
- pandas
- matplotlib

## Estructura de salida

Todos los binarios compilados con make quedan en la carpeta `bin/`.

## Compilar y ejecutar cada archivo individualmente

### 1) `mergesort_seq.cpp`

Compilar:

```bash
mkdir -p bin
g++ -std=c++17 -O3 -march=native -Wall -Wextra -Wpedantic mergesort_seq.cpp -o bin/mergesort_seq
```

Ejecutar:

```bash
./bin/mergesort_seq <n> <threshold>
# ejemplo
./bin/mergesort_seq 1048576 1024
```

### 2) `mergesort_par.cpp`

Compilar:

```bash
mkdir -p bin
g++ -std=c++17 -O3 -march=native -Wall -Wextra -Wpedantic -fopenmp mergesort_par.cpp -o bin/mergesort_par
```

Ejecutar:

```bash
OMP_NUM_THREADS=8 ./bin/mergesort_par <n> <threshold>
# ejemplo
OMP_NUM_THREADS=8 ./bin/mergesort_par 1048576 1024
```

### 3) `kway_mergesort_seq.cpp`

Compilar:

```bash
mkdir -p bin
g++ -std=c++17 -O3 -march=native -Wall -Wextra -Wpedantic kway_mergesort_seq.cpp -o bin/kway_seq
```

Ejecutar:

```bash
./bin/kway_seq <n> <k> <threshold>
# ejemplo
./bin/kway_seq 1048576 4 1024
```

### 4) `kway_mergesort_par.cpp`

Compilar:

```bash
mkdir -p bin
g++ -std=c++17 -O3 -march=native -Wall -Wextra -Wpedantic -fopenmp kway_mergesort_par.cpp -o bin/kway_par
```

Ejecutar:

```bash
OMP_NUM_THREADS=8 ./bin/kway_par <n> <k> <threshold>
# ejemplo
OMP_NUM_THREADS=8 ./bin/kway_par 1048576 4 4096
```

### 5) `kway_mergesort_ranks.cpp`

Compilar:

```bash
mkdir -p bin
g++ -std=c++17 -O3 -march=native -Wall -Wextra -Wpedantic -fopenmp kway_mergesort_ranks.cpp -o bin/kway_ranks
```

Ejecutar:

```bash
OMP_NUM_THREADS=8 ./bin/kway_ranks <n> <k> <threshold>
# ejemplo
OMP_NUM_THREADS=8 ./bin/kway_ranks 1048576 4 4096
```

### 6) `kway_mergesort_full.cpp`

Compilar:

```bash
mkdir -p bin
g++ -std=c++17 -O3 -march=native -Wall -Wextra -Wpedantic -fopenmp kway_mergesort_full.cpp -o bin/kway_full
```

Ejecutar:

```bash
OMP_NUM_THREADS=8 ./bin/kway_full <n> <k> <threshold>
# ejemplo
OMP_NUM_THREADS=8 ./bin/kway_full 1048576 4 4096
```

## Compilar y ejecutar con make

### Compilar

```bash
make all
```

Otros targets utiles:

```bash
make seq
make par
make kway
make mergesort_seq
make mergesort_par
make kway_seq
make kway_par
make kway_ranks
make kway_full
```

### Ejecutar binarios compilados por make

```bash
./bin/mergesort_seq 1048576 1024
OMP_NUM_THREADS=8 ./bin/mergesort_par 1048576 1024
./bin/kway_seq 1048576 4 1024
OMP_NUM_THREADS=8 ./bin/kway_par 1048576 4 4096
OMP_NUM_THREADS=8 ./bin/kway_ranks 1048576 4 4096
OMP_NUM_THREADS=8 ./bin/kway_full 1048576 4 4096
```

### Limpiar

```bash
make clean
```

Esto elimina la carpeta `bin/` completa y tambien borra binarios legacy que puedan quedar en la raiz.

---

Nosotros, Javier Cadagán, Alan Ibacache, Mariel Muñoz declaramos que hemos utilizado herramientas de inteligencia artificial generativa, como [ChatGPT, Gemini], de manera ética y responsable para apoyar la realización de este trabajo. A continuación, se detalla específicamente el uso otorgado:

[✅] Redacción, Estructuración, Mejora del Texto y Corrección Ortográfica: Uso de IA para reescribir ideas originales, organizar secciones, mejorar la coherencia, claridad, estilo, corregir errores gramaticales y ortográficos. (Readme en Markdown y Análisis Teorico en LaTeX)

[] Traducción: Uso de IA para traducir textos a distintos idiomas.

[] Generación de Ideas: Uso de la IA como fuente de inspiración o para explorar enfoques novedosos en el desarrollo del trabajo. Siempre que se han utilizado ideas específicas provenientes de la IA, se ha citado adecuadamente su origen.

[✅] Asesoría Técnica o Conceptual: Consulta sobre conceptos técnicos o metodológicos complejos. La información proporcionada por la IA ha sido revisada, contrastada y validada con fuentes académicas o científicas adecuadas para asegurar su precisión y pertinencia. (Debugging, Compilación)

[] Otros usos: _____________________________________________________________________________________

Declaramos que todo contenido generado o asistido por IA ha sido revisado, adaptado y validado para asegurar su originalidad y pertinencia. Somos los únicos responsables del trabajo presentado y nos comprometemos a que las fuentes utilizadas sean debidamente citadas.
