#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <algorithm>
//idea de la implementacion https://gist.github.com/vnkdj5/0471d4ff02371eb5bb0a8773127a448d

//merge de los arreglos ordenados
void merge(int* A, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int* L = new int[n1];
    int* R = new int[n2];

    //copiar los datos a los arreglos temporales
    for (int i = 0; i < n1; i++) L[i] = A[left + i];
    for (int j = 0; j < n2; j++) R[j] = A[mid + 1 + j];

    int i = 0, j = 0, k = left;

    //comparar los elementos de los arreglos temporales y ordenarlos en el arreglo original
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) A[k++] = L[i++];
        else A[k++] = R[j++];
    }

    while (i < n1) 
    A[k++] = L[i++];
    
    while (j < n2) 
    A[k++] = R[j++];

    delete[] L;
    delete[] R;
}

void mergesort_paralelo(int* A, int left, int right, int umbral) {
    //parte secuencial
    //si el tamaño del subarreglo es menor o igual al umbral, ordenarlo secuencialmente usando sort para que sea mas rapido
    //control de granularidad
    if (right - left <= umbral) {
        std::sort(A + left, A + right + 1); 
        return;
    }

    int mid = left + (right - left) / 2;
//parte paralela
    //crear tareas para ordenar las mitades del arreglo en paralelo
    #pragma omp task
    mergesort_paralelo(A, left, mid, umbral); //ordenar la primera mitad del arreglo

    #pragma omp task
    mergesort_paralelo(A, mid + 1, right, umbral); //ordenar la segunda mitad del arreglo

    #pragma omp taskwait //esperar a que las tareas de ordenamiento terminen antes de hacer el merge
    //combinar las mitades ordenadas
    merge(A, left, mid, right);
}

//iniciar el proceso de mergesort paralelo
void merge_paralelo(int* A, int n) {
    int umbral_granularidad = 1000; //umbral de granularidad para controlar el tamaño de los subarreglos que se ordenan secuencialmente

    #pragma omp parallel //iniciar parte paralela, se inician varios hhilos
    {
        #pragma omp single //solo un hilo ejecuta la tarea inicial de mergesort
        {
            mergesort_paralelo(A, 0, n - 1, umbral_granularidad); 
        }
    }
}

//prueba 
/*
int main() {
    int n = 100000;

    int* A = new int[n];

    srand(time(NULL));

    // llenar arreglo aleatorio
    for (int i = 0; i < n; i++) {
        A[i] = rand() % 100000;
    }

    // mostrar cantidad de hilos disponibles
    std::cout << "Hilos disponibles: " 
              << omp_get_max_threads() << "\n";

    double start = omp_get_wtime();

    // ejecutar mergesort paralelo
    merge_paralelo(A, n);

    double end = omp_get_wtime();

    std::cout << "Tiempo paralelo: " 
              << (end - start) << " segundos\n";

    // verificar que esta ordenado
    bool ordenado = true;
    for (int i = 1; i < n; i++) {
        if (A[i] < A[i - 1]) {
            ordenado = false;
            break;
        }
    }

    if (ordenado)
        std::cout << "Arreglo correctamente ordenado\n";
    else
        std::cout << "Error: arreglo NO ordenado\n";

    delete[] A;
    return 0;
}
*/