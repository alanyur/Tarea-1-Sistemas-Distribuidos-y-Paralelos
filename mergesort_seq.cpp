/// Basado en implementacion de geeksforgeeks.org
/// https://www.geeksforgeeks.org/dsa/merge-sort/

#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>
#include "output_helper.hpp"

void merge(std::vector<int>& arr, std::vector<int>& temp, int left, int mid, int right)
{
    int i = left;
    int j = mid;
    int idx = left;
    
    while (i < mid && j < right)
    {
        if (arr[i] <= arr[j])
        {
            temp[idx] = arr[i++];
        } else {
            temp[idx] = arr[j++];
        }
        idx++;
    }

    // Nos aseguramos que no falte ningun elemento
    while (i < mid) {
        temp[idx++] = arr[i++];
    }
    while (j < right) {
        temp[idx++] = arr[j++];
    }

    for (auto k = left; k < right; k++)
    {
        arr[k] = temp[k];
    }

}

void mergesort_secuencial(std::vector<int>& arr, std::vector<int>& temp, int left, int right, int threshold)
{
    if (right - left <= threshold)
    {
        std::sort(arr.begin() + left, arr.begin() + right);
        return;
    }
    
    int mid = left + (right - left) / 2;
    mergesort_secuencial(arr, temp, left, mid, threshold);
    mergesort_secuencial(arr, temp, mid, right, threshold);
    merge(arr, temp, left, mid, right);
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
    int threshold = (argc > 2) ? std::stoi(argv[2]) : (1 << 10);
    std::string alg = "secuencial";
    std::cout << alg << " n=" << n 
            << " threshold=" << threshold << std::endl;

    auto A = generate_random(n);
    std::vector<int> tmp(n);

    auto t0 = std::chrono::high_resolution_clock::now();
    mergesort_secuencial(A, tmp, 0, n, threshold);
    auto t1 = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(t1 - t0).count();
    
    print_output(alg, n, UNDEFINED_PARAM, threshold, 1, is_sorted(A), elapsed);
    return 0;
}

