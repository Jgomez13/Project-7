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
   p->MAX = 0;
   for (i = 0; i < p->length; i++) {
      sem_wait(&mutex);
      if (p->random_ints[i] > p->MAX) {
         p->MAX = p->random_ints[i];
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
   pthread_t *threads = calloc(argv[2], sizeof(pthread_t));
   pineapple -> random_ints = calloc(argv[1], sizeof(int));
   pineapple -> length = atoi(argv[1]) / atoi(argv[2]);
   sem_init(&mutex, 0, 1);

   srand(atoi(argv[3]));

   for (i = 0; i < *argv[1]; i++) {
      pineapple -> random_ints[i] = rand();
   }
   if (*argv[4] == 1) {
      for (i = 0; i < *(argv[2]) - 1; i) {
         pthread_create(&threads[i], NULL, get_max, pineapple);
      }
      pineapple -> length += left_over;
      pthread_create(&threads[i], NULL, get_max, pineapple);
      for (i = 0; i < *(argv[2]) - 1; i) {
         pthread_join(threads[i], NULL);

      }
      printf("MAX %d\n", pineapple->MAX);

   } else {
      /* SUM */
   }

   return 0;
}
