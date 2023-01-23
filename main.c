#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define REP_SIZE 50
#define PRODUCERS_A 1
#define PRODUCERS_B 1
#define CONSUMERS 1

pthread_mutex_t lock;
char repository[REP_SIZE];


void *producer(void *arg)
{
    char product = *((char *)arg);

    
}

void *consumer(void *arg)
{

}

int main(int argc, char **argv)
{
    pthread_t producersA[PRODUCERS_A], producersB[PRODUCERS_B], consumers[CONSUMERS];
    pthread_mutex_init(&lock, NULL);

    char a = 'a', b = 'b';


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