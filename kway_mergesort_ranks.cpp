// Idea de la implementacion:
// Cormen et al., Introduction to Algorithms 3rd ed., Section 27.3 (p. 804)
// https://en.wikipedia.org/wiki/Merge_algorithm#Parallel_merge
// http://www2.hawaii.edu/~nodari/teaching/f16/notes/notes10.pdf

/**
 * k-way MergeSort - Merge paralelo usando ranks y búsqueda binaria
 *
 * El cuello de botella del k-way mergesort paralelo es la fase de merge,
 * que en la versión anterior sigue siendo secuencial. Este archivo implementa
 * un merge paralelo donde cada hilo es responsable de producir una porción
 * de la salida final, calculando su punto de partida mediante búsqueda binaria
 * sobre los subarreglos ordenados (técnica de "ranks").
 *
 * Compilar:
 *   g++ -O2 -std=c++17 -fopenmp -o kway_ranks kway_mergesort_ranks.cpp
 *
 * Ejecutar:
 *   $env:OMP_NUM_THREADS=<p>; ./kway_ranks <n> <k> [threshold]
 *   Ejemplo: $env:OMP_NUM_THREADS=8; ./kway_ranks 4194304 4 4096
 */

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <algorithm>
#include <climits>
#include <cstring>
#include <omp.h>
#include "output_helper.hpp"

struct HeapNode {
    int value;
    int array_idx;
    int elem_idx;
};

struct MinCmp {
    bool operator()(const HeapNode& a, const HeapNode& b) const {
        return a.value > b.value;
    }
};


void compute_starts(const std::vector<int>& A,
                    const std::vector<int>& boundaries,
                    int out_start, int lo,
                    std::vector<int>& starts)
{
    int k = (int)boundaries.size() - 1;

    if (out_start == 0) {
        for (int i = 0; i < k; ++i)
            starts[i] = boundaries[i];
        return;
    }

    int lo_val = INT_MAX, hi_val = INT_MIN;
    for (int i = 0; i < k; ++i) {
        if (boundaries[i] < boundaries[i + 1]) {
            lo_val = std::min(lo_val, A[boundaries[i]]);
            hi_val = std::max(hi_val, A[boundaries[i + 1] - 1]);
        }
    }

    int pivot = lo_val;
    int low = lo_val, high = hi_val;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        long long total_less = 0;
        for (int i = 0; i < k; ++i) {
            total_less += std::lower_bound(
                A.begin() + boundaries[i],
                A.begin() + boundaries[i + 1],
                mid
            ) - (A.begin() + boundaries[i]);
        }
        if (total_less <= out_start) {
            pivot = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    for (int i = 0; i < k; ++i) {
        starts[i] = (int)(std::lower_bound(
            A.begin() + boundaries[i],
            A.begin() + boundaries[i + 1],
            pivot
        ) - A.begin());
    }

  
    long long consumed = 0;
    for (int i = 0; i < k; ++i)
        consumed += starts[i] - boundaries[i];

    for (int i = 0; i < k && consumed > out_start; ++i) {
        long long excess = consumed - out_start;
        long long retreat = std::min(excess, (long long)(starts[i] - boundaries[i]));
        starts[i] -= (int)retreat;
        consumed -= retreat;
    }

    for (int i = 0; i < k && consumed < out_start; ++i) {
        while (starts[i] < boundaries[i + 1] &&
               A[starts[i]] == pivot &&
               consumed < out_start) {
            ++starts[i];
            ++consumed;
        }
    }
}

/**
 * Merge paralelo de k subarreglos ordenados usando el método de ranks.
 *
 * Cada hilo calcula de forma independiente su segmento de la salida:
 *   Determina qué rango de la salida le corresponde.
 *   Usa compute_starts para saber desde dónde leer en cada subarreglo.
 *   Produce su parte del resultado con un heap local.
 *
 * Esto es un merge completamente paralelo: no hay región crítica ni
 * comunicación entre hilos durante la producción de la salida.
 *
 * Nota: se usa omp parallel for en lugar de omp parallel anidado dentro
 * de tasks, para evitar el problema de regiones paralelas anidadas que
 * retornan num_threads=1 sin OMP_NESTED=true.
 */

void parallel_merge_ranks(std::vector<int>& A,
                           const std::vector<int>& boundaries,
                           std::vector<int>& tmp,
                           int lo, int hi)
{
    int n = hi - lo;
    int k = (int)boundaries.size() - 1;
    int n_threads = omp_get_max_threads();

    #pragma omp parallel for schedule(static) num_threads(n_threads)
    for (int tid = 0; tid < n_threads; ++tid) {
        int my_start = lo + (long long)tid * n / n_threads;
        int my_end   = (tid == n_threads - 1)
                       ? hi
                       : lo + (long long)(tid + 1) * n / n_threads;

        if (my_start >= my_end) continue;

        std::vector<int> starts(k);
        compute_starts(A, boundaries, my_start - lo, lo, starts);

        std::priority_queue<HeapNode, std::vector<HeapNode>, MinCmp> pq;
        for (int i = 0; i < k; ++i) {
            if (starts[i] < boundaries[i + 1])
                pq.push({A[starts[i]], i, starts[i]});
        }

        int out = my_start;
        while (!pq.empty() && out < my_end) {
            auto node = pq.top(); pq.pop();
            tmp[out++] = node.value;
            int next = node.elem_idx + 1;
            if (next < boundaries[node.array_idx + 1])
                pq.push({A[next], node.array_idx, next});
        }
    }

    std::memcpy(A.data() + lo, tmp.data() + lo, n * sizeof(int));
}



void kway_mergesort_ranks(std::vector<int>& A, std::vector<int>& tmp,
                           int lo, int hi, int k, int threshold)
{
    int n = hi - lo;
    if (n <= threshold) {
        std::sort(A.begin() + lo, A.begin() + hi);
        return;
    }

    std::vector<int> boundaries(k + 1);
    boundaries[0] = lo;
    for (int i = 1; i <= k; ++i)
        boundaries[i] = lo + (long long)i * n / k;

    for (int i = 0; i < k; ++i) {
        int b_lo = boundaries[i];
        int b_hi = boundaries[i + 1];
        #pragma omp task shared(A, tmp) firstprivate(b_lo, b_hi, k, threshold)
        {
            kway_mergesort_ranks(A, tmp, b_lo, b_hi, k, threshold);
        }
    }

    #pragma omp taskwait
    parallel_merge_ranks(A, boundaries, tmp, lo, hi);
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

int main(int argc, char* argv[])
{
    int n         = (argc > 1) ? std::stoi(argv[1]) : (1 << 20);
    int k         = (argc > 2) ? std::stoi(argv[2]) : 4;
    int threshold = (argc > 3) ? std::stoi(argv[3]) : 4096;
    std::string alg = "k-way_ranks";
    int n_threads = omp_get_max_threads();

    std::cout << alg << " n=" << n
              << " k=" << k << " threshold=" << threshold
              << " threads=" << n_threads << "\n";

    auto A = generate_random(n);
    std::vector<int> tmp(n);

    auto t0 = std::chrono::high_resolution_clock::now();
    #pragma omp parallel
    {
        #pragma omp single
        kway_mergesort_ranks(A, tmp, 0, n, k, threshold);
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    double elapsed = std::chrono::duration<double>(t1 - t0).count();
    print_output(alg, n, k, threshold, n_threads, is_sorted(A), elapsed);

    return 0;
}
