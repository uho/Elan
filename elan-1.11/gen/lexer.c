/*
   File: lexer.c
   Defines a lexer for ELAN
   To add: checks op buffer overflows....

   CVS ID: "$Id: lexer.c,v 1.8 2011/08/13 15:58:54 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>
#include <dcg_lexutil.h>

/* local includes */
#include "elan_ast.h"
#include "options.h"
#include "lexer.h"

/* exported variables */
int analyzing_main;
string this_fname;
int this_line, this_column;
symbol this_symbol;
string this_sval;
real this_rval;
int this_nval;
static int next_line, next_column;
symbol next_symbol;
static string next_sval;
static int next_nval;
static real next_rval;
static int next2_line, next2_column;
symbol next2_symbol;
static string next2_sval;
static int next2_nval;
static real next2_rval;
static pragmat_list curr_pragmats;

/* error administration */
static void lexical_error (int lin, int col, char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  dcg_error (0, "lexical error in file %s, line %d, col %d: %s", this_fname, lin, col, buf);
}

/* local variables */
static FILE *lexical_in;
static int lex_line, lex_column;
static int this_char;
static int next_char;
static void read_char ()
{ if (this_char == '\n')
    { lex_line++; lex_column = 1; }
  else if (this_char == '\t')
    /* Unix tabbing convention to tabulate by the eights */
    lex_column = ((lex_column >> 3) + 1) << 3;
  else lex_column++;
  this_char = next_char;
  next_char = fgetc (lexical_in);
}

/*
   White space recognition
*/
static int is_white_space ()
{ if (isspace (this_char))
    { read_char ();
      return (1);
    };
  return (0);
}

/*
   character class recognition
*/
static int is_small_letter (char *value)
{ if (('a' <= this_char) && (this_char <= 'z'))
    { *value = this_char;
      read_char ();
      return (1);
    };
  return (0);
}

static int is_capital_letter (char *value)
{ if (('A' <= this_char) && (this_char <= 'Z'))
    { *value = this_char;
      read_char ();
      return (1);
    };
  return (0);
}

static int is_small_letgit (char *value)
{ if (is_small_letter (value)) return (1);
  if ((('0' <= this_char) && (this_char <= '9')) || (this_char == '_'))
    { *value = this_char;
      read_char ();
      return (1);
    };
  return (0);
}

static int is_capital_letgit (char *value)
{ if (is_capital_letter (value)) return (1);
  if ((('0' <= this_char) && (this_char <= '9')) || (this_char == '_'))
    { *value = this_char;
      read_char ();
      return (1);
    };
  return (0);
}

static int is_digit (char *value)
{ if (('0' <= this_char) && (this_char <= '9'))
    { *value = this_char;
      read_char ();
      return (1);
    };
  return (0);
}

static int is_char (char ch)
{ if (this_char == ch)
    { read_char ();
      return (1);
    };
  return (0);
}

/*
   Comment and pragmat reading
*/
static struct pragmat_rec
{ char *pragmat_text;
  pragmat pragmat_coding;
} pragmats_table[] =
{{ "nonreentrant", nonreentrant },
};
#define NR_PRAGMATS \
(sizeof(pragmats_table)/sizeof(struct pragmat_rec))

static pragmat scan_pragmat ()
{ int column = lex_column;
  int line = lex_line;
  char buf[MAXSTRLEN];
  char *ptr = buf;
  int ix;

  /* pragmats should only consist of small letters */
  while (is_small_letter (ptr)) ptr++;
  *ptr = '\0';

  /* try and recognize this pragmat */
  for (ix = 0; ix < NR_PRAGMATS; ix++)
    if (streq (buf, pragmats_table[ix].pragmat_text))
      return (pragmats_table[ix].pragmat_coding);
  lexical_error (line, column, "illegal pragmat '%s'", buf);
  return (no_pragmat);
}

static void may_skip_layout ()
{ while (is_white_space ());
  while (is_char ('{'))
    { if (is_char ('|'))
	{ pragmat curr = scan_pragmat ();
	  while (is_white_space ());
	  if (is_char ('|') && is_char ('}'))
	    { if (curr != no_pragmat)
	        app_pragmat_list (curr_pragmats, curr);
	    }
	  else lexical_error (lex_line, lex_column, "illegal pragmat end");
	}
      else
	{ while ((this_char != '}') && (this_char != EOF)) read_char ();
	  read_char ();	/* closing } or EOF */
	};
      while (is_white_space ());
    };
}

