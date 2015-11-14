/* Filename: scanner.c
/* PURPOSE:
*    SCANNER.C: Functions implementing a Lexical Analyzer (Scanner)
*    as required for CST8152, Assignment #2
*    scanner_init() must be called before using the scanner.
*    The file is incomplete;
*    Provided by: Svillen Ranev
*    Version: 1.15.02
*    Date: 29 September 2015
*******************************************************************
*    REPLACE THIS HEADER WITH YOUR HEADER
*******************************************************************
*/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in Borland compiler projects. */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/* #define NDEBUG */ /* to suppress assert() call */
#include <assert.h>	 /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG


/*******************************************************************
*    VARIABLES
*******************************************************************/

/* Global objects - variables */
extern Buffer * str_LTBL;						/* String literal table */
int line;										/* current line number of the source code */
extern int scerrnum;							/* run-time error number */

/* Local(file) global objects - variables */
static Buffer *lex_buf;							/* pointer to temporary lexeme buffer CREATE FIXED BUFFER, then you don't have to check for null when addc */


/*******************************************************************
*    FUNCTION PROTOTYPES
*******************************************************************/

/* scanner.c static(local) functions */
static int char_class(char c);					/* character class function */
static int get_next_state(int, char, int *);	/* state machine function */
static int iskeyword(char * kw_lexeme);			/* keywords lookup functuion */
static long atool(char * lexeme);				/* converts octal string to decimal value */


/*******************************************************************
*    FUNCTION DEFINITIONS
*******************************************************************/

int scanner_init(Buffer * sc_buf) {
	if (b_isempty(sc_buf))
		return EXIT_FAILURE;

	/* in case the buffer has been read previously */
	b_setmark(sc_buf, 0);
	b_retract_to_mark(sc_buf);
	b_reset(str_LTBL);
	line = 1;
	return EXIT_SUCCESS;
}

