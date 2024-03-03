#include <stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include <semaphore.h>

#define MaxCars 5
#define num_threads 3

sem_t empty;
sem_t full;
sem_t mutex;

char * queueParkingLot[MaxCars];
int tail = 0;
int head = 0;

char* menu[10]={"Car1","Car2","Car3","Car4","Car5","Car6","Car7","Car8","Car9","Car10"};

void*producer(void *args){
    char* car = malloc(100 * sizeof (char));
    int producer_ID = *(int *)args;
    int cars_produced = 0;

    do{
        sem_wait(&empty);
        sem_wait(&mutex);

        car = menu[rand()%10];
        queueParkingLot[tail] = car;
        printf("Producer %d: ADDED item %s at %d \n ", producer_ID, car, tail);
        tail = (tail + 1) % MaxCars;
        cars_produced++;

        sem_post(&mutex);
        sem_post(&full);

    } while(cars_produced < MaxCars);
}

void*consumer(void *args){

    char* car = malloc(100 * sizeof (char));
    int consumer_ID = *(int *)args;
    int cars_consumed = 0;

    do{
        sem_wait(&full);
        sem_wait(&mutex);

        car = queueParkingLot[head];
        printf("Consumer %d: Removed item %s at %d \n ",consumer_ID, car, head);
        head = (head + 1) % MaxCars;

        cars_consumed++;
        sem_post(&mutex);
        sem_post(&empty);

    } while(cars_consumed < MaxCars);

}
int main() {

    sem_init(&full,0,0);
    sem_init(&empty,0,MaxCars);
    sem_init(&mutex,0,1);

    pthread_t *parkLot = malloc(num_threads * sizeof(*parkLot));
    pthread_t *barrier = malloc(num_threads * sizeof(*barrier));
    int *thread_id = malloc(num_threads * sizeof(*thread_id));

    int error;
    if (sem_init(&full,0,0) != 0) {
        printf("\n full init has failed\n");
        return 1;
    }
    if (sem_init(&empty,0,MaxCars) != 0) {
        printf("\n empty init has failed\n");
        return 1;
    }
    if (sem_init(&mutex,0,1) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    for (int i = 0; i < num_threads; i++){
        thread_id[i] = i;
        error = pthread_create(&barrier[i], NULL, &producer, &thread_id[i]);
        if (error != 0)
            printf("\nThread can't be created :[%s]", strerror(error));
    }

    for (int i = 0; i < num_threads; i++){
        thread_id[i] = i;
        error = pthread_create(&parkLot[i], NULL, &consumer, &thread_id[i]);
        if (error != 0)
            printf("\nThread can't be created :[%s]", strerror(error));
    }

    for (int i = 0; i < num_threads; i++)
        pthread_join(barrier[i], NULL);

    for (int i = 0; i < num_threads; i++)
        pthread_join(parkLot[i], NULL);


    free(barrier);
    free(parkLot);
    free(thread_id);

    sem_unlink(&full);
    sem_unlink(&empty);
    sem_unlink(&mutex);

    sem_close(&full);
    sem_close(&empty);
    sem_close(&mutex);

    return 0;
}