pragmat_list read_and_reset_pragmats ()
{ pragmat_list curr = curr_pragmats;
  curr_pragmats = new_pragmat_list ();
  return (curr);
}

/*
   class recognition
*/
static void dump_current_symbol ()
{ dcg_eprint ("file %s, line %d, column %d: ", this_fname, this_line, this_column);
  switch (this_symbol)
    { case UNDEFINED:			break;

      /* General symbols */
      case SMALL_NAME:			dcg_eprint ("SMALL NAME: '%s'", this_sval); break;
      case CAPITAL_NAME:		dcg_eprint ("CAPITAL NAME: '%s'", this_sval); break;
      case INTEGER_DENOTER:		dcg_eprint ("INTEGER DENOTER: %d", this_nval); break;
      case REAL_DENOTER:		dcg_eprint ("REAL DENOTER: %.1e", this_rval); break;
      case TEXT_DENOTER:		dcg_eprint ("TEXT DENOTER: '%s'", this_sval); break;

      /* Reserved words */
      case USES_SYMBOL:			dcg_eprint ("USES"); break;
      case END_SYMBOL:			dcg_eprint ("END"); break;
      case PROC_SYMBOL:			dcg_eprint ("PROC"); break;
      case ENDPROC_SYMBOL:		dcg_eprint ("ENDPROC"); break;
      case OP_SYMBOL:			dcg_eprint ("OP"); break;
      case ENDOP_SYMBOL:		dcg_eprint ("ENDOP"); break;
      case LET_SYMBOL:			dcg_eprint ("LET"); break;
      case TYPE_SYMBOL:			dcg_eprint ("TYPE"); break;
      case INT_SYMBOL:			dcg_eprint ("INT"); break;
      case REAL_SYMBOL:			dcg_eprint ("REAL"); break;
      case BOOL_SYMBOL:			dcg_eprint ("BOOL"); break;
      case TEXT_SYMBOL:			dcg_eprint ("TEXT"); break;
      case ROW_SYMBOL:			dcg_eprint ("ROW"); break;
      case ARRAY_SYMBOL:		dcg_eprint ("ARRAY"); break;
      case STRUCT_SYMBOL:		dcg_eprint ("STRUCT"); break;
      case CONST_SYMBOL:		dcg_eprint ("CONST"); break;
      case VAR_SYMBOL:			dcg_eprint ("VAR"); break;
      case IF_SYMBOL:			dcg_eprint ("IF"); break;
      case THEN_SYMBOL:			dcg_eprint ("THEN"); break;
      case ELIF_SYMBOL:			dcg_eprint ("ELIF"); break;
      case ELSE_SYMBOL:			dcg_eprint ("ELSE"); break;
      case ENDIF_SYMBOL:		dcg_eprint ("ENDIF"); break;
      case FOR_SYMBOL:			dcg_eprint ("FOR"); break;
      case FROM_SYMBOL:			dcg_eprint ("FROM"); break;
      case UPTO_SYMBOL:			dcg_eprint ("UPTO"); break;
      case DOWNTO_SYMBOL:		dcg_eprint ("DOWNTO"); break;
      case WHILE_SYMBOL:		dcg_eprint ("WHILE"); break;
      case UNTIL_SYMBOL:		dcg_eprint ("UNTIL"); break;
      case REP_SYMBOL:			dcg_eprint ("REP"); break;
      case ENDREP_SYMBOL:		dcg_eprint ("ENDREP"); break;
      case SELECT_SYMBOL:		dcg_eprint ("SELECT"); break;
      case OF_SYMBOL:			dcg_eprint ("OF"); break;
      case CASE_SYMBOL:			dcg_eprint ("CASE"); break;
      case OTHERWISE_SYMBOL:		dcg_eprint ("OTHERWISE"); break;
      case ENDSELECT_SYMBOL:		dcg_eprint ("ENDSELECT"); break;
      case CONCR_SYMBOL:		dcg_eprint ("CONCR"); break;
      case LEAVE_SYMBOL:		dcg_eprint ("LEAVE"); break;
      case WITH_SYMBOL:			dcg_eprint ("WITH"); break;
      case DIV_SYMBOL:			dcg_eprint ("DIV"); break;
      case MOD_SYMBOL:			dcg_eprint ("MOD"); break;
      case AND_SYMBOL:			dcg_eprint ("AND"); break;
      case OR_SYMBOL:			dcg_eprint ("OR"); break;
      case XOR_SYMBOL:			dcg_eprint ("XOR"); break;
      case NOT_SYMBOL:			dcg_eprint ("NOT"); break;
      case TRUE_SYMBOL:			dcg_eprint ("TRUE"); break;
      case FALSE_SYMBOL:		dcg_eprint ("FALSE"); break;
      case NIL_SYMBOL:			dcg_eprint ("NIL"); break;

      /* Only for packets */
      case PACKET_SYMBOL:		dcg_eprint ("PACKET"); break;
      case ENDPACKET_SYMBOL:		dcg_eprint ("ENDPACKET"); break;
      case DEFINES_SYMBOL:		dcg_eprint ("DEFINES"); break;
      case INTERNAL_SYMBOL:		dcg_eprint ("INTERNAL"); break;
      case EXTERNAL_SYMBOL:		dcg_eprint ("EXTERNAL"); break;
      case USES_LIBRARY_SYMBOL:		dcg_eprint ("USES_LIBRARY"); break;
      case USES_LIBRARY_PATH_SYMBOL:	dcg_eprint ("USES_LIBRARY_PATH"); break;

      /* Punctuation */
      case COLON_SYMBOL:		dcg_eprint (":"); break;
      case INITIAL_SYMBOL:		dcg_eprint ("::"); break;
      case BECOMES_SYMBOL:		dcg_eprint (":="); break;
      case SEMICOLON_SYMBOL:		dcg_eprint (";"); break;
      case COMMA_SYMBOL:		dcg_eprint (","); break;
      case PERIOD_SYMBOL:		dcg_eprint ("."); break;
      case OPEN_SYMBOL:			dcg_eprint ("("); break;
      case CLOSE_SYMBOL:		dcg_eprint (")"); break;
      case SUB_SYMBOL:			dcg_eprint ("["); break;
      case BUS_SYMBOL:			dcg_eprint ("]"); break;
      case PLUS_SYMBOL:			dcg_eprint ("+"); break;
      case MINUS_SYMBOL:		dcg_eprint ("-"); break;
      case ASTERIX_SYMBOL:		dcg_eprint ("*"); break;
      case DIVIDES_SYMBOL:		dcg_eprint ("/"); break;
      case OBELIX_SYMBOL:		dcg_eprint ("**"); break;
      case EQUAL_SYMBOL:		dcg_eprint ("="); break;
      case NOT_EQUAL_SYMBOL:		dcg_eprint ("<>"); break;
      case LESS_THAN_SYMBOL:		dcg_eprint ("<"); break;
      case LESS_EQUAL_SYMBOL:		dcg_eprint ("<="); break;
      case GREATER_THAN_SYMBOL:		dcg_eprint (">"); break;
      case GREATER_EQUAL_SYMBOL:	dcg_eprint (">="); break;

      /* The end */
      case EOF_SYMBOL:			dcg_eprint ("<EOF>"); break;
      default: dcg_bad_tag (this_symbol, "dump_current_symbol");
    };
  dcg_eprint ("\n");
}

