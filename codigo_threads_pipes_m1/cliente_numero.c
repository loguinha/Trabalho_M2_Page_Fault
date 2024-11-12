#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define PIPE_NUMERO "/tmp/pipe_numero"
#define PIPE_NUMERO_RESP "/tmp/pipe_numero_resp"

int main() {
    char buffer[256];
    int fd = open(PIPE_NUMERO, O_WRONLY);
    int fd_resp = open(PIPE_NUMERO_RESP, O_RDONLY);

    if (fd == -1 || fd_resp == -1) {
        perror("Erro ao abrir o pipe");
        exit(1);
    }

    int contador = 1;

    while (1) {
        // Envia o número com o delimitador '\n' para facilitar a leitura no servidor
        snprintf(buffer, sizeof(buffer), "%d\n", contador++);
        if (write(fd, buffer, strlen(buffer)) == -1) {
            perror("Erro ao escrever no pipe");
            break;
        }
        printf("Enviado: %s", buffer);

        // Lê a resposta do servidor
        int bytes_lidos = read(fd_resp, buffer, sizeof(buffer) - 1);
        if (bytes_lidos > 0) {
            buffer[bytes_lidos] = '\0'; // Garantir que o buffer seja uma string válida
            printf("Recebido do servidor: %s\n", buffer);
        } else {
            perror("Erro ao ler a resposta do servidor");
            break;
        }

        sleep(2);
    }

    close(fd);
    close(fd_resp);
    return 0;
}
