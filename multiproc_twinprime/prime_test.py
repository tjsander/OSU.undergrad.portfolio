# Travis J. Sanders
# sandetra@onid.oregonstate.edu
# CS344-400
# Homework #5

#
#	UINT MAX VALIDATION HAS BEEN COMMENTED OUT
#	I assumed you wouldn't want me to submit my 275mb file of
#	validated twin prime pairs.
#

import os
import sys
import getopt
import time

def validate_primes(command, clean):
	print "Validating [" + command + "] against known twins primes..."
	os.system("rm -f test_100k.txt")
	os.system(command + " -c 5 -m 100000 > test_100k.txt")
	os.system("diff test_100k.txt valid_primes_100k.txt")
	os.system("rm -f test_10m.txt")
	os.system(command + " -c 5 -m 10000000 > test_10m.txt")
	os.system("diff test_10m.txt valid_primes_10m.txt")
	# print "Validating to UINT_MAX... please wait!"
	# os.system("rm -f test_max.txt")
	# os.system(command + " -c 5 > test_max.txt")
	# os.system("diff test_max.txt valid_primes_uint_max.txt")
	print "Finished!"
	if (clean == 1):
		os.system("rm -f test_100k.txt test_10m.txt")
		# os.system("rm -f test_max.txt")

def collect_times(command, top):
	print "program,m,c,time"
	m = 500000000
	while m <= 4000000000:
		for n in range (1, top+1):
			timer = time.time()
			os.system(command + " -q -c " + str(n) + " -m " + str(m))
			timer = time.time() - timer
			print command + "," + str(m) + "," + str(n) + "," + str(timer)
		m += 500000000
	for n in range (1, top+1):
		timer = time.time()
		os.system(command + " -q -c " + str(n))
		timer = time.time() - timer
		print command + ",4294967295," + str(n) + "," + str(timer)
	
def main():
	os.system("make")
	## Change the second value to 0 to keep test files.
	validate_primes("prime_pthread", 1)
	validate_primes("prime_mproc", 1)
	# collect_times("prime_mproc", 10)
	# collect_times("prime_pthread", 10)
	collect_times("prime_mproc", 30)
	collect_times("prime_pthread", 30)

if __name__ == '__main__':
	main()
