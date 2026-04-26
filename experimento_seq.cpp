#include <algorithm>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

/*
    BENCHMARK SECUENCIAL, hecho con ayuda de chatgpt para implementar los tiempos de ejecucion, comparacion con std::sort y escritura de resultados en CSV
*/

#define main kway_mergesort_seq_original_main
#define generate_random kway_mergesort_seq_generate_random
#define is_sorted kway_mergesort_seq_is_sorted
#include "kway_mergesort_seq.cpp"
#undef main
#undef generate_random
#undef is_sorted

#define main mergesort_seq_original_main
#define generate_random mergesort_seq_generate_random
#define is_sorted mergesort_seq_is_sorted
#include "mergesort_seq.cpp"
#undef main
#undef generate_random
#undef is_sorted

namespace {

// Convierte exponentes a tamaños reales (2^n)
std::vector<int> build_sizes_from_exponents(const std::vector<int>& exponents)
{
    std::vector<int> sizes;
    for (int exponent : exponents) {
        sizes.push_back(1 << exponent);
    }
    return sizes;
}

// generar arreglo de enteros aleatorios 
std::vector<int> generate_input(int n, unsigned seed)
{
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, n * 10);

    std::vector<int> data(n);
    for (int& x : data) {
        x = dist(rng);
    }
    return data;
}

// calcular el tiempo transcurrido en segundos
double elapsed_seconds(const std::chrono::high_resolution_clock::time_point& start,
                       const std::chrono::high_resolution_clock::time_point& end)
{
    return std::chrono::duration<double>(end - start).count();
}

void write_bool(std::ofstream& out, bool value)
{
    out << (value ? 1 : 0);
}

// calcular promedio
double average(const std::vector<double>& values)
{
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

} 

int main()
{
    try {
        const std::string output_file = "experimento_seq.csv";

        // 5 repeticiones por prueba para obtener tiempos promedio 
        const int repetitions = 5;

        // umbral fijo para los algoritmos
        const int threshold = 1024;

        // exponentes 2^20, 2^22, 2^24, 2^26
        const std::vector<int> n_exponents{20, 22, 24, 26};

        // Valores de k para k-way mergesort
        const std::vector<int> k_values{2, 4, 8, 16};
        const auto sizes = build_sizes_from_exponents(n_exponents);

        // Crear archivo CSV
        std::ofstream csv(output_file);
        if (!csv) {
            throw std::runtime_error("No se pudo abrir el archivo CSV");
        }
        csv << "n_exponent,n,k,threshold,repetitions,"
               "mergesort_avg_seconds,kway_avg_seconds,correcto\n";

        csv << std::fixed << std::setprecision(9);

        // Iterar sobre cada combinación de tamaño y k
        for (size_t i = 0; i < sizes.size(); ++i) {
            int exponent = n_exponents[i];
            int n = sizes[i];

            for (int k : k_values) {

                std::vector<double> merge_times;
                std::vector<double> kway_times;

                bool correcto = true;

                for (int rep = 1; rep <= repetitions; ++rep) {
                    //generar los datos de entrada para prueba, usando una semilla que depende del tamaño, k y repetición para asegurar variedad
                    unsigned seed = 42u + exponent * 1000 + k * 100 + rep;
                    std::vector<int> base = generate_input(n, seed);

                    // validar usando std::sort 
                    std::vector<int> reference = base;
                    std::sort(reference.begin(), reference.end());

                    //mergesort secuencial
                    std::vector<int> merge_data = base;
                    std::vector<int> merge_tmp(n);

                    auto t0 = std::chrono::high_resolution_clock::now();
                    mergesort_secuencial(merge_data, merge_tmp, 0, n, threshold);
                    auto t1 = std::chrono::high_resolution_clock::now();

                    double merge_time = elapsed_seconds(t0, t1);
                    merge_times.push_back(merge_time);

                    //kway mergesort secuencial
                    std::vector<int> kway_data = base;
                    std::vector<int> kway_tmp(n);

                    auto t2 = std::chrono::high_resolution_clock::now();
                    kway_mergesort(kway_data, kway_tmp, 0, n, k, threshold);
                    auto t3 = std::chrono::high_resolution_clock::now();

                    double kway_time = elapsed_seconds(t2, t3);
                    kway_times.push_back(kway_time);

                    //validar resultados
                    correcto = correcto &&
                               mergesort_seq_is_sorted(merge_data) &&
                               kway_mergesort_seq_is_sorted(kway_data) &&
                               (merge_data == reference) &&
                               (kway_data == reference);
                }

                csv << exponent << ","
                    << n << ","
                    << k << ","
                    << threshold << ","
                    << repetitions << ","
                    << average(merge_times) << ","
                    << average(kway_times) << ",";

                write_bool(csv, correcto);
                csv << "\n";
                std::cout << "n=2^" << exponent << " k=" << k << " listo\n";
            }
        }

        std::cout << "\nResultados en experimento_seq.csv\n";

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}