static void shift_symbol (int line, int column, symbol sy, string sval, int nval, real rval)
{ this_line = next_line;
  this_column = next_column;
  this_symbol = next_symbol;
  this_sval = next_sval;
  this_nval = next_nval;
  this_rval = next_rval;
  next_line = next2_line;
  next_column = next2_column;
  next_symbol = next2_symbol;
  next_sval = next2_sval;
  next_nval = next2_nval;
  next_rval = next2_rval;
  next2_line = line;
  next2_column = column;
  next2_symbol = sy;
  next2_sval = sval;
  next2_nval = nval;
  next2_rval = rval;
  if (dump_lex) dump_current_symbol ();
}

static void yield_symbol (symbol sy)
{ shift_symbol (lex_line, lex_column, sy, string_nil, 0, 0.0);
  read_char ();
}

static struct reserved_word_rec
{ char *word;
  symbol coding;
} reserved_words_table[] =
{{ "USES", USES_SYMBOL },
 { "END", END_SYMBOL },
 { "PROC", PROC_SYMBOL },
 { "ENDPROC", ENDPROC_SYMBOL },
 { "OP", OP_SYMBOL },
 { "ENDOP", ENDOP_SYMBOL },
 { "LET", LET_SYMBOL },
 { "TYPE", TYPE_SYMBOL },
 { "INT", INT_SYMBOL },
 { "REAL", REAL_SYMBOL },
 { "BOOL", BOOL_SYMBOL },
 { "TEXT", TEXT_SYMBOL },
 { "ROW", ROW_SYMBOL },
 { "ARRAY", ARRAY_SYMBOL },
 { "STRUCT", STRUCT_SYMBOL },
 { "CONST", CONST_SYMBOL },
 { "VAR", VAR_SYMBOL },
 { "IF", IF_SYMBOL },
 { "THEN", THEN_SYMBOL },
 { "ELIF", ELIF_SYMBOL },
 { "ELSE", ELSE_SYMBOL },
 { "ENDIF", ENDIF_SYMBOL },
 { "FI", ENDIF_SYMBOL },
 { "FOR", FOR_SYMBOL },
 { "FROM", FROM_SYMBOL },
 { "UPTO", UPTO_SYMBOL },
 { "DOWNTO", DOWNTO_SYMBOL },
 { "WHILE", WHILE_SYMBOL },
 { "UNTIL", UNTIL_SYMBOL },
 { "REP", REP_SYMBOL },
 { "ENDREP", ENDREP_SYMBOL },
 { "REPEAT", REP_SYMBOL },
 { "ENDREPEAT", ENDREP_SYMBOL },
 { "SELECT", SELECT_SYMBOL },
 { "OF", OF_SYMBOL },
 { "CASE", CASE_SYMBOL },
 { "OTHERWISE", OTHERWISE_SYMBOL },
 { "ENDSELECT", ENDSELECT_SYMBOL },
 { "CONCR", CONCR_SYMBOL },
 { "LEAVE", LEAVE_SYMBOL },
 { "WITH", WITH_SYMBOL },
 { "DIV", DIV_SYMBOL },
 { "MOD", MOD_SYMBOL },
 { "AND", AND_SYMBOL },
 { "OR", OR_SYMBOL },
 { "XOR", XOR_SYMBOL },
 { "NOT", NOT_SYMBOL },
 { "TRUE", TRUE_SYMBOL },
 { "FALSE", FALSE_SYMBOL },
 { "NIL", NIL_SYMBOL },
 { NULL, UNDEFINED },
 { "PACKET", PACKET_SYMBOL },
 { "ENDPACKET", ENDPACKET_SYMBOL },
 { "DEFINES", DEFINES_SYMBOL },
 { "INTERNAL", INTERNAL_SYMBOL },
 { "EXTERNAL", EXTERNAL_SYMBOL },
 { "USES_LIBRARY", USES_LIBRARY_SYMBOL },
 { "USES_LIBRARY_PATH", USES_LIBRARY_PATH_SYMBOL },
};
#define NR_RESERVED_WORDS \
(sizeof(reserved_words_table)/sizeof(struct reserved_word_rec))

