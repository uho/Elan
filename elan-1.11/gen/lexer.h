/*
   File: lexer.h
   Defines the lexical analyzer for elan

   CVS ID: "$Id: lexer.h,v 1.5 2007/01/25 23:58:18 marcs Exp $"
*/
#ifndef IncLexer
#define IncLexer

/* libdcg includes */
#include <dcg.h>

/* local includes */
#include "elan_ast.h"

/* Define the tokens, to begin with undefined */
typedef enum { UNDEFINED,				/* 0 */

/* General categories */
SMALL_NAME, CAPITAL_NAME,				/* 1,2 */
INTEGER_DENOTER, REAL_DENOTER, TEXT_DENOTER,		

/* Reserved words */
USES_SYMBOL, END_SYMBOL,				/* 6,7 */
PROC_SYMBOL, ENDPROC_SYMBOL,
OP_SYMBOL, ENDOP_SYMBOL,
LET_SYMBOL, TYPE_SYMBOL,
INT_SYMBOL, REAL_SYMBOL,
BOOL_SYMBOL, TEXT_SYMBOL,
ROW_SYMBOL, ARRAY_SYMBOL, STRUCT_SYMBOL,
CONST_SYMBOL, VAR_SYMBOL,
IF_SYMBOL, THEN_SYMBOL, ELIF_SYMBOL,
ELSE_SYMBOL, ENDIF_SYMBOL,
FOR_SYMBOL, FROM_SYMBOL,
UPTO_SYMBOL, DOWNTO_SYMBOL,
WHILE_SYMBOL, UNTIL_SYMBOL,
REP_SYMBOL, ENDREP_SYMBOL,
SELECT_SYMBOL, OF_SYMBOL, CASE_SYMBOL,
OTHERWISE_SYMBOL, ENDSELECT_SYMBOL,
CONCR_SYMBOL, LEAVE_SYMBOL, WITH_SYMBOL,
DIV_SYMBOL, MOD_SYMBOL,
AND_SYMBOL, OR_SYMBOL, XOR_SYMBOL, NOT_SYMBOL,
TRUE_SYMBOL, FALSE_SYMBOL, NIL_SYMBOL,

/* Reserved words for packets */
PACKET_SYMBOL, ENDPACKET_SYMBOL, DEFINES_SYMBOL,	/* 52 */
INTERNAL_SYMBOL, EXTERNAL_SYMBOL,
USES_LIBRARY_SYMBOL, USES_LIBRARY_PATH_SYMBOL,

/* Punctuation marks */
COLON_SYMBOL, INITIAL_SYMBOL, BECOMES_SYMBOL,
SEMICOLON_SYMBOL, COMMA_SYMBOL, PERIOD_SYMBOL,
OPEN_SYMBOL, CLOSE_SYMBOL,
SUB_SYMBOL, BUS_SYMBOL,
PLUS_SYMBOL, MINUS_SYMBOL, DIVIDES_SYMBOL,
ASTERIX_SYMBOL, OBELIX_SYMBOL,
EQUAL_SYMBOL, NOT_EQUAL_SYMBOL,
LESS_THAN_SYMBOL, LESS_EQUAL_SYMBOL,
GREATER_THAN_SYMBOL, GREATER_EQUAL_SYMBOL,

/* End of file */
EOF_SYMBOL } symbol;

/* exported variables */
extern int analyzing_main;
extern int this_line, this_column;
extern string this_fname;
extern symbol this_symbol;
extern string this_sval;
extern real this_rval;
extern int this_nval;
extern symbol next_symbol, next2_symbol;

/* exported actions */
void read_token ();
pragmat_list read_and_reset_pragmats ();
void init_lexer (FILE *fd, char *fname, int main_packet);

#endif /* IncLexer */
