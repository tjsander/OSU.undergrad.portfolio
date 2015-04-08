TITLE Prime Numbers     (sandetra4.asm)

; Author: Travis Sanders (sandetra@onid.oregonstate.edu)
; Course / Project ID : CS271-400 / Homework 4
; Date: 05/11/2014
; Description: Calculates and prints a user-specified number of prime numbers.

INCLUDE Irvine32.inc

UPPER_LIMIT = 300
LOWER_LIMIT = 1

.data

intro_1		BYTE	"Prime Numbers		Programmed by Travis J. Sanders", 0
intro_2		BYTE	"Enter the number of prime numbers you would like to see.", 0
intro_3		BYTE	"I will accept orders for up to 300 primes.", 0
prompt_1	BYTE	"Enter the number of primes to display [1 - 300]: ", 0
error_1		BYTE	"Out of range. Try again.", 0
goodbye		BYTE	"Results certified by Eratosthenes. Goodbye.", 0
spaces		BYTE	"   ", 0
space		BYTE	" ",0
twin		BYTE	"*",0
input_1		DWORD	?
counter		DWORD	0
primer		DWORD	1	;prime number counter
lastPrime	DWORD	-1	;previously found prime no
primeBool	BYTE	0	;boolean value for isPrime to communicate with nextPrime

.code
main PROC
	call	introduction
	call	getUserData
	call	showPrimes
	call	farewell
	exit	; exit to operating system
main ENDP

; Procedure to introduce program, programmer and
;	print instructions.
; registers changed: edx
introduction PROC
	mov		edx, OFFSET intro_1
	call	WriteString
	call	CrLf
	call	CrLf
	mov		edx, OFFSET intro_2
	call	WriteString
	call	CrLf
	mov		edx, OFFSET intro_3
	call	WriteString
	call	CrLf
	call	CrLf
	ret
introduction ENDP

; Prompt user to input a number of primes to display
;	and return a validated user input.
; receives: User input
; returns: User input in input_1
; preconditions: none
; registers changed: eax, edx
getUserData PROC
	mov		edx, OFFSET prompt_1
	call	WriteString
	call	ReadInt
	mov		input_1, eax
	call	validateInput
	ret
getUserData ENDP

;
; Validates user input to be within specified limits
;	UPPER_LIMIT and LOWER_LIMIT
; receives: input_1
; returns: Validated user input in input_1
; preconditions: input_1 must contain an integer
; registers changed: eax, edx
validateInput PROC
	jmp		skip1					;skip on first run
bound_out:
	mov		edx, OFFSET	error_1		;print error message
	call	WriteString
	call	CrLf
	call	getUserData				;recursive call to getUserData
skip1:
	mov		eax, input_1
	cmp		eax, UPPER_LIMIT
	jnle	bound_out
	cmp		eax, LOWER_LIMIT
	jl		bound_out
	ret
validateInput ENDP

; Procedure loop to print 'input_1' prime numbers.
; receives: input_1
; returns: Outputs input_1 prime numbers.
; preconditions: 300 >= input_1 > 0
; registers changed: eax, ebx, ecx, edx
showPrimes PROC
	call	CrLf
	mov		ecx, input_1
ploop:
	call	nextPrime
	mov		eax, primer
	call	WriteDec
	mov		ebx, lastPrime
	sub		eax, ebx
	cmp		eax, 2
	jne		not_twin			;skip if the prime difference is not 2
	mov		edx, OFFSET twin
	call	WriteString
not_twin:
	mov		edx, OFFSET spaces
	call	WriteString
	mov		eax, primer
	mov		lastPrime, eax		;save the prime for comparison
	inc		counter				;incriment the output counter
	cmp		counter, 10
	jne		skip_return
	call	CrLf
	mov		counter, 0
skip_return:
	loop	ploop				;counted loop
	call	CrLf
	call	CrLf
	ret
showPrimes ENDP

; Incriments the primer counter until isPrime returns true
; receives: primer counter
; returns: The next prime number in 'primer'
; preconditions: primer > 0
; registers changed: eax, ebx, edx (in subprocess)
nextPrime PROC
again:
	inc		primer
	call	isPrime
	cmp		primebool, 1
	jne		again
	ret
nextPrime	ENDP

; Boolean to determine if primer is a prime number
; receives: Candidate prime number in 'primer'
; returns: sets primeBool to 0 if value is not prime
;	sets primeBool to 1 if value is prime
; preconditions: primer > 1
; registers changed: eax, ebx, edx
isPrime	PROC
	mov		primeBool, 0		;default to false
	mov		eax, primer
	cmp		eax, 2
	je		primefound			;if n = 2, n is prime

	mov		ebx, 2
	mov		edx, 0
	div		ebx
	cmp		edx, 0
	je		notprime			;if n mod (m = n/2) = 0, n is not prime
	mov		ebx, eax
divisors:
	cmp		ebx, 2
	jle		primefound			;if m <= 2, number is prime (n/2 checked in base case)
	mov		edx, 0
	mov		eax, primer
	div		ebx
	cmp		edx, 0
	je		notprime			;if n mod (m>2) = 0, n is not prime
	dec		ebx					;decrement m
	jg		divisors			;check m
primefound:
	mov		primeBool, 1
notprime:
	ret
isPrime		ENDP

; Procedure to say goodbye
; registers changed: edx
farewell PROC
	mov		edx, OFFSET goodbye
	call	WriteString
	call	CrLf
	ret
farewell ENDP

END main
