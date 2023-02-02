#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define REP_SIZE 10
#define PRODUCTS 1000
#define PRODUCERS_A 5
#define PRODUCERS_B 5
#define CONSUMERS 5

pthread_mutex_t lock;
sem_t aSpace, bSpace, full, lackOfA, lackOfB, bufSemA;


char repository[REP_SIZE];
//int firstEmpty = 0, produced = 0;
int produced = 0;
//int aAmount = 0, bAmount = 0;
int empty[REP_SIZE], A[REP_SIZE], B[REP_SIZE];
int nextEmpty = 0, lastEmpty = 0, lastA = 0, firstA = 0, lastB = 0, firstB = 0;


void *producerA(void *arg)
{
    int place;

    for (int i = 0; i < PRODUCTS; i++)
    {

        sem_wait(&aSpace);                  // aSpace
        sem_wait(&full);                    // full

        pthread_mutex_lock(&lock);          // lock
        place = empty[nextEmpty];
        nextEmpty = (nextEmpty + 1) % REP_SIZE;
        pthread_mutex_unlock(&lock);///

        repository[place] = 'a';

        pthread_mutex_lock(&lock);///
        A[lastA] = place;//
        lastA = (lastA + 1) % REP_SIZE;//
        pthread_mutex_unlock(&lock);

        //repository[place] = 'a';
        printf("Komponent a dodany\n");

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

        pthread_mutex_lock(&lock);          // lock
        place = empty[nextEmpty];
        nextEmpty = (nextEmpty + 1) % REP_SIZE;
        pthread_mutex_unlock(&lock);///

        repository[place] = 'b';

        pthread_mutex_lock(&lock);///
        B[lastB] = place;//
        lastB = (lastB + 1) % REP_SIZE;//
        pthread_mutex_unlock(&lock);

        //repository[place] = 'b';
        printf("Komponent b dodany\n");

        sem_post(&lackOfB);                 // lackOfB
    }
    
    return NULL;
}

void *consumer(void *arg)
{
    int productA, productB;
    for (int i = 0; i < PRODUCTS; i++)
    {
        sem_wait(&lackOfA);

        pthread_mutex_lock(&lock);
        productA = A[firstA];
        firstA = (firstA + 1) % REP_SIZE;
        pthread_mutex_unlock(&lock);

        repository[productA] = 'x';

        pthread_mutex_lock(&lock);
        empty[lastEmpty] = productA;//
        lastEmpty = (lastEmpty + 1) % REP_SIZE;//
        pthread_mutex_unlock(&lock);

        sem_post(&full);
        sem_post(&aSpace);


        sem_wait(&lackOfB);
        
        pthread_mutex_lock(&lock);
        productB = B[firstB];
        firstB = (firstB + 1) % REP_SIZE;
        pthread_mutex_unlock(&lock);

        repository[productB] = 'x';

        pthread_mutex_lock(&lock);
        empty[lastEmpty] = productB;
        lastEmpty = (lastEmpty + 1) % REP_SIZE;
        pthread_mutex_unlock(&lock);

        sem_post(&full);
        sem_post(&bSpace);

        //repository[productA] = 'x';
        //repository[productB] = 'x';
        produced++;                         // montaz
        printf("Wyprodukowano kolejny produkt, laczna liczba wyprodukowanych: %d\n", produced);

    }
    
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t producersA[PRODUCERS_A], producersB[PRODUCERS_B], consumers[CONSUMERS];
    pthread_mutex_init(&lock, NULL);
    sem_init(&aSpace, 0, REP_SIZE - 1);
    sem_init(&bSpace, 0, REP_SIZE - 1);
    sem_init(&full, 0, REP_SIZE);
    sem_init(&lackOfA, 0, 0);
    sem_init(&lackOfB, 0, 0);
    sem_init(&bufSemA, 0, REP_SIZE);


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


    pthread_mutex_destroy(&lock);
    sem_destroy(&aSpace);
    sem_destroy(&bSpace);
    sem_destroy(&full);
    sem_destroy(&lackOfA);
    sem_destroy(&lackOfB);


    return 0;
}