#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // Funções do SO (como open, close, etc...)
#include <fcntl.h>  // Para usar arquivos e pipes
#include <string.h>  // Uso de strings

// Caminhos dos pipes
#define PIPE_STRING "/tmp/pipe_string"
#define PIPE_NUMERO "/tmp/pipe_numero"

// Função que manda solicitações de string pro servidor
void send_string_request(const char *mensagem) {
    int fd_string = open(PIPE_STRING, O_WRONLY);  // Abertura do pipe para escrita

    // Caso dê ERRO:
    if (fd_string == -1) {
        perror("ERRO ao abrir o pipe de string.\n");
        exit(1);
    }

    // Coloca delimitador '\n' no final da mensagem
    char mensagem_com_delimitador[256];
    snprintf(mensagem_com_delimitador, sizeof(mensagem_com_delimitador), "%s\n", mensagem);  // + "\n"

    // Escreve no pipe considerando o delimitador '\n'
    write(fd_string, mensagem_com_delimitador, strlen(mensagem_com_delimitador));
    close(fd_string);
    printf("STRING ENVIADA!!!!!!\n");
}

// Função que manda solicitações de número pro servidor
void send_number_request(const char *numero) {
    int fd_numero = open(PIPE_NUMERO, O_WRONLY);  // Abertura do pipe para escrita

    // Caso dê ERRO:
    if (fd_numero == -1) {
        perror("ERRO ao abrir o pipe de numero.\n"); 
        exit(1);
    }

    // Coloca delimitador '\n' no final do número
    char numero_com_delimitador[256];
    snprintf(numero_com_delimitador, sizeof(numero_com_delimitador), "%s\n", numero);  // + "\n"

    // Escreve no pipe considerando o delimitador '\n'
    write(fd_numero, numero_com_delimitador, strlen(numero_com_delimitador));
    close(fd_numero);
    printf("NUMERO ENVIADO!!!!!!!\n");
}

int main() {

    // Armazenamento dos inputs do usuário
    char option[10];
    char input[256];

    // Menu
    printf("\n1 - string\n2 - para numero\nEscolha que input sera: ");
    fgets(option, sizeof(option), stdin);  // Leitura do input do menu

    // STRING
    if (option[0] == '1') {
        printf("Digite a string que sera enviada: ");
        fgets(input, sizeof(input), stdin);  // Leitura do input (string)
        send_string_request(input);  // Chama função que envia a string ao servidor

    // NUMERO
    } else if (option[0] == '2') { 
        printf("Digite o numero que sera enviado: ");
        fgets(input, sizeof(input), stdin);  // Leitura do input (numero)
        send_number_request(input);  // Chama função que envia o número ao servidor

    // DEFAULT
    } else {
        printf("\nOPCAO INVALIDA!\n");
    }

    return 0;
}