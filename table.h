/* Filename: table.h
* Transition Table and function declarations necessary for the scanner implementation
* as required for CST8152 - Assignment #2.
* Version: 1.15.02
* Date: 29 September 2015
* Provided by: Svillen Ranev
* The file is incomplete. You are to complete it.
***************************************************
* REPLACE THIS HEADER WITH YOUR HEADER
***************************************************
*/

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

#define ES 12	/* Error state */
#define IS -1   /* Inavalid state */

/* State transition table definition */
#define TABLE_COLUMNS 7	

/*transition table - type of states defined in separate table */
int  st_table[][TABLE_COLUMNS] = {
		{ 1, 6, 4, 4, ES, ES, ES },		/* State 0 */
		{ 1, 1, 1, 1, ES, 3, 2 },		/* State 1 */
		{ IS, IS, IS, IS, IS, IS, IS },	/* State 2 */
		{ IS, IS, IS, IS, IS, IS, IS },	/* State 3 */
		{ ES, 4, 4, 4, 7, 5, 5 },		/* State 4 */
		{ IS, IS, IS, IS, IS, IS, IS },	/* State 5 */
		{ ES, ES, 9, ES, 7, ES, 5 },	/* State 6 */
		{ ES, 7, 7, 7, ES, 8, 8 },		/* State 7 */
		{ IS, IS, IS, IS, IS, IS },		/* State 8 */
		{ ES, 9, 9, ES, ES, ES, 5 },	/* State 9 */
		{ IS, IS, IS, IS, IS, IS },		/* State 10 */
		{ IS, IS, IS, IS, IS, IS },		/* State 11 */
		{ IS, IS, IS, IS, IS, IS },		/* State 12 */
		{ IS, IS, IS, IS, IS, IS }		/* State 13 */
};

/* Accepting state table definition */
#define ASWR -1		/* accepting state with retract */
#define ASNR 1		/* accepting state with no retract */
#define NOAS 0		/* not accepting state */

int as_table[] = { 
	NOAS, 
	NOAS, 
	ASWR, 
	ASNR, 
	NOAS, 
	ASWR, 
	NOAS, 
	NOAS, 
	ASWR, 
	NOAS, 
	ASWR, 
	ASWR, 
	ASNR, 
	ASWR 
};

/* Accepting action function declarations */
/* Named using the number of the accepting state */
Token aa_func02(char *lexeme);
Token aa_func03(char *lexeme);
Token aa_func05(char *lexeme);
Token aa_func08(char *lexeme);
Token aa_func10(char *lexeme);
Token aa_func11(char *lexeme);
Token aa_func12(char *lexeme);
Token aa_func13(char *lexeme);

/* Define pointer to function (of one char * argument) returning Token */
typedef Token(*PTR_AAF)(char *lexeme);

/* Accepting function callback table definition */
PTR_AAF aa_table[] = {
	NULL, 
	NULL, 
	aa_func02, 
	aa_func03, 
	NULL, 
	aa_func05, 
	NULL, 
	NULL, 
	aa_func08, 
	NULL, 
	aa_func10, 
	aa_func11, 
	aa_func12, 
	aa_func13 
};

/* Keyword lookup table */
#define KWT_SIZE  8

char * kw_table[] = {
	"ELSE",
	"IF",
	"INPUT",
	"OUTPUT",
	"PLATYPUS",
	"REPEAT",
	"THEN",
	"USING"
};

#endif
