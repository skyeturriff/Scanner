/* File name: platy_st.c
*  Purpose:This is the main program for Assignment #2 - Scanner
*  CST8152 - Compilers
*  Version: 1.15.02
*  Author: Svillen Ranev
*  Date: 29 September 2015
*/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in other compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "buffer.h"
#include "token.h"

/* constant definitions */
#define INIT_CAPACITY 200		/* initial buffer capacity */
#define INC_FACTOR 15			/* increment factor */ 
#define STR_INIT_CAPACITY 100	/* initial string literal table capacity */
#define STR_CAPACITY_INC  50	/* initial string literal table capacity inc */

/* check for ANSI C compliancy */
#define ANSI_C 0
#if defined(__STDC__)
#undef ANSI_C
#define ANSI_C 1
#endif

/* Global objects - variables */
Buffer * str_LTBL;	/* this buffer implements String Literal Table */
/* it is used as a repository for string literals */
int scerrnum;		/* run-time error number = 0 by default (ANSI) */

/* external objects */
extern int line;	/* source code line numbers - defined in scanner.c */
extern int scanner_init(Buffer * sc_buf);
extern Token mlwpar_next_token(Buffer * sc_buf);

/* function declarations */
void err_printf(char *fmt, ...);
void display(Buffer *ptrBuffer);
long get_filesize(char *fname);
void print_token(Token t);

/*  main function takes a PLATYPUS source file as
*  an argument at the command line.
*  usage: scanner source_file_name"
*/
int main(int argc, char ** argv) {

	Buffer *sc_buf;			/* pointer to input (source) buffer */
	FILE *fi;				/* input file handle */
	Token t;				/* token produced by the scanner */
	int loadsize = 0;		/* the size of the file loaded in the buffer */
	int ansi_c = !ANSI_C;	/* ANSI C compliancy flag */

	/* Check if the compiler option is set to compile ANSI C */
	/* __DATE__, __TIME__, __LINE__, __FILE__, __STDC__ are predefined preprocessor macros */
	if (ansi_c) {
		err_printf("Date: %s  Time: %s", __DATE__, __TIME__);
		err_printf("ERROR: Compiler is not ANSI C compliant!\n");
		exit(1);
	}

	/* check for correct arrguments - source file name */
	if (argc <= 1){
		err_printf("Date: %s  Time: %s", __DATE__, __TIME__);
		err_printf("Runtime error at line %d in file %s", __LINE__, __FILE__);
		err_printf("%s%s%s", argv[0], ": ", "Missing source file name.");
		err_printf("%s%s%s", "Usage: ", "scanner", "  source_file_name");
		exit(1);
	}

	/* create a source code input buffer - multiplicative mode */
	sc_buf = b_create(INIT_CAPACITY, INC_FACTOR, 'm');
	if (sc_buf == NULL) {
		err_printf("%s%s%s", argv[0], ": ", "Could not create source buffer");
		exit(1);
	}

	/* open source file */
	if ((fi = fopen(argv[1], "r")) == NULL) {
		err_printf("%s%s%s%s", argv[0], ": ", "Cannot open file: ", argv[1]);
		exit(1);
	}

	/* load source file into input buffer */
	printf("Reading file %s ....Please wait\n", argv[1]);
	loadsize = b_load(fi, sc_buf);
	if (loadsize == R_FAIL_1)
		err_printf("%s%s%s", argv[0], ": ", "Error in loading buffer.");

	/* close source file */
	fclose(fi);

	/* find the size of the file */
	if (loadsize == LOAD_FAIL) {
		printf("The input file %s %s\n", argv[1], "is not completely loaded.");
		printf("Input file size: %ld\n", get_filesize(argv[1]));
	}

	/* pack and display the source buffer */

	if (b_pack(sc_buf)) {
		display(sc_buf);
	}

	/* create string Literal Table */
	str_LTBL = b_create(INIT_CAPACITY, INC_FACTOR, 'a');
	if (str_LTBL == NULL) {
		err_printf("%s%s%s", argv[0], ": ", "Could not create string literals buffer");
		exit(1);
	}

	/* Testbed for the scanner */
	/* add SEOF to input program buffer */
	b_addc(sc_buf, '\0');

	/* Initialize scanner input buffer */
	if (scanner_init(sc_buf)) {
		;
		err_printf("%s%s%s", argv[0], ": ", "Empty program buffer - scanning canceled");
		exit(1);
	}

	printf("\nScanning source file...\n\n");
	printf("Token\t\tAttribute\n");
	printf("----------------------------------\n");
	do {
		t = mlwpar_next_token(sc_buf);
		print_token(t);
	} while (t.code != SEOF_T);

	if (b_size(str_LTBL))
		b_print(str_LTBL);

	b_destroy(sc_buf);
	b_destroy(str_LTBL);
	sc_buf = str_LTBL = NULL;

	return (0);
}

