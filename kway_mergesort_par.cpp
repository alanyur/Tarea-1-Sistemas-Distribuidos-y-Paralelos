// Idea de la implementacion:
// https://avcourt.github.io/tiny-cluster/2019/03/08/merge_sort.html

/**
 * k-way MergeSort - Versión Paralela (División paralela, merge secuencial)
 *
 * Cada subdivisión recursiva se lanza como una tarea OpenMP independiente.
 * El merge sigue siendo secuencial (O(n log k)) pero la fase de sort
 * aprovecha todos los cores disponibles.
 *
 * Compilar:
 *   g++ -O2 -std=c++17 -fopenmp -o kway_par kway_mergesort_par.cpp
 *
 * Ejecutar:
 *   $env:OMP_NUM_THREADS=<p>; ./kway_par <n> <k> [threshold]
 *   Ejemplo: $env:OMP_NUM_THREADS=8; ./kway_par 4194304 4 4096
 */

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <algorithm>
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


void kway_merge(std::vector<int>& A,
                const std::vector<int>& boundaries,
                std::vector<int>& tmp,
                int lo, int hi)
{
    int k = (int)boundaries.size() - 1;
    std::priority_queue<HeapNode, std::vector<HeapNode>, MinCmp> pq;

    for (int i = 0; i < k; ++i) {
        if (boundaries[i] < boundaries[i + 1])
            pq.push({A[boundaries[i]], i, boundaries[i]});
    }

    int out = lo;
    while (!pq.empty()) {
        auto node = pq.top(); pq.pop();
        tmp[out++] = node.value;
        int next = node.elem_idx + 1;
        if (next < boundaries[node.array_idx + 1])
            pq.push({A[next], node.array_idx, next});
    }

    std::memcpy(A.data() + lo, tmp.data() + lo, (hi - lo) * sizeof(int));
}


void kway_mergesort_par(std::vector<int>& A, std::vector<int>& tmp,
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
            kway_mergesort_par(A, tmp, b_lo, b_hi, k, threshold);
        }
    }

    #pragma omp taskwait
    kway_merge(A, boundaries, tmp, lo, hi);
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
    std::string alg = "k-way_paralelo";
    int n_threads = omp_get_max_threads();

    std::cerr << "[k-way MergeSort Paralelo] n=" << n
              << " k=" << k << " threshold=" << threshold
              << " threads=" << n_threads << "\n";

    auto A = generate_random(n);
    std::vector<int> tmp(n);

    auto t0 = std::chrono::high_resolution_clock::now();
    #pragma omp parallel
    {
        #pragma omp single
        kway_mergesort_par(A, tmp, 0, n, k, threshold);
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    double elapsed = std::chrono::duration<double>(t1 - t0).count();
    print_output(alg, n, k, threshold, n_threads, is_sorted(A), elapsed);

    return 0;
}
