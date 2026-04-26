#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>
#include <algorithm>
#include "output_helper.hpp"
//idea de la implementacion https://gist.github.com/vnkdj5/0471d4ff02371eb5bb0a8773127a448d

//merge de los arreglos ordenados
void merge(std::vector<int>& A, std::vector<int>& tmp, int left, int mid, int right) {

    int i = left, j = mid, k = left;

    //comparar los elementos de los arreglos temporales y ordenarlos en el arreglo original
    while (i < mid && j < right) {
        if (A[i] <= A[j]) tmp[k++] = A[i++];
        else tmp[k++] = A[j++];
    }

    while (i < mid) 
    tmp[k++] = A[i++];
    
    while (j < right) 
    tmp[k++] = A[j++];

    for (auto aux = left; aux < right; aux++)
    {
        A[aux] = tmp[aux];
    }
}

void mergesort_paralelo(std::vector<int>& A, std::vector<int>& tmp, int left, int right, int umbral) {
    //parte secuencial
    //si el tamaño del subarreglo es menor o igual al umbral, ordenarlo secuencialmente usando sort para que sea mas rapido
    //control de granularidad
    if (right - left <= umbral) {
        std::sort(A.begin() + left, A.begin() + right); 
        return;
    }

    int mid = left + (right - left) / 2;
    //parte paralela
    //crear tareas para ordenar las mitades del arreglo en paralelo
    #pragma omp task shared(A, tmp) firstprivate(left, right)
    mergesort_paralelo(A,tmp, left, mid, umbral); //ordenar la primera mitad del arreglo

    #pragma omp task shared(A, tmp) firstprivate(left, right)
    mergesort_paralelo(A, tmp, mid, right, umbral); //ordenar la segunda mitad del arreglo

    #pragma omp taskwait //esperar a que las tareas de ordenamiento terminen antes de hacer el merge
    //combinar las mitades ordenadas
    merge(A, tmp, left, mid, right);
}

std::vector<int> generate_random(int n, unsigned seed = 42) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, n * 10);
    std::vector<int> v(n);
    for (auto& x : v) x = dist(rng);
    return v;
}

bool is_sorted(const std::vector<int>& v) {
    for (size_t i = 1; i < v.size(); ++i)
        if (v[i] < v[i - 1]) return false;
    return true;
}

int main(int argc, char* argv[]) {
    int n = (argc > 1) ? std::stoi(argv[1]) : (1 << 20);
    //int k         = (argc > 2) ? std::stoi(argv[2]) : 4; No aplica 
    int threshold = (argc > 3) ? std::stoi(argv[3]) : (1 << 10);
    std::string alg = "mergesort_par";
    int n_threads = omp_get_max_threads();

    std::cerr << alg << " n=" << n
    << " threshold=" << threshold
    << " threads=" << n_threads << "\n";

    auto A = generate_random(n);
    std::vector<int> tmp(n);

    auto t0 = std::chrono::high_resolution_clock::now();
    #pragma omp parallel//iniciar parte paralela, se inician varios hhilos
    {
        #pragma omp single //solo un hilo ejecuta la tarea inicial de mergesort
        {
            mergesort_paralelo(A, tmp, 0, n, threshold); 
        }
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    double elapsed = std::chrono::duration<double>(t1 - t0).count();

    print_output(alg, n, UNDEFINED_PARAM, threshold, n_threads, is_sorted(A), elapsed);
    return 0;
}