static void scan_rest_capital_name (char first_char)
{ char buf[MAXSTRLEN];
  char *ptr = buf;
  int line = lex_line;
  int column = lex_column - 1;
  int ix;
  *ptr++ = first_char;
  while (is_capital_letgit (ptr)) ptr++;
  *ptr = '\0';
  if (iris_compatibility && streq (buf, "USE"))
    { shift_symbol (line, column, USES_SYMBOL, string_nil, 0, 0.0);
      return;
    };
  for (ix = 0; ix < NR_RESERVED_WORDS; ix++)
    if (reserved_words_table[ix].word == NULL)
      { if (analyzing_main) break;
	continue;
      }
    else if (streq (buf, reserved_words_table[ix].word))
      { shift_symbol (line, column, reserved_words_table[ix].coding, string_nil, 0, 0.0);
	return;
      };
  shift_symbol (line, column, CAPITAL_NAME, new_string (buf), 0, 0.0);
}

static void scan_rest_small_name (char first_char)
{ char buf[MAXSTRLEN];
  char *ptr = buf;
  int line = lex_line;
  int column = lex_column - 1;
  *ptr++ = first_char;
  for (;;)
    if (is_small_letgit (ptr)) ptr++;
    else if (is_white_space ()) continue;	/* No comments within names */
    else break;
  *ptr = '\0';
  shift_symbol (line, column, SMALL_NAME, new_string (buf), 0, 0.0);
}

