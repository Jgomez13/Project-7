#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

static int SUM = 0;
static sem_t mutex;
typedef struct array {
   int *random_ints;
   int length;
   int MAX;
} Array;

void *get_max(void *args) {
   int i = 0;
   Array *p = (Array *) args;
   for (i = 0; i < p->length; i++) {
      sem_wait(&mutex);
      printf("Checking max from %d against %d\n", p -> MAX, p -> random_ints[i]);
      if (p->random_ints[i] > p->MAX) {
         printf("Replacing max from %d to %d\n", p -> MAX, p -> random_ints[i]);
         p -> MAX = p -> random_ints[i];
      }
      sem_post(&mutex);
   }
   return NULL;
}

int main(int argc, char *argv[]) {
   /*
      argv[1]-elements
      2- threads
      3- seed
      4- task
      5- print

    */
   Array *pineapple = malloc(sizeof(Array));
   int left_over = atoi(argv[1]) % atoi(argv[2]);
   int i = 0;
   pthread_t *threads = calloc(atoi(argv[2]), sizeof(pthread_t));
   pineapple -> random_ints = calloc(atoi(argv[1]), sizeof(int));
   pineapple -> length = atoi(argv[1]) / atoi(argv[2]);
   pineapple -> MAX = 0;
   sem_init(&mutex, 0, 1);

   srand(atoi(argv[3]));

   for (i = 0; i < atoi(argv[1]); i++) {
      pineapple -> random_ints[i] = rand();
   }

   if (atoi(argv[4]) == 1) {
      for (i = 0; i < atoi(argv[2]) - 1; i++) {
         pthread_create(&threads[i], NULL, get_max, pineapple);
      }
      pineapple -> length += left_over;
      pthread_create(&threads[i], NULL, get_max, pineapple);
      for (i = 0; i < atoi(argv[2]) - 1; i++) {
         pthread_join(threads[i], NULL);

      }
      printf("MAX %d\n", pineapple->MAX);
   } else {
      /* SUM */
   }

   return 0;
}
