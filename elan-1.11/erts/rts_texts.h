/*
   File: rts_texts.h
   Provides text engine

   CVS ID: "$Id: rts_texts.h,v 1.4 2004/12/18 13:24:51 marcs Exp $"
*/

#ifndef IncRtsTexts
#define IncRtsTexts

#define MAX_TEXT_LEN 32767

extern char *rts_new_text (char *a);
extern char *rts_ascii_int (int nr);
extern char *rts_text_plus_text (char *a, char *b);
extern int rts_length_text (char *a);
extern char *rts_int_times_text (int a, char *b);
extern char *rts_text_sub_int (char *a, int idx);
extern int rts_pos_text_text (char *a, char *b);
extern char *rts_subtext_text_int_int (char *a, int from, int to);
extern int rts_text_equal_text (char *a, char *b);
extern int rts_text_not_equal_text (char *a, char *b);
extern int rts_text_less_than_text (char *a, char *b);
extern int rts_text_less_equal_text (char *a, char *b);
extern int rts_text_greater_than_text (char *a, char *b);
extern int rts_text_greater_equal_text (char *a, char *b);
extern int rts_text_compare (char *a, char *b);

#endif /* IncRtsTexts */
