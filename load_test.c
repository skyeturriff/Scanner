#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "buffer.h"
#include "token.h"

#define INIT_CAPACITY 200 
#define INC_FACTOR 15     
#define STR_INIT_CAPACITY 100 
#define STR_CAPACITY_INC  50    

Buffer * str_LTBL;
int scerrnum; 
extern int line;  
extern int scanner_init(Buffer * sc_buf);
extern Token mlwpar_next_token(Buffer * sc_buf);

void display(Buffer *ptrBuffer);
void print_token(Token t);


 int main() {   

    Buffer *sc_buf;
    FILE *fp;
    Token t;
    int loadsize = 0; 

    sc_buf = b_create(INIT_CAPACITY, INC_FACTOR, 'm');

    fp = fopen("SVID.txt", "r");
    if (fp == NULL)
    {
        printf("Error in opening file");
        return(-1);
    }

    /* load a source file into the input buffer  */
    printf("Reading file....Please wait\n");
    loadsize = b_load(fp, sc_buf);
    printf("number of symbols loaded: %d\n", loadsize);

    fclose(fp);

    if (b_pack(sc_buf)) {
        display(sc_buf);
    }

    /* create string Literal Table */
    str_LTBL = b_create(INIT_CAPACITY, INC_FACTOR, 'a');

    /* Testbed for the scanner */
    /* add SEOF to input program buffer */
    b_addc(sc_buf, '\0');

    /* Initialize scanner input buffer */
	scanner_init(sc_buf);

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

/* The function displays buffer contents */
void display(Buffer *ptrBuffer) {
    printf("\nPrinting buffer parameters:\n\n");
    printf("The capacity of the buffer is:  %d\n", b_capacity(ptrBuffer));
    printf("The current size of the buffer is:  %d\n", b_size(ptrBuffer));
    printf("\nPrinting buffer contents:\n\n");
    b_print(ptrBuffer);
}

/* The function prints the token returned by the scanner */
void print_token(Token t) {
    extern char * kw_table[];   /* link to keyword table in */
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