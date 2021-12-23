#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int contador = 0, N;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;
pthread_cond_t x_condDois;
bool first = false, second = false;

void *Thread1 (void *arg) {

    pthread_mutex_lock(&x_mutex);
    for (int i=0; i<N; i++) {
        contador++;
        if(contador%100==0){
            first = true;
            second = false;
            pthread_cond_signal(&x_cond);
            while(!second){
                pthread_cond_wait(&x_condDois, &x_mutex);
            }
        }
    }
    pthread_mutex_unlock(&x_mutex);
    pthread_exit(NULL);
}
void *Thread2 (void *arg) {

    pthread_mutex_lock(&x_mutex);
    for(int i =0; i<(N/100); i++){
        while(!first){
            pthread_cond_wait(&x_cond, &x_mutex);
        }
        printf("%d ", contador);
        first = false;
        second = true;
        pthread_cond_signal(&x_condDois);      
    }
    pthread_mutex_unlock(&x_mutex);
    pthread_exit(NULL);
}

int main (int argc, char* argv[]){
    int i; 
    pthread_t threads[2];

    if(argc<2) {
        printf("Digite: %s <valor desejado>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    printf("N: %d\n", N);

    /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init (&x_cond, NULL);
    pthread_cond_init (&x_condDois, NULL);

    /* Cria as threads */

    if(pthread_create(&(threads[1]), NULL, Thread1, NULL)){
        puts("ERRO--pthread_create"); return 3;
    }
    if(pthread_create(&(threads[0]), NULL, Thread2, NULL)){
        puts("ERRO--pthread_create"); return 3;
    }

    /* Espera todas as threads completarem */
    for (i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("\n");

    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);
    pthread_cond_destroy(&x_condDois);
    return 0;
}