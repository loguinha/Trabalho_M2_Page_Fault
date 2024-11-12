#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BUFFER_SIZE 256
#define PIPE_NAME "\\\\.\\pipe\\random_num_pipe"

void receive_data()
{
    HANDLE pipe = CreateFile(
        PIPE_NAME,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (pipe == INVALID_HANDLE_VALUE)
    {
        DWORD error_code = GetLastError();
        fprintf(stderr, "Erro ao conectar ao pipe. Codigo de erro: %lu\n", error_code);
        return;
    }

    char buffer[BUFFER_SIZE];
    DWORD bytes_read;
    while (ReadFile(pipe, buffer, BUFFER_SIZE, &bytes_read, NULL))
    {
        printf("Numero recebido: %s\n", buffer);
    }

    CloseHandle(pipe);
}

int main()
{
    printf("Tentando conectar ao servidor...\n");
    receive_data();
    return 0;
}