Token mlwpar_next_token(Buffer * sc_buf) {
	Token t;				/* token to return after recognition */
	unsigned char c;		/* input symbol */
	int state = 0;			/* initial state of the FSM */
	short lexstart;			/* start offset of a lexeme in the input buffer */
	short lexend;			/* end offset of a lexeme in the input buffer */
	int accept = NOAS;		/* type of state - initially not accepting */

	/* The following variables are used when testing for string literals, comments */
	short temp_offset;		/* storage for offset to beginning of string */
	char temp_char;			/* storage for char found in string or comment */
	short str_len;			/* length of the string */
	short MAX_CHARS = 17;	/* maximum chars that can be stored in error attribute */
	int i;					/* loop counter */

	while (1) { /* Endless loop broken by token returns. Will generate warning */

		c = b_getc(sc_buf);	/* Get the next symbol from the input buffer */

		/* Test for end of source file */
		if (c == '\0') {
			t.code = SEOF_T;
			return t;
		}

		/* Test for empty space */					/* <---- Test for all white space characters */
		else if (c == ' ')
			continue;

		/* Test for new line */						/* <---- Test for all new line characters */
		else if (c == '\n') {
			line++;
			continue;
		}

		/* Test for left brace */
		else if (c == '{') {
			t.code = LBR_T;
			return t;
		}

		/* Test for right brace */
		else if (c == '}') {
			t.code = RBR_T;
			return t;
		}

		/* Test for left parenthesis */
		else if (c == '(') {
			t.code = LPR_T;
			return t;
		}

		/* Test for right parenthesis */
		else if (c == ')') {
			t.code = RPR_T;
			return t;
		}

		/* Test for comma */
		else if (c == ',') {
			t.code = COM_T;
			return t;
		}

		/* Test for semi-colon */
		else if (c == ';') {
			t.code = EOS_T;
			return t;
		}

		/* Test for string concatenation operator*/
		else if (c == '#') {
			t.code = SCC_OP_T;
			return t;
		}

		/* Test for assignment (=) or equals (==) */
		else if (c == '=') {
			if (b_getc(sc_buf) == '=') {	/* equals */
				t.code = REL_OP_T;
				t.attribute.rel_op = EQ;
				return t;
			}
			b_retract(sc_buf);	/* assignment */
			t.code = ASS_OP_T;
			return t;
		}

		/* Test for less than (<) or not equal (<>) */
		else if (c == '<') {
			if (b_getc(sc_buf) == '>') {	/* not equal */
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
				return t;
			}
			b_retract(sc_buf);	/* less than */
			t.code = REL_OP_T;
			t.attribute.rel_op = LT;
			return t;
		}

		/* Test for greater than */
		else if (c == '>') {
			t.code = REL_OP_T;
			t.attribute.rel_op = GT;
			return t;
		}

		/* Test for addition operator */
		else if (c == '+') {
			t.code = ART_OP_T;
			t.attribute.arr_op = PLUS;
			return t;
		}

		/* Test for subtraction operator */
		else if (c == '-') {
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS;
			return t;
		}

		/* Test for multiplication operator */
		else if (c == '*') {
			t.code = ART_OP_T;
			t.attribute.arr_op = MULT;
			return t;
		}

		/* Test for division operator */
		else if (c == '/') {
			t.code = ART_OP_T;
			t.attribute.arr_op = DIV;
			return t;
		}

		/* Test for logical operators (.AND., .OR) */
		else if (c == '.') {

			/* Set mark at next symbol, in case of error token */
			b_setmark(sc_buf, b_getc_offset(sc_buf));

			/* Test for logical OR */
			if (b_getc(sc_buf) == 'O') {
				if (b_getc(sc_buf) == 'R') {
					if (b_getc(sc_buf) == '.') {
						t.code = LOG_OP_T;
						t.attribute.log_op = OR;
						return t;
					}
				}
			}

			/* Retract and test for logical AND */
			b_retract_to_mark(sc_buf);
			if (b_getc(sc_buf) == 'A') {
				if (b_getc(sc_buf) == 'N') {
					if (b_getc(sc_buf) == 'D') {
						if (b_getc(sc_buf) == '.') {
							t.code = LOG_OP_T;
							t.attribute.log_op = AND;
							return t;
						}
					}
				}
			}

			/* Retract and set error token */
			b_retract_to_mark(sc_buf);
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[1] = '\0';
			return t;
		}

		/* Test for comment */
		else if (c == '!') {
			temp_char = b_getc(sc_buf);

			/* Check next symbol, must be '<' for legal comment */
			if (temp_char == '<') {

				/* Skip all symbols until line terminator */
				temp_char = b_getc(sc_buf);
				while (temp_char != '\n') {									/** <---- test for all newline characters /

					/* Error if comment does not end in line terminator */														
					if ((temp_char == '\0') | (temp_char == EOF)) {
						t.code = ERR_T;
						t.attribute.err_lex[0] = '!';
						t.attribute.err_lex[1] = '<';
						t.attribute.err_lex[2] = '\0';
						b_retract(sc_buf);	/* So that next read will recognize SEOF */
						return t;
					}

					/* Keep checking for line terminator */
					temp_char = b_getc(sc_buf);
				}

				/* Process line terminator and continue */
				line++;
				continue;
			}

			/* Process illegal comment token (missing '<') */
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[1] = temp_char;
			t.attribute.err_lex[2] = '\0';

			/* Skip all symbols until line terminator or SEOF */
			temp_char = b_getc(sc_buf);
			while (temp_char != '\n') {

				/* If SEOF, retract so it will be recognized next read */
				if ((temp_char == '\0') | (temp_char == EOF)) {
					b_retract(sc_buf);
					return t;
				}

				/* Keep checking for line terminator */
				temp_char = b_getc(sc_buf);
			}

			/* Process line terminator and return error token */
			line++;
			return t;
		}

		/* Test for string literal */					/* <----- count line numbers */
		else if (c == '"') {

			/* Set mark to beginning of string */
			b_setmark(sc_buf, b_getc_offset(sc_buf) - 1);

			/* Check for legal string -- delegate to separate function??? */
			temp_char = b_getc(sc_buf);
			while (temp_char != '"') {

				/* If SEOF found before closing ", illegal string */
				if ((temp_char == '\0') | (temp_char == EOF)) {
					t.code = ERR_T;
					str_len = (b_getc_offset(sc_buf) - 1) - b_mark(sc_buf);
					if (str_len > MAX_CHARS) str_len = MAX_CHARS;
					b_retract_to_mark(sc_buf);

					/* Add string to error token attribute */
					for (i = 0; i < str_len; i++)
						t.attribute.err_lex[i] = b_getc(sc_buf);
					t.attribute.err_lex[i++] = '.';
					t.attribute.err_lex[i++] = '.';
					t.attribute.err_lex[i++] = '.';
					t.attribute.err_lex[i++] = '\0';

					/* Return error token */
					return t;
				}

				/* Keep looking for closing " or SEOF */
				temp_char = b_getc(sc_buf);
			}

			/* Closing " found, process legal string literal */
			/* Calculate length of string literal */
			str_len = (b_getc_offset(sc_buf) - 1) - b_mark(sc_buf);

			/* Mark end of string literal, before retracting to start */
			lexend = b_getc_offset(sc_buf);
			b_retract_to_mark(sc_buf);

			/* Mark offset in string literal table for token attribute */
			temp_offset = b_size(str_LTBL);

			/* Add string to string literal table, excluding "" */
			for (i = 0; i <= MAX_CHARS && i <= str_len; i++) {
				temp_char = b_getc(sc_buf);
				if (temp_char != '"')
					b_addc(str_LTBL, temp_char);
			}
			b_addc(str_LTBL, '\0');

			/* if string was longer than size allowed in string literal
			table, set input buffer offset to end of string */
			if (str_len > MAX_CHARS) {
				b_setmark(sc_buf, lexend);
				b_retract_to_mark(sc_buf);
			}

			/* Return string literal token */
			t.code = STR_T;
			t.attribute.str_offset = temp_offset;
			return t;
		}

		/* Process state transition table */						/* count line numbers??? */
		else if (isalpha(c) | isdigit(c)) {
			b_setmark(sc_buf, b_getc_offset(sc_buf) - 1);
			state = 0;

			do {
				state = get_next_state(state, c, &accept);
				c = b_getc(sc_buf);
			} while (accept == NOAS);

			/* Token found - process final state */
			if (accept == ASWR) 
				b_retract(sc_buf);

			/* Get beginning and end of the lexeme */
			lexstart = b_mark(sc_buf);
			lexend = b_getc_offset(sc_buf) - 1;

			/* Create temporary lexeme buffer and store lexeme */
			lex_buf = b_create(100, 0, 'f');						/* ERROR IF LEX_BUF == NULL */
			str_len = lexend - lexstart;
			b_retract_to_mark(sc_buf);
			for (i = 0; i < str_len; i++)
				b_addc(lex_buf, b_getc(sc_buf));					/* CHECK NULL IF LEX_BUF NOT IN FIXED MODE */
			b_addc(lex_buf, '\0');

			/* Call accepting function */
			t = aa_table[state](lex_buf->cb_head);					/* HOW TO NOT REFER to cb_head? */
			b_destroy(lex_buf);
			return t;
		}

		else {
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[1] = '\0';
			return t;
		}

	}//end while(1)
}