#define MAXINTDIV10 214748364
#define MAXINTP1MOD10 8
static void scan_rest_number (char first_char)
{ char buf[MAXSTRLEN];
  char *ptr = buf;
  int line = lex_line;
  int column = lex_column - 1;
  char dig;
  *ptr++ = first_char;
  while (is_digit (&dig)) *ptr++ = dig;

  /* Creative Mc Carthy OR! */
  if ((this_char != '.') || (next_char < '0') || (next_char > '9'))
    { *ptr = '\0';
      shift_symbol (line, column, INTEGER_DENOTER, string_nil, dcg_convert_integer (buf, 10), 0.0);
      /* add nice convertion with overflow check */
      return;
    };

  if (!is_char ('.')) dcg_internal_error ("scan_rest_number");
  this_symbol = REAL_DENOTER;
  *ptr++ = '.';
  if (!is_digit (&dig)) dcg_internal_error ("scan_rest_number");
  *ptr++ = dig;
  while (is_digit (&dig)) *ptr++ = dig;
  if (is_char ('E') || is_char ('e'))	/* Mc Carthy OR! */
    { *ptr++ = 'e';
      if (is_char ('-')) *ptr++ = '-';
      else if (is_char ('+')) *ptr++ = '+';
      if (!is_digit (&dig))
        { lexical_error (lex_line, lex_column, "digits expected");
	  *ptr = '\0';
          shift_symbol (line, column, REAL_DENOTER, string_nil, 0, dcg_convert_real (buf));
	  return;
	};
      *ptr++ = dig;
      while (is_digit (&dig)) *ptr++ = dig;
    };
  *ptr = '\0';
  shift_symbol (line, column, REAL_DENOTER, string_nil, 0, dcg_convert_real (buf));
}

static void scan_rest_text_denoter ()
{ char buf[MAXSTRLEN];
  char *ptr = buf;
  int line = lex_line;
  int column = lex_column;
  read_char ();
  while (1)
    { if (this_char == '\n')
	{ lexical_error (lex_line, lex_column, "Unterminated text denotation");
	  break;
	}
      else if (this_char == '"')
	{ char dig;
	  read_char ();
	  if (this_char == '"')	/* quote image */
	    { read_char ();
	      *ptr++ = '"';
	    }
	  else if (is_digit (&dig))	/* character image */
	    { char value = dig - '0';
	      while (is_digit (&dig))
		value = value * 10 + dig - '0';
	      *ptr++ = value & 0x7f;
	      if (this_char != '"')
		{ lexical_error (lex_line, lex_column, "Unterminated character image");
		  break;
		}
	      else read_char ();  
	    } 
	  else break;			/* end of string */
	}
      else if (this_char == '\\')
        { *ptr++ = '\\';
	  read_char ();
	  switch (this_char)
	    { case '\\': *ptr++ = '\\'; break;
	      case '"': *ptr++ = '"'; break;
	      case 'f': *ptr++ = 'f'; break;
	      case 'r': *ptr++ = 'r'; break;
	      case 'n': *ptr++ = 'n'; break;
	      case 't': *ptr++ = 't'; break;
	      default:
		lexical_error (lex_line, lex_column, "Illegal escape sequence");
		ptr--;
	    };
	  read_char ();
	}
      else
	{ *ptr++ = this_char;
	  read_char ();
	};
    };
  *ptr = '\0';
  shift_symbol (line, column, TEXT_DENOTER, new_string (buf), 0, 0.0);
}

