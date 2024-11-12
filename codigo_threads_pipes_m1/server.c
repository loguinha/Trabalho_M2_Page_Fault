#include <stdio.h> 
#include <stdlib.h>
#include <fcntl.h>  // Uso de arquivos e pipes
#include <string.h>  // Uso de strings
#include <unistd.h>  // Funções do SO (read, write, open, close...)
#include <errno.h>  // Tratamento de erros
#include <pthread.h>  // Uso de threads
#include <sys/stat.h>  // Criação de pipes (mkfifo)

// Caminho para os pipes
#define PIPE_STRING "/tmp/pipe_string"
#define PIPE_NUMERO "/tmp/pipe_numero"

#define NUM_THREADS 5  // Número de threads na pool
#define QUEUE_SIZE 15  // Tmanho da fila de tarefas

// Cria o MUTEX e a variável de condição (sincronização de threads)
pthread_mutex_t mutex_fila;  // Protege o acesso à fila de trabalho
pthread_cond_t cond_fila;  // Diz se há trabalho disponível

// TAREFA
typedef struct {
    int tipo;  // 1 = string, 2 = número
    char data[256];  // Conteúdo (dados)
} Tarefa;

// FILA DE TAREFAS
Tarefa fila_trabalho[QUEUE_SIZE];
int fila_fim = 0;  // Posição onde a próxima tarefa é inserida
int posicao_atual_fila = 0;  // Posição da tarefa sendo processada

// Função: adiciona tarefa à fila
void adicionar_tarefa (int tipo, const char* dados) {
    pthread_mutex_lock(&mutex_fila);  // Bloqueia o MUTEX para acessar a fila de forma segura
    
    // Copia tipo/dados e coloca na fila
    fila_trabalho[fila_fim].tipo = tipo;
    strncpy(fila_trabalho[fila_fim].data, dados, sizeof(fila_trabalho[fila_fim].data) - 1); // -1 desconta o "/0"
    fila_fim = (fila_fim + 1) % QUEUE_SIZE;  // Incrementa a posição circularmente
    
    // Alarma as threads de que há uma tarefa e desbloqueia o MUTEX
    pthread_cond_signal(&cond_fila);
    pthread_mutex_unlock(&mutex_fila);
}

// Função: distribuição de trabalho na POOL
void* thread_pool (void* arg) {
    int thread_id = *(int*)arg;  // Identificador da thread

    // Loop infinito (threads sempre aguardam novas tarefas)
    while (1) { 
        pthread_mutex_lock(&mutex_fila);  // bloqueia o MUTEX para ver a fila

        // FILA VAZIA
        while (fila_fim == posicao_atual_fila) {
            pthread_cond_wait(&cond_fila, &mutex_fila);  // Espera alarme de tarefa e desbloqueio MUTEX
        }

        // GARANTE A TAREFA
        Tarefa trabalho = fila_trabalho[posicao_atual_fila];
        posicao_atual_fila = (posicao_atual_fila + 1) % QUEUE_SIZE;  // Incrementa a posição circularmente

        pthread_mutex_unlock(&mutex_fila);  // desbloqueia o MUTEX

        // PROCESSA A TAREFA
        if (trabalho.tipo == 1) {
            printf("Esta thread: %d \nacabou de processar esta STRING: %s\n", thread_id, trabalho.data);
        } else if (trabalho.tipo == 2) {
            printf("Esta thread: %d \nacabou de processar este NUMERO: %s\n", thread_id, trabalho.data);
        }

        sleep(2);  // Tempo de processamento da tarefa (ilustrar paralelismo)
    }

    return NULL;
}

// Função: Lê byte a byte até encontrar o "\n" (delimitador)
void ler_com_delimitador(int fd, int tipo) {
    char buffer[256];
    char temp;
    int index = 0;
    // Loop que lê byte por byte do fd
    while (read(fd, &temp, 1) > 0) {

        // Verifica se achou o \n
        if (temp == '\n') {
            buffer[index] = '\0'; 
            adicionar_tarefa(tipo, buffer);  // Coloca a tarefa na fila
            index = 0;
            break; // sai do loop para ir processar a tarefa

        // vai adicionando bytes lidos ao buffer enquanto não acha o \n    
        } else {
            buffer[index++] = temp;
        }
    }
}


int main() {

    char buffer[256];  // Buffer para leitura de dados dos pipes

    pthread_t threads[NUM_THREADS];  // Vetor de threads
    int thread_ids[NUM_THREADS];  // Vetor de IDs das threads

    // Inicializa MUTEXs e condição
    pthread_mutex_init(&mutex_fila, NULL);
    pthread_cond_init(&cond_fila, NULL);

    // Cria pipes
    mkfifo(PIPE_STRING, 0666);
    mkfifo(PIPE_NUMERO, 0666);

    // Abrir pipes para leitura (não bloqueante)
    int pipe_fd_string = open(PIPE_STRING, O_RDONLY | O_NONBLOCK);
    int pipe_fd_numero = open(PIPE_NUMERO, O_RDONLY | O_NONBLOCK);

    // Cria threads da POOL de threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;  // Atribuição dos IDs
        // Cria thread, colocando-a em função da thread POOL
        pthread_create(&threads[i], NULL, thread_pool, &thread_ids[i]);
    }

    // Loop infinito (sempre ler dados dos pipes)
    while (1) {
    // PIPE STRINGS
    ler_com_delimitador(pipe_fd_string, 1);

    // PIPE NUMEROS
    ler_com_delimitador(pipe_fd_numero, 2);

    sleep(1);  // Pausa (evita sobrecarregar CPU)
}

    // Destrói mutexes e variáveis de condição
    pthread_mutex_destroy(&mutex_fila);
    pthread_cond_destroy(&cond_fila);

    return 0;
}

// Comandos para usar no terminal WSL do VSCode: 

// rm /tmp/pipe_string /tmp/pipe_numero

// gcc server.c -o server -lpthread

// gcc client.c -o client

// gcc -o cliente_string cliente_string.c

// gcc -o cliente_numero cliente_numero.c