#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

static int SUM = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct array {
   int threads;/*number of threads*/
   int left_over;/*if the #of ele doesnt divide evenly we neet to account*/
   int count; /*which current thread*/
   int length; /*the size of the current threads array*/
   int MAX; /*the Max*/
   int *random_ints; /*the array of random integers*/
} Array;

void *get_max(void *args) {
   Array *p = (Array *) args;
   int i = 0;

   pthread_mutex_lock(&mutex);
   for (i = 0; i < p->length; i++) {
      if (p->random_ints[i] > p->MAX) {
         p->MAX = p->random_ints[i];
      }
   }
   p->random_ints += p->length;
   p->count++;
   /*when the current thread is the last one*/
   if (p->count == (p->threads - 1)) {
      /*the last thread will have the left over elements*/
      p->length += p->left_over;
   }
   pthread_mutex_unlock(&mutex);
   p = NULL;
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
   Array *pineapple = calloc(1,sizeof(Array));
   int *restore, elements = atoi(argv[1]), num_threads = atoi(argv[2]), seed =
      atoi(argv[3]), task = atoi(argv[4]), i = 0, expected = 0;
   char print = *argv[5];
   pthread_t *threads = calloc(num_threads, sizeof(pthread_t));

   pineapple->random_ints = calloc(elements, sizeof(int));
   pineapple->length = elements / num_threads;
   pineapple->MAX = 0;
   pineapple->count = 0;
   pineapple->threads = num_threads;
   pineapple->left_over = elements % num_threads;
   /*use restore to free because i move original array*/
   restore = pineapple->random_ints;

   srand(seed);

   for (i = 0; i < elements; i++) {
      int random = rand();
      pineapple->random_ints[i] = random;
      if (random > expected) {
         expected = random;
      }
   }

   if (task == 1) {
       /*MAX*/ 
      pthread_mutex_init(&mutex, NULL);
      for (i = 0; i < num_threads; i++) {
         pthread_create(&threads[i], NULL, get_max, pineapple);
      }
      for (i = 0; i < num_threads; i++) {
         pthread_join(threads[i], NULL);
      }
      pthread_mutex_destroy(&mutex);
      if(print == 'Y') {
         printf("%d\t our_MAX\n%d\t expected\n", pineapple->MAX, expected);
      }
   } else {
      /* SUM */
      SUM = 0;
   }
   free(restore);
   restore = NULL;
   pineapple->random_ints = NULL;
   free(pineapple);
   pineapple = NULL;
   return 0;
}
