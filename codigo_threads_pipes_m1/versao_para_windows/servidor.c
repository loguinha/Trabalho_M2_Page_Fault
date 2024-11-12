#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define BUFFER_SIZE 256
#define PIPE_NAME "\\\\.\\pipe\\random_num_pipe"
#define RUNTIME_SECONDS 10 // Tempo de execução em segundos

// Declaração da função antes da sua chamada
void send_random_num(HANDLE pipe);

// Função que cada thread vai executar para enviar números aleatórios
DWORD WINAPI thread_function(LPVOID pipe)
{
    HANDLE hPipe = *(HANDLE *)pipe;
    while (1)
    {
        send_random_num(hPipe);
        Sleep(500);
    }
    return 0;
}

// Função que envia números aleatórios pelo pipe
void send_random_num(HANDLE pipe)
{
    char buffer[BUFFER_SIZE];
    int num = rand() % 100 + 1; // Número aleatório entre 1 e 100
    snprintf(buffer, BUFFER_SIZE, "%d", num);
    DWORD bytes_written;
    if (WriteFile(pipe, buffer, strlen(buffer) + 1, &bytes_written, NULL))
    {
        printf("Enviando numero: %d\n", num);
    }
    else
    {
        DWORD error_code = GetLastError();
        fprintf(stderr, "Falha ao escrever no pipe. Codigo de erro: %lu\n", error_code);
    }
}

int main()
{
    srand((unsigned int)time(NULL));

    HANDLE pipe = CreateNamedPipe(
        PIPE_NAME,
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        1,
        BUFFER_SIZE,
        BUFFER_SIZE,
        0,
        NULL);

    if (pipe == INVALID_HANDLE_VALUE)
    {
        DWORD error_code = GetLastError();
        fprintf(stderr, "Falha ao criar o Named Pipe. Codigo de erro: %lu\n", error_code);
        return 1;
    }

    printf("Aguardando conexao do cliente...\n");
    ConnectNamedPipe(pipe, NULL);

    int num_threads = 60;
    HANDLE *thread_pool = (HANDLE *)malloc(num_threads * sizeof(HANDLE));

    for (int i = 0; i < num_threads; i++)
    {
        thread_pool[i] = CreateThread(NULL, 0, thread_function, &pipe, 0, NULL);
        if (thread_pool[i] == NULL)
        {
            fprintf(stderr, "Falha ao criar a thread %d. Codigo de erro: %lu\n", i, GetLastError());
            return 1;
        }
    }

    // Executa por 10 segundos
    Sleep(RUNTIME_SECONDS * 1000);

    // Termina todas as threads e limpa os recursos
    for (int i = 0; i < num_threads; i++)
    {
        TerminateThread(thread_pool[i], 0);
        CloseHandle(thread_pool[i]);
    }

    free(thread_pool); // Libera a memória alocada para a pool
    CloseHandle(pipe); // Fecha o pipe

    printf("Programa finalizado.\n");
    return 0;
}