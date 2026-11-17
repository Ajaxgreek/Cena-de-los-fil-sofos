/*
 * Problema de la cena de los filosofos
 * Implementacion con hilos POSIX (pthread) y mutex.
 *
 * Para evitar un interbloqueo, el filosofo 0 toma primero el tenedor
 * de su derecha y despues el de su izquierda. Los demas toman primero
 * el de su izquierda y despues el de su derecha. Esta asimetria rompe
 * la espera circular.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_FILOSOFOS 5
#define COMIDAS_POR_FILOSOFO 3

pthread_mutex_t tenedores[NUM_FILOSOFOS];

void *filosofo(void *arg)
{
    int id = *(int *)arg;
    int izquierdo = id;
    int derecho = (id + 1) % NUM_FILOSOFOS;

    for (int comida = 1; comida <= COMIDAS_POR_FILOSOFO; comida++) {
        printf("Filosofo %d esta pensando.\n", id);
        usleep(100000);

        if (id == 0) {
            pthread_mutex_lock(&tenedores[derecho]);
            printf("Filosofo %d toma el tenedor %d (derecho).\n", id, derecho);

            pthread_mutex_lock(&tenedores[izquierdo]);
            printf("Filosofo %d toma el tenedor %d (izquierdo).\n", id, izquierdo);
        } else {
            pthread_mutex_lock(&tenedores[izquierdo]);
            printf("Filosofo %d toma el tenedor %d (izquierdo).\n", id, izquierdo);

            pthread_mutex_lock(&tenedores[derecho]);
            printf("Filosofo %d toma el tenedor %d (derecho).\n", id, derecho);
        }

        printf("Filosofo %d esta COMIENDO (%d/%d).\n",
               id, comida, COMIDAS_POR_FILOSOFO);
        usleep(150000);

        pthread_mutex_unlock(&tenedores[izquierdo]);
        pthread_mutex_unlock(&tenedores[derecho]);
        printf("Filosofo %d dejo ambos tenedores.\n", id);
    }

    printf("Filosofo %d termino de comer.\n", id);
    return NULL;
}

int main(void)
{
    pthread_t hilos[NUM_FILOSOFOS];
    int ids[NUM_FILOSOFOS];

    printf("=== PROBLEMA DE LA CENA DE LOS FILOSOFOS ===\n");
    printf("Hay %d filosofos y cada uno comera %d veces.\n\n",
           NUM_FILOSOFOS, COMIDAS_POR_FILOSOFO);

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        if (pthread_mutex_init(&tenedores[i], NULL) != 0) {
            fprintf(stderr, "Error al inicializar el tenedor %d.\n", i);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        ids[i] = i;
        if (pthread_create(&hilos[i], NULL, filosofo, &ids[i]) != 0) {
            fprintf(stderr, "Error al crear el hilo del filosofo %d.\n", i);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_mutex_destroy(&tenedores[i]);
    }

    printf("\nTodos los filosofos terminaron. Programa finalizado.\n");
    return EXIT_SUCCESS;
}
