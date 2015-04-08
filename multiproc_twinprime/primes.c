/*
  Travis J. Sanders
  sandetra@onid.oregonstate.edu
  CS344-400
  Homework #5
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <getopt.h>
// #define UINT_MAX   4294967295U
// #define UINT_MAX_SQRT 65536

// http://www.tek-tips.com/faqs.cfm?fid=3999
#define ISBITSET(x,i) ((x[i>>3] & (1<<(i&7)))!=0)
#define SETBIT(x,i) x[i>>3]|=(1<<(i&7));
#define CLEARBIT(x,i) x[i>>3]&=(1<<(i&7))^0xFF;

unsigned int no_twin_primes(unsigned int n, int verbose);

unsigned int no_twin_primes(unsigned int n, int verbose)
{
	unsigned int k, t, j;
	long m, i;
	// int prime;
	int * list;
	list = calloc(n, 1);	// http://www.cplusplus.com/reference/cstdlib/calloc/
	for (i=0; i<n; i+=2) SETBIT(list, i);
	// prime = 0;
	t = 0;

	for (k=2; k < sqrt(n);) {
		m = k+1;
		while (m <= n && ISBITSET(list, m)) m++;
		// prime++;
		i = 3;
		while (m * i < n) {
			SETBIT(list, m*i);
			i+=2;
		}
		k = m;
		// printf("K AND M = %d\n", k);
	}
	for (j=3; (j+2 < n); j+=2) {
		if (!ISBITSET(list, j)) {
			if (!ISBITSET(list, (j+2))) {
				if (verbose == 1) printf("%u %u\n", j, j+2);
				t++;			//count twins
			}
		}
	}
	free(list);
	return t;		//return number of twins
}
 
int main(int argc, char *argv[]) 
{
	unsigned int i, c;
	int quiet;
	int para;
	unsigned int m;

	/* DEFAULT VALUES */
	m = UINT_MAX;
	quiet = 1;
	para = 1;

	while ((c = getopt (argc, argv, "qm:c:")) != -1) {
		switch(c) {
		case 'q':
			quiet = 0;
			// printf("Quiet!\n");
			break;
		case 'm':
			m = atoi(optarg);
			// printf("m = %u\n", m);
			break;
		case 'c':
			para = atoi(optarg);
			// printf("para = %d\n", para);
			break;
		default:
			break;
		}
	}
	
	printf("Finding twin primes under m=%u\n", m);
	printf("The square root of m is =%f\n", sqrt(m));
	i = no_twin_primes(m, quiet);
	printf("FINISHED! m=%u\nThe number of twin primes under m is=%u\n", m, i);

	return 0;
}
