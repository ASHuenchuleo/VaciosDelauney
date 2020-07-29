#include <sys/time.h>

void start_time_measure(struct timeval *tv_start);
double print_timestamp(char *message, struct timeval tv_start);
