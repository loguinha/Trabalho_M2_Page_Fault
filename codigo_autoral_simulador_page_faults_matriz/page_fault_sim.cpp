#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <windows.h> 
#include <unistd.h>
#include <cstdlib>

using namespace std;

void simulatePageFaultsWithMatrix(size_t matrixSize) {
    // Cria uma matriz 2D grande para simular uma carga de memória
    vector<vector<int>> matrix(matrixSize, vector<int>(matrixSize, 1));

    // Simula acessos para gerar page faults
    for (size_t i = 0; i < matrixSize; i++) {
        for (size_t j = 0; j < matrixSize; j += 1024) { // Acessa elementos espaçados
            matrix[i][j] = i + j;
        }
    }
}

void randomAccessMatrix(size_t matrixSize) {
    // Cria uma matriz 2D grande para simular uma carga de memória
    vector<vector<int>> matrix(matrixSize, vector<int>(matrixSize, 1));

    // Acessos aleatórios na matriz para gerar page faults
    for (size_t i = 0; i < matrixSize * matrixSize; i++) {
        size_t row = rand() % matrixSize;
        size_t col = rand() % matrixSize;
        matrix[row][col] += 1;
    }
}

int main() {
    size_t matrixSize;

    cout << "Digite o tamanho da matriz quadrada: ";
    cin >> matrixSize;

    cout << "Executando simulação com acesso sequencial...\n";
    simulatePageFaultsWithMatrix(matrixSize);

    cout << "Executando simulação com acesso aleatório...\n";
    randomAccessMatrix(matrixSize);

    cout << "Simulação concluída!\n";

    Sleep(29990);

    return 0;
}
