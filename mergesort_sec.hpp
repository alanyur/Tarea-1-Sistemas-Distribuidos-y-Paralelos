/// Basado en implementacion de geeksforgeeks.org
/// https://www.geeksforgeeks.org/dsa/merge-sort/

#pragma once

#include <vector>

void __merge(std::vector<int>& arr, std::vector<int>& temp, int left, int mid, int right)
{
    int i = left;
    int j = mid + 1;
    int idx = left;
    
    while (i <= mid && j <= right)
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
    while (i <= mid) {
        temp[idx++] = arr[i++];
    }
    while (j <= right) {
        temp[idx++] = arr[j++];
    }

    for (auto k = left; k <= right; k++)
    {
        arr[k] = temp[k];
    }

}

void __mergesort_recursivo(std::vector<int>& arr, std::vector<int>& temp, int left, int right)
{
    if (left >= right)
    {
        return;
    }
    
    int mid = left + (right - left) / 2;
    __mergesort_recursivo(arr, temp, left, mid);
    __mergesort_recursivo(arr, temp, mid + 1, right);
    __merge(arr, temp,left, mid, right);
}

void mergesort_secuencial(std::vector<int>& arr)
{
    if (arr.size() <= 1)
    {
        return;
    }

    int n = arr.size();
    std::vector<int> temp(n);
    __mergesort_recursivo(arr, temp, 0, n-1);
}

/* Ejemplo

#include "mergesort_sec.hpp"
#include <iostream>
#include <vector>

int main(int argc, const char* args[])
{
    std::vector<int> arr;
    const int n = 4;

    for (auto i = n; i > 0; i--)
    {
        arr.push_back(i);
    }

    std::cout << "Before sorting: ";
    for (const auto& num : arr)
    {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    mergesort_secuencial(arr);

    std::cout << "After sorting: ";
    for (const auto& num : arr)
    {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    return 0;
}

*/