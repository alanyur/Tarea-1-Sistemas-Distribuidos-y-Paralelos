#pragma once
#include <string>
#include <iostream>


#define UNDEFINED_PARAM -1

/*
    Funcion para facilitar el analisis de resultados
    con un script de python
    
    formato: nombre_algoritmo,n,k,threshold,threads,is_sorted,tiempo_segundos.
    -1 indica no definido o no aplica.
*/


void print_output(std::string alg, int n, int k, int threshold, int threads, bool sorted, double time)
{
    std::cout << alg << ","
              << n << ","
              << k << ","
              << threshold << ","
              << threads << ","
              << sorted << ","
              << time << std::endl;
}