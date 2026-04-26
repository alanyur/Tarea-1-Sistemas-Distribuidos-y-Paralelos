// Idea de la implementacion:
// https://www.geeksforgeeks.org/dsa/merge-k-sorted-arrays/
// https://en.wikipedia.org/wiki/K-way_merge_algorithm

/*
 *
 * Divide el arreglo en k subarreglos, los ordena recursivamente
 * y los fusiona usando un min-heap (priority_queue).
 *
 * Compilar:
 *   g++ -O2 -std=c++17 -o kway_seq kway_mergesort_seq.cpp
 *
 * Ejecutar:
 *   ./kway_seq <n> <k> [threshold]
 *   Ejemplo: ./kway_seq 1048576 4 1024
 */

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <algorithm>
#include <climits>
#include <cstring>
#include <cassert>
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
        auto [val, arr, idx] = pq.top();
        pq.pop();
        tmp[out++] = val;

        if (idx + 1 < boundaries[arr + 1])
            pq.push({A[idx + 1], arr, idx + 1});
    }

    std::memcpy(A.data() + lo, tmp.data() + lo, (hi - lo) * sizeof(int));
}


void kway_mergesort(std::vector<int>& A, std::vector<int>& tmp,
                    int lo, int hi, int k, int threshold)
{
    if (hi - lo <= threshold) {
        std::sort(A.begin() + lo, A.begin() + hi);
        return;
    }

    int n = hi - lo;
    std::vector<int> boundaries(k + 1);
    boundaries[0] = lo;
    for (int i = 1; i <= k; ++i)
        boundaries[i] = lo + (long long)i * n / k;

    for (int i = 0; i < k; ++i)
        kway_mergesort(A, tmp, boundaries[i], boundaries[i + 1], k, threshold);

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
    int threshold = (argc > 3) ? std::stoi(argv[3]) : 1024;
    std::string alg = "k-way_seq";
    std::cerr << alg << " n=" << n
              << " k=" << k << " threshold=" << threshold << "\n";

    auto A = generate_random(n);
    std::vector<int> tmp(n);

    auto t0 = std::chrono::high_resolution_clock::now();
    kway_mergesort(A, tmp, 0, n, k, threshold);
    auto t1 = std::chrono::high_resolution_clock::now();

    double elapsed = std::chrono::duration<double>(t1 - t0).count();
    print_output(alg, n, k, threshold, UNDEFINED_PARAM, is_sorted(A), elapsed);


    return 0;
}
