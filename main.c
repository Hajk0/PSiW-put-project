#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define REP_SIZE 100
#define PRODUCTS 10000
#define PRODUCERS_A 5
#define PRODUCERS_B 5
#define CONSUMERS 5

pthread_mutex_t lock;
pthread_cond_t full, lackOfA, lackOfB, lackOfA2, lackOfB2;

char repository[REP_SIZE];
int firstEmpty = 0, produced = 0;
int aAmount = 0, bAmount = 0;


void *producerA(void *arg)
{
    char product = 'a';

    for (int i = 0; i < PRODUCTS; i++)
    {
        pthread_mutex_lock(&lock);
        
        int wait = 1;
        while (wait)
        {
            wait = 0;
            while (firstEmpty == REP_SIZE)
            {
                pthread_cond_wait(&full, &lock);
                wait = 1;
            }
            while (aAmount >= ((REP_SIZE - 0.05 * REP_SIZE) - 2))
            {
                pthread_cond_wait(&lackOfB, &lock);
                wait = 1;
            }
            
        }
        repository[firstEmpty] = product; // dodanie do magazynu
        firstEmpty++;
        printf("Komponent a dodany.\t");

        aAmount++;
        printf("%d %d\n", aAmount, bAmount);

        pthread_cond_signal(&lackOfA);  // budzenie konsumenta jesli brak a
        pthread_mutex_unlock(&lock);
    }
    
    return NULL;
}

void *producerB(void *arg)
{
    char product = 'b';

    for (int i = 0; i < PRODUCTS; i++)
    {
        pthread_mutex_lock(&lock);
        
        int wait = 1;
        while (wait)
        {
            wait = 0;
            while (firstEmpty == REP_SIZE)
            {
                pthread_cond_wait(&full, &lock);
                wait = 1;
            }
            while (bAmount >= ((REP_SIZE - 0.05 * REP_SIZE) - 2))
            {
                pthread_cond_wait(&lackOfA, &lock);
                wait = 1;
            }
            
        }
        repository[firstEmpty] = product; // dodanie do magazynu
        firstEmpty++;
        printf("Komponent a dodany.\t");

        bAmount++;
        printf("%d %d\n", aAmount, bAmount);

        pthread_cond_signal(&lackOfB);  // budzenie konsumenta jesli brak a
        pthread_mutex_unlock(&lock);
    }
    
    return NULL;
}

void *producer(void *arg)
{
    char product = *((char *)arg);

    for (int i = 0; i < PRODUCTS; i++)
    {
        //printf("PROD");
        pthread_mutex_lock(&lock);
        //printf("PROD");
        int wait = 1;
        while (wait) // petla aby po warunkowym przelaczeniu kontekstu ponownie sprawdzic wszystkie warunki
        {
            wait = 0;
            while (firstEmpty == REP_SIZE)  // sprawdzanie czy magazyn jest pelny
            {
                pthread_cond_wait(&full, &lock);    // czekanie na pobranie komponentow
                wait = 1;
            }
            while (product == 'a' && aAmount == REP_SIZE - 1)   // sprawdzenie czy nie zapelnimy magazynu jednym komponentem
            {
                pthread_cond_wait(&lackOfB2, &lock);
                wait = 1;
            }
            while (product == 'b' && bAmount == REP_SIZE - 1)   // -||-
            {
                pthread_cond_wait(&lackOfA2, &lock);
                wait = 1;
            }

        }

        repository[firstEmpty] = product; // dodanie do magazynu
        firstEmpty++;
        printf("Komponent %c dodany.\t", product);

        if (product == 'a')
        {
            aAmount++;
            printf("%d %d\n", aAmount, bAmount);
            pthread_cond_signal(&lackOfA2);
            pthread_cond_signal(&lackOfA);  // budzenie konsumenta jesli brak a
        }
        else
        {
            bAmount++;
            printf("%d %d\n", aAmount, bAmount);
            pthread_cond_signal(&lackOfB2);
            pthread_cond_signal(&lackOfB);  // budzenie konsumenta jesli brak b
        }
        
        pthread_mutex_unlock(&lock);

    }

    return NULL;
}

void *consumer(void *arg)
{
    for (int i = 0; i < PRODUCTS; i++)
    {
        //printf("CONS");
        pthread_mutex_lock(&lock);
        //printf("CONS");
        int wait = 1;
        while (wait)
        {
            wait = 0;
            while (!aAmount)///
            {
                pthread_cond_wait(&lackOfA, &lock); // czekanie na dodanie komponentu a
                wait = 1;
            }
            while (!bAmount)///
            {
                pthread_cond_wait(&lackOfB, &lock); // czekanie na dodanie komponentu b
                wait = 1;
            }
        }
        
        int k = 2;                              // 2 bo ostatni item firstEmpty - 1
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
        printf("Wyprodukowano kolejny produkt, laczna liczba wyprodukowanych: %d\n", produced);
        pthread_cond_signal(&full);             // budzenie producenta jesli magazyn byl pelen
        pthread_mutex_unlock(&lock);

    }
    
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t producersA[PRODUCERS_A], producersB[PRODUCERS_B], consumers[CONSUMERS];
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&lackOfA, NULL);
    pthread_cond_init(&lackOfB, NULL);
    pthread_cond_init(&lackOfA2, NULL);
    pthread_cond_init(&lackOfB2, NULL);

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
    pthread_cond_destroy(&lackOfA);
    pthread_cond_destroy(&lackOfB);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&lackOfA2);
    pthread_cond_destroy(&lackOfB2);


    return 0;
}