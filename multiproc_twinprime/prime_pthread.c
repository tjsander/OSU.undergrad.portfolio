/*
 *	Travis J. Sanders
 *	sandetra@onid.oregonstate.edu
 *	CS344-400
 *	Homework #5
 *
 *	Threaded twin-prime number generator
 *
 *	Link with -lm.
 */
#define _XOPEN_SOURCE 700
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <getopt.h>
#include <sys/types.h>
#include <pthread.h>

/*	Bit array access macros
 *	http://www.tek-tips.com/faqs.cfm?fid=3999
 *	http://stackoverflow.com/questions/22006507/thread-safe-bit-array
 */
#define ISBITSET(x,i) ((x[i>>3] & (1<<(i&7)))!=0)
#define SETBIT(x,i) __sync_or_and_fetch(&x[i>>3], 1U << (i&7));
#define CLEARBIT(x,i) x[i>>3]&=(1<<(i&7))^0xFF;

struct data{
	unsigned int n;
	int * list;
	// int thread_id;
};

int topstack;

pthread_mutex_t mutex_q;
pthread_mutex_t mutex_r;

void *sieve_primes(void *arg);
int find_twins(int* list, unsigned int n, int verbose);

/*	Threaded prime number finder function.
 *	Pass data struct with size, bit array pointer 
 */
void *sieve_primes(void *arg)
{
	unsigned int k;
	unsigned int m, i;

	struct data *d = (struct data*)arg;
	int * list = d->list;
	// int thread_id = d->thread_id;
	unsigned int n = d->n;

	k = 1;

	while (k < sqrt(n)) {
		m = k+1;
		i = 2;

		/*	Critical portion	*/
		pthread_mutex_lock(&mutex_q);
		if (m < topstack) m = topstack+1;
		// printf("Topstack=%d\n", topstack);
		while ((m < n) && ISBITSET(list, m)) m++;
		if (m > topstack) topstack = m;
		// printf("Thread[%d] primebit %u\n", thread_id, m);
		SETBIT(list, m);
		pthread_mutex_unlock(&mutex_q);

		/* Set all multiples as composite */
		while ((long)m*i < n) {
			if (!ISBITSET(list, m*i)) SETBIT(list, m*i);
			i++;
		}

		/*	Second critical portion. Clear prime bit.	*/
		pthread_mutex_lock(&mutex_r);
		CLEARBIT(list, m);
		pthread_mutex_unlock(&mutex_r);
		k = m;
	}
	pthread_exit((void*)0);
}

/*	Finds all twin primes in bit array (list) of size (n) 
 *	-q flag included
 */
int find_twins(int* list, unsigned int n, int quiet)
{
	unsigned int j, t;
	t = 0;
	for (j=3; (j+2 < n); j+=2) {
		if (!ISBITSET(list, j)) {
			if (!ISBITSET(list, (j+2))) {
				if (quiet == 1) printf("%u %u\n", j, j+2);
				t++;
			}
		}
	}
	free(list);
	return t;
}
 
int main(int argc, char *argv[]) 
{
	unsigned int i, c;
	int quiet, verbose, procs;
	unsigned int m;

	struct data* d;
	pthread_t* thread;

	int * list;

	/* DEFAULT VALUES */
	m = UINT_MAX;
	quiet = 1;
	procs = 1;
	verbose = 0;
	topstack = 0;

	/* Parse arguments */
	while ((c = getopt (argc, argv, "vqm:c:")) != -1) {
		switch(c) {
		case 'v':
			verbose = 1;
			break;
		case 'q':
			quiet = 0;
			break;
		case 'm':
			m = atoi(optarg);
			if (m < 10) m = 10; //for safety
			break;
		case 'c':
			procs = atoi(optarg);
			if (c < 1) c = 1; //for safety
			break;
		default:
			break;
		}
	}

	d = malloc(procs * sizeof(struct data));
	thread = malloc(procs * sizeof(pthread_t));
	list = calloc(m, 1);

	pthread_mutex_init(&mutex_q, NULL);
	pthread_mutex_init(&mutex_r, NULL);

	if (verbose == 1) {
		printf("Finding twin primes under m=%u\n", m);
		printf("The square root of m is =%f\n", sqrt(m));
	}

	/* Make threads (modified from lecture code) */
	for (i=0; i<procs; i++) {
		d[i].list = list;
		d[i].n = m;
		// d[i].thread_id = i;

		if ( 0 != pthread_create(&thread[i], NULL, sieve_primes, (void*)&d[i])) {
			perror("Cannot create thread");
			exit(EXIT_FAILURE);
		}
	}
	/* Join threads */
	for (i=0; i<procs; i++) pthread_join(thread[i], NULL);

	/* Find twins (could be threaded, but runs fast enough) */
	i = find_twins(list, m, quiet);
	if (verbose == 1) printf("Twin primes under [%u] : %d\n",m,i);

	free(thread);
	free(d);
	return 0;
}