static void scan_rest_asterix ()
{ int line = lex_line;
  int column = lex_column;
  read_char ();
  if (this_char == '*')
    { read_char ();
      shift_symbol (line, column, OBELIX_SYMBOL, string_nil, 0, 0.0); 
    }
  else shift_symbol (line, column, ASTERIX_SYMBOL, string_nil, 0, 0.0);
}

static void scan_rest_colon ()
{ int line = lex_line;
  int column = lex_column;
  read_char ();
  if (this_char == '=')
    { read_char ();
      shift_symbol (line, column, BECOMES_SYMBOL, string_nil, 0, 0.0);
    }
  else if (this_char == ':')
    { read_char ();
      shift_symbol (line, column, INITIAL_SYMBOL, string_nil, 0, 0.0);
    }
  else shift_symbol (line, column, COLON_SYMBOL, string_nil, 0, 0.0);
}

static void scan_rest_left_bracket ()
{ int line = lex_line;
  int column = lex_column;
  read_char ();
  if (this_char == '=')
    { read_char ();
      shift_symbol (line, column, LESS_EQUAL_SYMBOL, string_nil, 0, 0.0);
    }
  else if (this_char == '>')
    { read_char ();
      shift_symbol (line, column, NOT_EQUAL_SYMBOL, string_nil, 0, 0.0);
    }
  else shift_symbol (line, column, LESS_THAN_SYMBOL, string_nil, 0, 0.0);
}

static void scan_rest_right_bracket ()
{ int line = lex_line;
  int column = lex_column;
  read_char ();
  if (this_char == '=')
    { read_char ();
      shift_symbol (line, column, GREATER_EQUAL_SYMBOL, string_nil, 0, 0.0);
    }
  else shift_symbol (line, column, GREATER_THAN_SYMBOL, string_nil, 0, 0.0);
}

static void handle_illegal_character ()
{ lexical_error (lex_line, lex_column, "illegal character: '%c'", this_char);
  read_char ();
  read_token ();
}

/*
   Read a new token
*/
void read_token ()
{ char first_char;
  may_skip_layout ();
  if (this_symbol == EOF_SYMBOL) return;
  else if (is_capital_letter (&first_char))
    scan_rest_capital_name (first_char);
  else if (is_small_letter (&first_char))
    scan_rest_small_name (first_char);
  else if (is_digit (&first_char))
    scan_rest_number (first_char);
  else
    switch (this_char)
      { case '"': scan_rest_text_denoter (); break;
	case '%': yield_symbol (DIV_SYMBOL); break;
	case '(': yield_symbol (OPEN_SYMBOL); break;
	case ')': yield_symbol (CLOSE_SYMBOL); break;
	case '*': scan_rest_asterix (); break;
	case '+': yield_symbol (PLUS_SYMBOL); break;
	case ',': yield_symbol (COMMA_SYMBOL); break;
	case '-': yield_symbol (MINUS_SYMBOL); break;
	case '.': yield_symbol (PERIOD_SYMBOL); break;
	case '/': yield_symbol (DIVIDES_SYMBOL); break;
	case ':': scan_rest_colon (); break;
	case ';': yield_symbol (SEMICOLON_SYMBOL); break;
	case '<': scan_rest_left_bracket (); break;
	case '=': yield_symbol (EQUAL_SYMBOL); break;
	case '>': scan_rest_right_bracket (); break;
	case '[': yield_symbol (SUB_SYMBOL); break;
	case ']': yield_symbol (BUS_SYMBOL); break;
	case EOF: yield_symbol (EOF_SYMBOL); break;
	default: handle_illegal_character (); break;
      };
}

/*
   Initialize the lexical scanner with a new file
*/
void init_lexer (FILE *fd, char *fname, int main_packet)
{ if (fd == NULL) dcg_internal_error ("init_lexer");
  lexical_in = fd;
  this_fname = new_string (fname);
  analyzing_main = main_packet;

  /* Reset token administration */
  curr_pragmats = new_pragmat_list ();
  this_symbol = UNDEFINED;
  next_symbol = UNDEFINED;
  next2_symbol = UNDEFINED;
  next_char = ' ';
  this_char = ' ';
  lex_line = 1;
  lex_column = -1;
  read_token ();
  read_token ();
  read_token ();
}
