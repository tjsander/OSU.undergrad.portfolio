TITLE Sorting Arrays    (sandetra_6A.asm)

; Author: Travis Sanders (sandetra@onid.oregonstate.edu)
; Course / Project ID : CS271-400 / Homework 6A
; Date: 06/07/2014
; Description: Implements and tests ReadVal and WriteVal procedures
;	for unsigned integers using getString and displayString macros.
;
; EXTRA CREDIT: Numbers each line of user input / displays running subtotal of user’s numbers (+25)
;		ReadVal and WriteVal procedures are recursive (+50)

INCLUDE Irvine32.inc

NO_LOOPS = 15	;numer of inputs to loop
STRSIZE = 20	;max size of input strings

;-----------------------------------------------------
getString MACRO buffer, prompt_string
;
; Displays a variable, using its known attributes.
; Receives: 1: A string to store the string in.
;	2: Address of a prompt
;-----------------------------------------------------
	push	edx
	push	ecx
	displayString OFFSET prompt_string
	mov		edx, buffer
	mov		ecx, STRSIZE - 1
	call	ReadString
	pop		ecx
	pop		edx
ENDM

;-----------------------------------------------------
displayString MACRO buffer
;
; Displays a string.
; Receives: address of a string.
;-----------------------------------------------------
	push	edx
	mov		edx, buffer
	call	WriteString
	pop		edx
ENDM

.data
dwordArray	DWORD	NO_LOOPS DUP(?)
loopcount	DWORD	0				;loop counter
runningtot	DWORD	0				;running total accumulator
intro1		BYTE	"PROGRAMMING ASSIGNMENT 6: Designing low-level I/O procedures", 0
intro2		BYTE	"Written by: Travis J. Sanders", 0
intro3		BYTE	"Please provide 15 unsigned decimal integers.",0
intro4		BYTE	"After you have finished inputting the raw numbers I will display",0
intro5		BYTE	"a list of the integers, their sum, and their average value. ",0
info1		BYTE	"The largest possible unsigned 32 bit int is: ", 0
info2		BYTE	"So each int and the sum of all 15 must be less than that.", 0
prompt1		BYTE	"Please enter an unsigned int: ", 0
prompt2		BYTE	"Decimal value accepted as: ", 0
prompt3		BYTE	"String value read as: ", 0
valuest		BYTE	" values entered. Running total: ", 0
error1		BYTE	"< ERROR! : Invalid character >", 0
error2		BYTE	"< ERROR! : INT32 OVERFLOW! PLS USE SMALLER NUMBERS >", 0
error3		BYTE	"< ERROR! : TOTAL OVERFLOWED. PLS USE SMALLER NUMBERS >", 0
results1	BYTE	"You entered the following numbers: ", 0
results2	BYTE	"The sum of these numbers is: ", 0
results3	BYTE	"The average is: ", 0
goodBye		BYTE	"Good-bye! Thanks for a very substantial class!", 0
spacer		BYTE	", ",0

.code
;-----------------------------------------------------
main PROC
;-----------------------------------------------------
	displayString OFFSET intro1
	call	crlf
	displayString OFFSET intro2
	call	crlf
	call	crlf
	displayString OFFSET intro3
	call	crlf
	displayString OFFSET intro4
	call	crlf
	displayString OFFSET intro5
	call	crlf
	call	crlf
	displayString OFFSET info1
	push	0FFFFFFFFh				;largest possible 32bit int (also for testing)
	call	WriteVal
	call	crlf
	displayString OFFSET info2
	call	crlf
	call	crlf
	mov		ecx, NO_LOOPS			;global variable
	mov		edi, OFFSET dwordArray	;array[0]
mainloop:
	mov		eax, loopcount
	call	WriteDec
	displayString OFFSET valuest
	mov		eax, runningtot
	call	WriteDec
	call	crlf
	push	edi						;push the array
	call	ReadVal
	call	crlf
	inc		loopcount
	mov		eax, [edi]
	add		edi, 4					;incriment array
	mov		ebx, runningtot
	add		eax, ebx
	jc		overflow
	mov		runningtot, eax
	loop	mainloop
	jmp		skipend
