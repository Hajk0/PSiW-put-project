#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define REP_SIZE 10
#define PRODUCTS 10000
#define PRODUCERS_A 5
#define PRODUCERS_B 5
#define CONSUMERS 5

pthread_mutex_t lock;
sem_t aSpace, bSpace, full, lackOfA, lackOfB;


char repository[REP_SIZE];
int firstEmpty = 0, produced = 0;
int aAmount = 0, bAmount = 0;


void *producerA(void *arg)
{
    char product = 'a';

    for (int i = 0; i < PRODUCTS; i++)
    {
        sem_wait(&aSpace);                  // aSpace
        sem_wait(&full);                    // full

        pthread_mutex_lock(&lock);          // lock
        repository[firstEmpty] = product; // dodanie do magazynu
        firstEmpty++;
        aAmount++;
        printf("Komponent a dodany.\t");

        printf("%d %d\n", aAmount, bAmount);
        pthread_mutex_unlock(&lock);        // lock

        sem_post(&lackOfA);                 // lackOfA
    }
    
    return NULL;
}

void *producerB(void *arg)
{
    char product = 'b';

    for (int i = 0; i < PRODUCTS; i++)
    {
        sem_wait(&bSpace);
        sem_wait(&full);

        pthread_mutex_lock(&lock);
        repository[firstEmpty] = product; // dodanie do magazynu
        firstEmpty++;
        bAmount++;
        pthread_mutex_unlock(&lock);
        printf("Komponent b dodany.\t");
        printf("%d %d\n", aAmount, bAmount);

        sem_post(&lackOfB);
    }
    
    return NULL;
}

void *consumer(void *arg)
{
    for (int i = 0; i < PRODUCTS; i++)
    {
        sem_wait(&lackOfA);
        sem_wait(&lackOfB);
        
        int k = 2;                              // 2 bo ostatni item firstEmpty - 1
        pthread_mutex_lock(&lock);
        if (repository[firstEmpty - 1] == 'a')
        {
            repository[firstEmpty - 1] = 'x';   // wyjecie a z magazynu
            while (repository[firstEmpty - k] != 'b')   // szukanie b
            {
                k++;
            }
            while (k != 1)
            {
                repository[firstEmpty - k] = repository[firstEmpty - k + 1];    // wyjecie b i uporzadkowanie magazynu
                k--;
            }
            
        }
        else
        {
            repository[firstEmpty - 1] = 'x';   // wyjecie b z magazynu
            while (repository[firstEmpty - k] != 'a')   // szukanie a
            {
                k++;
            }
            while (k != 1)
            {
                repository[firstEmpty - k] = repository[firstEmpty - k + 1];    // wyjecie a i uporzadkowanie magazynu
                k--;
            }

        }
        
        
        firstEmpty -= 2;
        aAmount--;
        bAmount--;
        produced++;
        pthread_mutex_unlock(&lock);
        printf("Wyprodukowano kolejny produkt, laczna liczba wyprodukowanych: %d\n", produced);
        sem_post(&full);
        sem_post(&full);
        sem_post(&aSpace);
        sem_post(&bSpace);

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

    char a = 'a', b = 'b';
    for (int i = 0; i < REP_SIZE; i++)
    {
        repository[i] = 'x';
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