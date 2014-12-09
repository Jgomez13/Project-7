#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

typedef struct array {
   int threads;                 /*number of threads */
   int left_over;               /*if the #of ele doesnt divide evenly we neet to account */
   int count;                   /*which current thread */
   int length;                  /*the size of the current threads array */
   int MAX;                     /*the Max */
   int *random_ints;            /*the array of random integers */
} Array;

struct timeval tv_delta(struct timeval start, struct timeval end) {
   struct timeval delta = end;

   delta.tv_sec -= start.tv_sec;
   delta.tv_usec -= start.tv_usec;
   if (delta.tv_usec < 0) {
      delta.tv_usec += 1000000;
      delta.tv_sec--;
   }

   return delta;
}

static int SUM = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
   /*when the current thread is the last one */
   if (p->count == (p->threads - 1)) {
      /*the last thread will have the left over elements */
      p->length += p->left_over;
   }
   pthread_mutex_unlock(&mutex);
   p = NULL;

   return NULL;
}

void *get_sum(void *args) {
   Array *p = (Array *) args;
   int i = 0;

   pthread_mutex_lock(&mutex);
   for (i = 0; i < p->length; i++) {
      SUM += p->random_ints[i];
   }
   p->random_ints += p->length;
   p->count++;
   /*when the current thread is the last one */
   if (p->count == (p->threads - 1)) {
      /*the last thread will have the left over elements */
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
   Array *pineapple = calloc(1, sizeof(Array));
   int *restore, elements = atoi(argv[1]), num_threads = atoi(argv[2]), seed =
      atoi(argv[3]), task = atoi(argv[4]), i = 0, expected_max =
      0, expected_sum = 0;
   char print = *argv[5];
   pthread_t *threads = calloc(num_threads, sizeof(pthread_t));
   struct rusage start_ru, end_ru;
   struct timeval start_wall, end_wall;
   struct timeval diff_ru_utime, diff_wall, diff_ru_stime;

   pineapple->random_ints = calloc(elements, sizeof(int));
   pineapple->length = elements / num_threads;
   pineapple->MAX = 0;
   pineapple->count = 0;
   pineapple->threads = num_threads;
   pineapple->left_over = elements % num_threads;
   /*use restore to free because i move original array */
   restore = pineapple->random_ints;

   srand(seed);
   /*making the array*/
   for (i = 0; i < elements; i++) {
      int random = rand() % 100;
      pineapple->random_ints[i] = random;
      expected_sum += random;
      if (random > expected_max) {
         expected_max = random;
      }

   }

   if (task == 1) {
       /*MAX*/
      /*start timing*/ 
      gettimeofday(&start_wall, NULL);
      getrusage(RUSAGE_SELF, &start_ru);
      /*making and killing threads*/
      pthread_mutex_init(&mutex, NULL);
      for (i = 0; i < num_threads; i++) {
         pthread_create(&threads[i], NULL, get_max, pineapple);
      }
      for (i = 0; i < num_threads; i++) {
         pthread_join(threads[i], NULL);
      }
      pthread_mutex_destroy(&mutex);
      /*end timing*/
      gettimeofday(&end_wall, NULL);
      getrusage(RUSAGE_SELF, &end_ru);
      /*computing differences*/
      diff_ru_utime = tv_delta(start_ru.ru_utime, end_ru.ru_utime);
      diff_ru_stime = tv_delta(start_ru.ru_stime, end_ru.ru_stime);
      diff_wall = tv_delta(start_wall, end_wall);
      if (print == 'Y') {
         printf("%d\t our_MAX\n%d\t expected\n", pineapple->MAX,
                expected_max);
         printf("User time: %ld.%06ld\n", diff_ru_utime.tv_sec,
                diff_ru_utime.tv_usec);
         printf("System time: %ld.%06ld\n", diff_ru_stime.tv_sec,
                diff_ru_stime.tv_usec);
         printf("Wall time: %ld.%06ld\n", diff_wall.tv_sec,
                diff_wall.tv_usec);
      }
   } else {
      /*SUM*/
      /*start timing*/
      gettimeofday(&start_wall, NULL);
      getrusage(RUSAGE_SELF, &start_ru);
      /*making and killing threads*/
      pthread_mutex_init(&mutex, NULL);
      for (i = 0; i < num_threads; i++) {
         pthread_create(&threads[i], NULL, get_sum, pineapple);
      }
      for (i = 0; i < num_threads; i++) {
         pthread_join(threads[i], NULL);
      }
      pthread_mutex_destroy(&mutex);
      /*end timing*/
      gettimeofday(&end_wall, NULL);
      getrusage(RUSAGE_SELF, &end_ru);
      /*computing differences*/
      diff_ru_utime = tv_delta(start_ru.ru_utime, end_ru.ru_utime);
      diff_ru_stime = tv_delta(start_ru.ru_stime, end_ru.ru_stime);
      diff_wall = tv_delta(start_wall, end_wall);

      if (print == 'Y') {
         printf("%d\t our_SUM\n%d\t expected\n", SUM, expected_sum);
         printf("User time: %ld.%06ld\n", diff_ru_utime.tv_sec,
                diff_ru_utime.tv_usec);
         printf("System time: %ld.%06ld\n", diff_ru_stime.tv_sec,
                diff_ru_stime.tv_usec);
         printf("Wall time: %ld.%06ld\n", diff_wall.tv_sec,
                diff_wall.tv_usec);
      }
   }
   free(restore);
   restore = NULL;
   pineapple->random_ints = NULL;
   free(pineapple);
   pineapple = NULL;
   free(threads);
   threads = NULL;
   return 0;
}