/* Error printing function with variable number of arguments */
void err_printf(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	(void)vfprintf(stderr, fmt, ap);
	va_end(ap);
	/* Move to new line */
	if (strchr(fmt, '\n') == NULL)
		fprintf(stderr, "\n");
}

/* The function displays buffer contents */
void display(Buffer *ptrBuffer) {
	printf("\nPrinting buffer parameters:\n\n");
	printf("The capacity of the buffer is:  %d\n", b_capacity(ptrBuffer));
	printf("The current size of the buffer is:  %d\n", b_size(ptrBuffer));
	printf("\nPrinting buffer contents:\n\n");
	b_print(ptrBuffer);
}

long get_filesize(char  *fname){
	FILE *input;
	long flength;
	input = fopen(fname, "r");
	if (input == NULL)
		err_printf("%s%s", "Cannot open file: ", fname);
	fseek(input, 0L, SEEK_END);
	flength = ftell(input);
	fclose(input);
	return flength;
}

/* The function prints the token returned by the scanner */
void print_token(Token t) {
	extern char * kw_table[];	/* link to keyword table in */
	switch (t.code) {
	case  ERR_T:
		printf("ERR_T\t\t%s", t.attribute.err_lex);
		if (scerrnum) {
			printf("%d", scerrnum);
			exit(scerrnum);
		}
		printf("\n");
		break;
	case  SEOF_T:
		printf("SEOF_T\n");
		break;
	case  AVID_T:
		printf("AVID_T\t\t%s\n", t.attribute.vid_lex);
		break;
	case  SVID_T:
		printf("SVID_T\t\t%s\n", t.attribute.vid_lex);
		break;
	case  FPL_T:
		printf("FPL_T\t\t%f\n", t.attribute.flt_value);
		break;
	case  INL_T:
		printf("IL_T\t\t%d\n", t.attribute.get_int);
		break;
	case  STR_T:
		printf("STR_T\t\t%d\t ", (short)t.attribute.get_int);
		printf("%s\n", b_setmark(str_LTBL, (short)t.attribute.get_int));
		break;
	case  SCC_OP_T:
		printf("SCC_OP_T\n");
		break;
	case  ASS_OP_T:
		printf("ASS_OP_T\n");
		break;
	case  ART_OP_T:
		printf("ART_OP_T\t%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T:
		printf("REL_OP_T\t%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:
		printf("LOG_OP_T\t%d\n", t.attribute.get_int);
		break;
	case  LPR_T:
		printf("LPR_T\n");
		break;
	case  RPR_T:
		printf("RPR_T\n");
		break;
	case LBR_T:
		printf("LBR_T\n");
		break;
	case RBR_T:
		printf("RBR_T\n");
		break;
	case KW_T:
		printf("KW_T\t\t%s\n", kw_table[t.attribute.get_int]);
		break;
	case COM_T:
		printf("COM_T\n");
		break;
	case EOS_T:
		printf("EOS_T\n");
		break;
	default:
		printf("Scanner error: invalid token code: %d\n", t.code);
	}
}