int get_next_state(int state, char c, int *accept) {
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];

#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif

	assert(next != IS);

#ifdef DEBUG
	if (next == IS){
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif

	*accept = as_table[next];
	return next;
}

int char_class(char c) {
	int val;

	if (isalpha(c)) val = 0;	/* letter */
	else if (c == 0) val = 1;	/* 0 */
	else if (isdigit(c)) {
		if ((c >= 1) & (c <= 7))
			val = 2;			/* 1-7 */
		else val = 3;			/* 8-9 */
	}
	else if (c == '.') val = 4;	/* . */
	else if (c == '%') val = 5;	/* % */
	else val = 6;				/* other */

	return val;
}

/* Accepting functions */
Token aa_func02(char *lexeme) {
	Token t;
	unsigned int i;

	/* Create token for AVID */
	t.code = AVID_T;
	for (i = 0; (i < strlen(lexeme)) && (i < VID_LEN); i++)
		t.attribute.vid_lex[i] = lexeme[i];

	/* Make C-type string */
	t.attribute.vid_lex[i] = '\0';
	return t;
}

Token aa_func03(char *lexeme) {
	Token t;
	unsigned int i;

	/* Create token for SVID */
	t.code = SVID_T;
	for (i = 0; (i < strlen(lexeme)) && (i < VID_LEN); i++)
		t.attribute.vid_lex[i] = lexeme[i];
	
	/* If lexeme is longer than VID_LEN, overwrite 
	 the last char stored in attribute to '%' */
	if (strlen(lexeme) > VID_LEN)
		t.attribute.vid_lex[i-1] = '%';

	/* Make C-type string */
	t.attribute.vid_lex[i] = '\0';	
	return t;
}

Token aa_func05(char *lexeme) {
	Token t = { 0 };
	return t;
}

Token aa_func08(char *lexeme) {
	Token t = { 0 };
	return t;
}

Token aa_func10(char *lexeme) {
	Token t = { 0 };
	return t;
}

Token aa_func11(char *lexeme) {
	Token t = { 0 };
	return t;
}

Token aa_func12(char *lexeme) {
	Token t = { 0 };
	return t;
}

Token aa_func13(char *lexeme) {
	Token t = { 0 };
	return t;
}

/* HERE YOU WRITE YOUR ADDITIONAL FUNCTIONS (IF ANY). FOR EXAMPLE */
int iskeyword(char * kw_lexeme){}