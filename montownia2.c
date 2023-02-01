#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define REP_SIZE 10
#define PRODUCTS 1000
#define PRODUCERS_A 5
#define PRODUCERS_B 5
#define CONSUMERS 5

pthread_mutex_t lockEmpty, lockA, lockB;
sem_t aSpace, bSpace, full, lackOfA, lackOfB;


char repository[REP_SIZE];
int firstEmpty = 0, produced = 0;
int aAmount = 0, bAmount = 0;
int empty[REP_SIZE], A[REP_SIZE], B[REP_SIZE];
int nextEmpty = 0, lastA = 0, lastB = 0;


void *producerA(void *arg)
{
    int place;

    for (int i = 0; i < PRODUCTS; i++)
    {

        sem_wait(&aSpace);                  // aSpace
        sem_wait(&full);                    // full

        pthread_mutex_lock(&lockEmpty);          // lock
        place = empty[nextEmpty];
        nextEmpty = (nextEmpty + 1) % REP_SIZE;
        pthread_mutex_unlock(&lockEmpty);

        repository[place] = 'a';
        printf("Komponent a dodany\n");

        pthread_mutex_lock(&lockEmpty); //A
        A[lastA] = place;
        lastA = (lastA + 1) % REP_SIZE;
        pthread_mutex_unlock(&lockEmpty); //A


        sem_post(&lackOfA);                 // lackOfA
    }
    
    return NULL;
}

void *producerB(void *arg)
{
    int place;

    for (int i = 0; i < PRODUCTS; i++)
    {

        sem_wait(&bSpace);                  // aSpace
        sem_wait(&full);                    // full

        pthread_mutex_lock(&lockEmpty);          // lock
        place = empty[nextEmpty];
        nextEmpty = (nextEmpty + 1) % REP_SIZE;
        pthread_mutex_unlock(&lockEmpty);

        repository[place] = 'b';
        printf("Komponent b dodany\n");

        pthread_mutex_lock(&lockEmpty); //B
        B[lastB] = place;
        lastA = (lastB + 1) % REP_SIZE;
        pthread_mutex_unlock(&lockEmpty); //B


        sem_post(&lackOfB);                 // lackOfA
    }
    
    return NULL;
}

void *consumer(void *arg)
{
    int productA, productB;
    for (int i = 0; i < PRODUCTS; i++)
    {
        sem_wait(&lackOfA);

        pthread_mutex_lock(&lockEmpty);
        productA = A[lastA];
        lastA = (lastA + 1) % REP_SIZE;
        pthread_mutex_unlock(&lockEmpty);

        sem_post(&full);
        sem_post(&aSpace);


        sem_wait(&lackOfB);
        
        pthread_mutex_lock(&lockEmpty);
        productB = B[lastB];
        lastB = (lastB + 1) % REP_SIZE;
        pthread_mutex_unlock(&lockEmpty);

        sem_post(&full);
        sem_post(&bSpace);

        repository[productA] = 'x';
        repository[productB] = 'x';
        produced++;                         // montaz
        printf("Wyprodukowano kolejny produkt, laczna liczba wyprodukowanych: %d\n", produced);

    }
    
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t producersA[PRODUCERS_A], producersB[PRODUCERS_B], consumers[CONSUMERS];
    pthread_mutex_init(&lockEmpty, NULL);
    pthread_mutex_init(&lockA, NULL);
    pthread_mutex_init(&lockB, NULL);
    sem_init(&aSpace, 0, REP_SIZE - 1);
    sem_init(&bSpace, 0, REP_SIZE - 1);
    sem_init(&full, 0, REP_SIZE);
    sem_init(&lackOfA, 0, 0);
    sem_init(&lackOfB, 0, 0);


    for (int i = 0; i < REP_SIZE; i++)
    {
        repository[i] = 'x';
        empty[i] = i;
    }


    for (int i = 0; i < PRODUCERS_A; i++)
    {
        pthread_create(&producersA[i], NULL, producerA, NULL);
    }
    for (int i = 0; i < PRODUCERS_B; i++)
    {
        pthread_create(&producersB[i], NULL, producerB, NULL);
    }
    for (int i = 0; i < CONSUMERS; i++)
    {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }
    

    for (int i = 0; i < PRODUCERS_A; i++)
    {
        pthread_join(producersA[i], NULL);
    }
    for (int i = 0; i < PRODUCERS_B; i++)
    {
        pthread_join(producersB[i], NULL);
    }
    for (int i = 0; i < CONSUMERS; i++)
    {
        pthread_join(consumers[i], NULL);
    }


    pthread_mutex_destroy(&lockEmpty);
    pthread_mutex_destroy(&lockA);
    pthread_mutex_destroy(&lockB);
    sem_destroy(&aSpace);
    sem_destroy(&bSpace);
    sem_destroy(&full);
    sem_destroy(&lackOfA);
    sem_destroy(&lackOfB);


    return 0;
}