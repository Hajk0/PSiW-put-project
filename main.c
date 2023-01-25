#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define REP_SIZE 10000
#define PRODUCERS_A 1
#define PRODUCERS_B 1
#define CONSUMERS 1

pthread_mutex_t lock;
pthread_cond_t full, lackOfA, lackOfB;

char repository[REP_SIZE];
int firstEmpty = 0, produced = 0;
int aAmount = 0, bAmount = 0;


void *producer(void *arg)
{
    char product = *((char *)arg);

    for (int i = 0; i < 1000; i++)
    {
        pthread_mutex_lock(&lock);

        while (firstEmpty == REP_SIZE)  // sprawdzanie czy magazyn jest pelny
        {
            pthread_cond_wait(&full, &lock);    // czekanie na pobranie komponentow
        }
        repository[firstEmpty] = product; // dodanie do magazynu
        firstEmpty++;
        printf("Komponent %c dodany.\n", product);

        if (product == 'a')
        {
            pthread_cond_signal(&lackOfA);  // budzenie konsumenta jesli brak a
            aAmount++;
        }
        else
        {
            pthread_cond_signal(&lackOfB);  // budzenie konsumenta jesli brak b
            bAmount++;
        }
        
        pthread_mutex_unlock(&lock);

    }

    return NULL;
}

void *consumer(void *arg)
{
    for (int i = 0; i < 1000; i++)
    {
        int j = 0, a = 0, b = 0;
        pthread_mutex_lock(&lock);

        while ((!a || !b) && j < firstEmpty)    // sprawdzanie czy w magazynie są oba komponenty
        {
            if (repository[j] == 'a')
            {
                a = 1;
            }
            else if (repository[j] == 'b')
            {
                b = 1;
            }
            j++;
        }
        
        while (!a)
        {
            pthread_cond_wait(&lackOfA, &lock); // czekanie na dodanie komponentu a

            j = 0;
            while (j < firstEmpty)
            {
                if (repository[j] == 'a')
                {
                    a = 1;
                    break;
                }
                j++;
            }
            
        }
        while (!b)
        {
            pthread_cond_wait(&lackOfB, &lock); // czekanie na dodanie komponentu b

            j = 0;
            while (j < firstEmpty)
            {
                if (repository[j] == 'b')
                {
                    b = 1;
                    break;
                }
                j++;
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

    char a = 'a', b = 'b';
    for (int i = 0; i < REP_SIZE; i++)
    {
        repository[i] = 'x';
    }


    for (int i = 0; i < PRODUCERS_A; i++)
    {
        pthread_create(&producersA[i], NULL, producer, &a);
    }
    for (int i = 0; i < PRODUCERS_B; i++)
    {
        pthread_create(&producersB[i], NULL, producer, &b);
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


    return 0;
}