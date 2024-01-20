/*
   File: dcg_plist.c
   Generated on Thu Aug 23 22:26:45 2012
*/

/* standard includes */
#include <stdio.h>

/* support lib includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_alloc.h>
#include <dcg_string.h>
#include <dcg_dump.h>
#include <dcg_binfile.h>

/* local includes */
#include "dcg_plist.h"

/* Allocate new int_list */
int_list init_int_list (int room)
{ int_list new = (int_list) dcg_malloc (sizeof (struct str_int_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (int *) dcg_calloc (safe_room, sizeof (int));
  return (new);
}

/* Allocate new string_list */
string_list init_string_list (int room)
{ string_list new = (string_list) dcg_malloc (sizeof (struct str_string_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (string *) dcg_calloc (safe_room, sizeof (string));
  return (new);
}

/* Announce to use 'room' chunks for int_list */
void room_int_list (int_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (int));
  l -> room = room;
}

/* Announce to use 'room' chunks for string_list */
void room_string_list (string_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (string));
  l -> room = room;
}

/* Recursively detach a int_list */
void detach_int_list (int_list *lp)
{ int ix;
  int_list old = (int_list) dcg_predetach ((void **) lp);
  if (old == int_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_int (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a string_list */
void detach_string_list (string_list *lp)
{ int ix;
  string_list old = (string_list) dcg_predetach ((void **) lp);
  if (old == string_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_string (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a int_list without detaching elements */
void nonrec_detach_int_list (int_list *lp)
{ int_list old = (int_list) dcg_predetach ((void **) lp);
  if (old == int_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a string_list without detaching elements */
void nonrec_detach_string_list (string_list *lp)
{ string_list old = (string_list) dcg_predetach ((void **) lp);
  if (old == string_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Append element to int_list */
int_list append_int_list (int_list l, int el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_int_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to string_list */
string_list append_string_list (string_list l, string el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_string_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Concatenate two int_lists */
int_list concat_int_list (int_list l1, int_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_int_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two string_lists */
string_list concat_string_list (string_list l1, string_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_string_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Insert element into int_list at pos 'pos' */
int_list insert_int_list (int_list l, int pos, int el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_int_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_int_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into string_list at pos 'pos' */
string_list insert_string_list (string_list l, int pos, string el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_string_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_string_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Delete element from int_list at pos 'pos' */
int_list delete_int_list (int_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_int_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from string_list at pos 'pos' */
string_list delete_string_list (string_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_string_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Comparison test for a int_list */
int cmp_int_list (int_list a, int_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == int_list_nil) return (-1);
  if (b == int_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_int (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a string_list */
int cmp_string_list (string_list a, string_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == string_list_nil) return (-1);
  if (b == string_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_string (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Estimate printing a int_list */
int est_int_list (int_list old)
{ int ix;
  int size = 0;
  if (old == int_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_int (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a string_list */
int est_string_list (string_list old)
{ int ix;
  int size = 0;
  if (old == string_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_string (old -> array[ix]) + 2;
  return (size + 2);
}

/* Pretty print a int_list */
void ppp_int_list (FILE *f, int horiz, int ind, int_list old)
{ int ix, mhoriz;
  if (old == int_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_int_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_int (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a string_list */
void ppp_string_list (FILE *f, int horiz, int ind, string_list old)
{ int ix, mhoriz;
  if (old == string_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_string_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_string (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Recursively save a int_list */
void save_int_list (BinFile bf, int_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_int (bf, l -> array[ix]);
}

/* Recursively save a string_list */
void save_string_list (BinFile bf, string_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_string (bf, l -> array[ix]);
}

/* Recursively load a int_list */
void load_int_list (BinFile bf, int_list *l)
{ int ix, size;
  int_list new;
  loadsize (bf, &size);
  new = init_int_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_int (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a string_list */
void load_string_list (BinFile bf, string_list *l)
{ int ix, size;
  string_list new;
  loadsize (bf, &size);
  new = init_string_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_string (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively duplicate a int_list */
int_list rdup_int_list (int_list old)
{ int_list new;
  int ix;
  if (old == int_list_nil) return (old);
  new = init_int_list (old -> size);
  for (ix = 0; ix < old -> size; ix++)
    app_int_list (new, rdup_int (old -> array[ix]));
  return (new);
}

/* Recursively duplicate a string_list */
string_list rdup_string_list (string_list old)
{ string_list new;
  int ix;
  if (old == string_list_nil) return (old);
  new = init_string_list (old -> size);
  for (ix = 0; ix < old -> size; ix++)
    app_string_list (new, rdup_string (old -> array[ix]));
  return (new);
}

/* Recursively check the refcounts of a int_list */
void check_not_freed_int_list (int_list curr)
{ int ix;
  if (curr == int_list_nil) return;
  for (ix = 0; ix < curr -> size; ix++)
    check_not_freed_int (curr -> array[ix]);
}

/* Recursively check the refcounts of a string_list */
void check_not_freed_string_list (string_list curr)
{ int ix;
  if (curr == string_list_nil) return;
  for (ix = 0; ix < curr -> size; ix++)
    check_not_freed_string (curr -> array[ix]);
}

