#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>



/* timeval_subtract
 * 
 * http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
 * 
 * Deja en result la diferencia de tiempos entre x e y. */

static int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}



/* start_time_measure
 * 
 * Crea una nueva medición de tiempo.
 * */

void start_time_measure(struct timeval *tv_start)
{
  gettimeofday(tv_start, NULL);
}



/* print_timestamp
 * 
 * Imprime el tiempo transcurrido, junto con un mensaje.
 * Retorna el tiempo transcurrido.
 * */

double print_timestamp(char *message, struct timeval tv_start)
{
  struct timeval tv_now;
  struct timeval delta;
  double elapsed;

  gettimeofday(&tv_now, NULL);
  timeval_subtract(&delta, &tv_now, &tv_start);
  elapsed = delta.tv_sec + delta.tv_usec/1000000.0;
  printf("[%lf] %s", elapsed, message);
  
  return elapsed;
}