overflow:
	displayString OFFSET error3
	call	crlf
skipend:
	push	NO_LOOPS				;size of array
	push	OFFSET dwordArray
	call	Results
	call	crlf
	displayString OFFSET goodBye
	call	crlf
	exit	; exit to operating system
main ENDP

;-----------------------------------------------------
ReadVal PROC
; Converts a string of user input to an integer.
; Receives: address of a dword integer
; Returns: user-specified integer in address
;-----------------------------------------------------
	push	ebp
	mov		ebp, esp
	jmp		skip1
errormess:
	displayString OFFSET error1
	call	CrLf
	jmp		skip1
errormess2:
	displayString OFFSET error2
	pop		eax
	call	CrLf
skip1:
	mov		esi, [ebp+8]
	getString esi, prompt1
	mov		eax, 0
	push	eax
reads:
	mov		eax, 0
	LODSB
	cmp		al, 0
	je		endread		;NULL terminate
	pop		ebx			;restore calculated value
	push	eax			;save ascii char
	mov		eax, ebx
	mov		ebx, 10
	mul		ebx			;multiply result by 10
	jc		errormess2
	mov		edx, eax	;store result in edx
	pop		eax			;reload ascii char
	cmp		al, 48		;cmp to ascii 0
	jl		errormess
	cmp		al, 57		;cmp to ascii 9
	jg		errormess
	mov		ah, 48
	sub		al, ah		;convert ascii to dec
	mov		ah, 0
	add		eax, edx
	jc		errormess2
	push	eax			;save calculated value
	jmp		reads
endread:
	pop		eax
	mov		esi, [ebp+8]
	mov		[esi], eax
	;displayString OFFSET prompt2	;debug
	;call	WriteDec
	;call	CrLf
	pop		ebp
	ret 4
ReadVal ENDP

;-----------------------------------------------------
WriteVal PROC
; Converts a numeric value to a string of digits and displays output.
; Receives: address of a dword unsigned integer
; Returns: console output
;-----------------------------------------------------
	push	ebp
	mov		ebp, esp
	pushad
	sub		esp, 2			;make space for the character string
;-----header------
	mov		eax, [ebp+8]
	lea		edi, [ebp-2]	;LEA to access the local address
	mov		ebx, 10
	mov		edx, 0
	div		ebx				;divide input by 10
	cmp		eax, 0
	jle		endwrite		;end recursion when eax = 0
	push	eax
	call	WriteVal
endwrite:
	mov		eax, edx
	add		eax, 48			;convert to ascii
	stosb					;store in edi
	mov		eax, 0			;null terminator
	stosb
	sub		edi, 2			;reset edi
	displayString edi
;-----footer------
	add		esp, 2
	popad
	pop		ebp
	ret		4
WriteVal ENDP

;-----------------------------------------------------
Results PROC
; Calculates the sum and mean of ints in an array.
; Receives: address of an array, size of the array
; Returns: console output of the sum and mean
;-----------------------------------------------------
	push	ebp
	mov		ebp, esp
	mov		esi, [ebp+8]
	mov		ecx, [ebp+12]
	sub		esp, 4
	mov		edx, 0			;use eax to calculate the sum
	displayString OFFSET results1
	call	crlf
	jmp		s1
resloop:
	displayString OFFSET spacer
s1:
	push	[esi]
	call	WriteVal
	mov		ebx, [esi]
	add		edx, ebx
	add		esi, 4
	loop	resloop
	call	crlf
	displayString OFFSET results2
	push	edx				;display the sum
	call	WriteVal
	call	crlf
	displayString OFFSET results3
	mov		eax, edx
	mov		edx, 0
	mov		ebx, [ebp+12]
	div		ebx
	push	eax
	call	WriteVal
	call	crlf
	add		esp, 4
	pop		ebp
	ret		8
Results ENDP

END main
