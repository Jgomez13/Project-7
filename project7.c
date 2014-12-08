#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

static int SUM = 0;
static sem_t mutex;

typedef struct array {
   int length;
   int MAX;
   int *random_ints;
} Array;

void *get_max(void *args) {
   int i = 0;
   Array *p = (Array *)args;
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
   Array *pineapple = malloc(sizeof(Array)) ;
    
   int elements = atoi(argv[1]), num_threads = atoi(argv[2]), left_over =
      elements % num_threads, seed = atoi(argv[3]), task = atoi(argv[4]), i = 0, maxh=-1;

   pthread_t *threads = calloc(num_threads, sizeof(pthread_t));

   pineapple->random_ints = calloc(elements, sizeof(int));
   pineapple->length = elements / num_threads;
   pineapple->random_ints= calloc(1, sizeof(Array));
   sem_init(&mutex, 0, 1);

   srand(seed);

   for (i = 0; i < elements; i++) {
      int random = rand();
      if (random > maxh){
         maxh = random;
      }
      pineapple->random_ints[i] = random;
      printf("random %d\n", random);
   }
   printf("maxh :%d\n", maxh);
   if (task == 1) {
       /*MAX*/ 
      for (i = 0; i < num_threads; i++) {
         if(i == (num_threads - 2)) {
             pineapple->length += left_over;
         }
          pthread_create(&threads[i], NULL, get_max, pineapple);
      }
      for (i = 0; i < num_threads; i++) {
         pthread_join(threads[i], NULL);
      }
      printf("MAX %d\n", pineapple->MAX);

   } else {
      /* SUM */
      SUM = 0;
   }
   free(pineapple-> random_ints);
   free(pineapple);
   return 0;
}
