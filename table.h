/*******************************************************************************
* File Name:		table.h
* Compiler:			MS Visual Studio 2013
* Author:			S^R with additions by Skye Turriff
* Course:			CST 8152 - Compilers, Lab Section:	011
* Assignment:		1
* Date:				5 October 2015
* Professor:		Sv. Ranev
* Purpose:			Transition Table and function declarations necessary 
*					for the scanner implementation as required for CST8152 - 
*					Assignment #2.
* Function list:	aa_func02(char*), aa_func03(char*), aa_func05(char*),
*					aa_func08(char*), aa_func10(char*), aa_func11(char*),
*					aa_func12(char*), aa_func13(char*)
*******************************************************************************/

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

#define ES 12				/* Error state */
#define IS -1				/* Inavalid state */

#define PLATY_MAX 32767		/* Integer literals must <= 2 bytes in memory */

/* State transition table definition */
#define TABLE_COLUMNS 7	

/* Transition Table - type of states defined in separate table */
int  st_table[][TABLE_COLUMNS] = {
			/*|  A  |  0  | 1-7 | 8-9 |  .  |  %  | other | */
/* State 0 */ {  1,    6,    4,    4,   IS,    IS,    IS  },
/* State 1 */ {  1,    1,    1,    1,    2,     3,     2  },
/* State 2 */ { IS,   IS,   IS,   IS,   IS,    IS,    IS  },
/* State 3 */ { IS,   IS,   IS,   IS,   IS,    IS,    IS  },
/* State 4 */ { ES,    4,    4,    4,    7,     5,     5  },
/* State 5 */ { IS,   IS,   IS,   IS,   IS,    IS,    IS  },
/* State 6 */ { ES,   ES,    9,   ES,    7,    ES,     5  },	
/* State 7 */ {  8,    7,    7,    7,    8,     8,     8  },
/* State 8 */ { IS,   IS,   IS,   IS,   IS,    IS,    IS  },
/* State 9 */ { ES,    9,    9,   ES,   ES,    ES,    10  },
/* State 10 */{ IS,   IS,   IS,   IS,   IS,    IS,    IS  },
/* State 11 */{ IS,   IS,   IS,   IS,   IS,    IS,    IS  },
/* State 12 */{ IS,   IS,   IS,   IS,   IS,    IS,    IS  },
/* State 13 */{ IS,   IS,   IS,   IS,   IS,    IS,    IS  }

};

/* Accepting state table definition */
#define ASWR -1		/* Accepting state with retract */
#define ASNR 1		/* Accepting state with no retract */
#define NOAS 0		/* Not accepting state */

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
