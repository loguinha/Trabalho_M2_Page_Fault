import random
import time
import os

def simulatePageFaultsWithMatrix(matrixSize):
    # Cria uma matriz 2D grande para simular uma carga de memória
    matrix = [[1 for _ in range(matrixSize)] for _ in range(matrixSize)]

    # Simula acessos para gerar page faults
    for i in range(matrixSize):
        for j in range(0, matrixSize, 1024):  # Acessa elementos espaçados
            matrix[i][j] = i + j

def randomAccessMatrix(matrixSize):
    # Cria uma matriz 2D grande para simular uma carga de memória
    matrix = [[1 for _ in range(matrixSize)] for _ in range(matrixSize)]

    # Acessos aleatórios na matriz para gerar page faults
    for _ in range(matrixSize * matrixSize):
        row = random.randint(0, matrixSize - 1)
        col = random.randint(0, matrixSize - 1)
        matrix[row][col] += 1

def main():
    matrixSize = int(input("Digite o tamanho da matriz quadrada: "))

    print("Executando simulação com acesso sequencial...")
    simulatePageFaultsWithMatrix(matrixSize)

    print("Executando simulação com acesso aleatório...")
    randomAccessMatrix(matrixSize)

    print("Simulação concluída!")

    time.sleep(29.99)

if __name__ == "__main__":
    main()