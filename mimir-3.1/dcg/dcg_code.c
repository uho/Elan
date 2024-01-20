/*
   File: dcg_code.c
   Generated on Tue Aug 21 21:59:43 2012

   Copyright (C) 2011 Marc Seutter

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   CVS ID: "$Id: dcg_code.c,v 1.12 2012/08/21 20:05:32 marcs Exp $"
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
#include "dcg_code.h"

/* Allocate new def_list */
def_list init_def_list (int room)
{ def_list new = (def_list) dcg_malloc (sizeof (struct str_def_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (def *) dcg_calloc (safe_room, sizeof (def));
  return (new);
}

/* Allocate new stat_list */
stat_list init_stat_list (int room)
{ stat_list new = (stat_list) dcg_malloc (sizeof (struct str_stat_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (stat *) dcg_calloc (safe_room, sizeof (stat));
  return (new);
}

/* Allocate new field_list */
field_list init_field_list (int room)
{ field_list new = (field_list) dcg_malloc (sizeof (struct str_field_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (field *) dcg_calloc (safe_room, sizeof (field));
  return (new);
}

/* Allocate new vfield_list */
vfield_list init_vfield_list (int room)
{ vfield_list new = (vfield_list) dcg_malloc (sizeof (struct str_vfield_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (vfield *) dcg_calloc (safe_room, sizeof (vfield));
  return (new);
}

/* Allocate new type_list */
type_list init_type_list (int room)
{ type_list new = (type_list) dcg_malloc (sizeof (struct str_type_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (type *) dcg_calloc (safe_room, sizeof (type));
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

/* Announce to use 'room' chunks for def_list */
void room_def_list (def_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (def));
  l -> room = room;
}

/* Announce to use 'room' chunks for stat_list */
void room_stat_list (stat_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (stat));
  l -> room = room;
}

/* Announce to use 'room' chunks for field_list */
void room_field_list (field_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (field));
  l -> room = room;
}

/* Announce to use 'room' chunks for vfield_list */
void room_vfield_list (vfield_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (vfield));
  l -> room = room;
}

/* Announce to use 'room' chunks for type_list */
void room_type_list (type_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (type));
  l -> room = room;
}

/* Announce to use 'room' chunks for string_list */
void room_string_list (string_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (string));
  l -> room = room;
}

/* Allocate new def record for constructor Primitive */
def new_Primitive (string lhs)
{ def new = (def) dcg_malloc (sizeof (struct str_def));
  new -> lhs = lhs;
  new -> nrlsts = int_nil;
  new -> implsts = int_nil;
  new -> tag = TAGPrimitive;
  return (new);
}

/* Allocate new def record for constructor Enum */
def new_Enum (string lhs,
		string_list elems)
{ def new = (def) dcg_malloc (sizeof (struct str_def));
  new -> lhs = lhs;
  new -> nrlsts = int_nil;
  new -> implsts = int_nil;
  new -> tag = TAGEnum;
  new -> Enum.elems = elems;
  return (new);
}

/* Allocate new def record for constructor Record */
def new_Record (string lhs,
		field_list fixed, vfield_list variant)
{ def new = (def) dcg_malloc (sizeof (struct str_def));
  new -> lhs = lhs;
  new -> nrlsts = int_nil;
  new -> implsts = int_nil;
  new -> tag = TAGRecord;
  new -> Record.fixed = fixed;
  new -> Record.variant = variant;
  return (new);
}

/* Allocate new stat record for constructor Use */
stat new_Use (type_list utype)
{ stat new = (stat) dcg_malloc (sizeof (struct str_stat));
  new -> tag = TAGUse;
  new -> Use.utype = utype;
  return (new);
}

/* Allocate new stat record for constructor Import */
stat new_Import (string imp)
{ stat new = (stat) dcg_malloc (sizeof (struct str_stat));
  new -> tag = TAGImport;
  new -> Import.imp = imp;
  return (new);
}

/* Allocate new field record */
field new_field (string fname, type ftype, int ftrav)
{ field new = (field) dcg_malloc (sizeof (struct str_field));
  new -> fname = fname;
  new -> ftype = ftype;
  new -> ftrav = ftrav;
  return (new);
};

/* Allocate new vfield record */
vfield new_vfield (string cons, field_list parts)
{ vfield new = (vfield) dcg_malloc (sizeof (struct str_vfield));
  new -> cons = cons;
  new -> parts = parts;
  return (new);
};

/* Allocate new type record for constructor Tname */
type new_Tname (string tname)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tag = TAGTname;
  new -> Tname.tname = tname;
  return (new);
}

/* Allocate new type record for constructor Tlist */
type new_Tlist (type etyp)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tag = TAGTlist;
  new -> Tlist.etyp = etyp;
  return (new);
}

/* Recursively detach a def */
void detach_def (def *optr)
{ def old = (def) dcg_predetach ((void **) optr);
  if (old == def_nil) return;
  detach_string (&(old -> lhs));
  switch (old -> tag)
    { case TAGPrimitive:
	break;
      case TAGEnum:
	detach_string_list (&(old -> Enum.elems));
	break;
      case TAGRecord:
	detach_field_list (&(old -> Record.fixed));
	detach_vfield_list (&(old -> Record.variant));
	break;
      default:
	dcg_bad_tag ((int) old -> tag, "detach_def");
    };
  dcg_detach ((void **) &old);
}

/* Recursively detach a stat */
void detach_stat (stat *optr)
{ stat old = (stat) dcg_predetach ((void **) optr);
  if (old == stat_nil) return;
  switch (old -> tag)
    { case TAGUse:
	detach_type_list (&(old -> Use.utype));
	break;
      case TAGImport:
	detach_string (&(old -> Import.imp));
	break;
      default:
	dcg_bad_tag ((int) old -> tag, "detach_stat");
    };
  dcg_detach ((void **) &old);
}

/* Recursively detach a field */
void detach_field (field *optr)
{ field old = (field) dcg_predetach ((void **) optr);
  if (old == field_nil) return;
  detach_string (&(old -> fname));
  detach_type (&(old -> ftype));
  detach_int (&(old -> ftrav));
  dcg_detach ((void **) &old);
}

/* Recursively detach a vfield */
void detach_vfield (vfield *optr)
{ vfield old = (vfield) dcg_predetach ((void **) optr);
  if (old == vfield_nil) return;
  detach_string (&(old -> cons));
  detach_field_list (&(old -> parts));
  dcg_detach ((void **) &old);
}

/* Recursively detach a type */
void detach_type (type *optr)
{ type old = (type) dcg_predetach ((void **) optr);
  if (old == type_nil) return;
  switch (old -> tag)
    { case TAGTname:
	detach_string (&(old -> Tname.tname));
	break;
      case TAGTlist:
	detach_type (&(old -> Tlist.etyp));
	break;
      default:
	dcg_bad_tag ((int) old -> tag, "detach_type");
    };
  dcg_detach ((void **) &old);
}

/* Recursively detach a def_list */
void detach_def_list (def_list *lp)
{ int ix;
  def_list old = (def_list) dcg_predetach ((void **) lp);
  if (old == def_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_def (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a stat_list */
void detach_stat_list (stat_list *lp)
{ int ix;
  stat_list old = (stat_list) dcg_predetach ((void **) lp);
  if (old == stat_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_stat (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a field_list */
void detach_field_list (field_list *lp)
{ int ix;
  field_list old = (field_list) dcg_predetach ((void **) lp);
  if (old == field_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_field (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a vfield_list */
void detach_vfield_list (vfield_list *lp)
{ int ix;
  vfield_list old = (vfield_list) dcg_predetach ((void **) lp);
  if (old == vfield_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_vfield (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a type_list */
void detach_type_list (type_list *lp)
{ int ix;
  type_list old = (type_list) dcg_predetach ((void **) lp);
  if (old == type_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_type (&(old -> array[ix]));
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

/* Detach a def_list without detaching elements */
void nonrec_detach_def_list (def_list *lp)
{ def_list old = (def_list) dcg_predetach ((void **) lp);
  if (old == def_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a stat_list without detaching elements */
void nonrec_detach_stat_list (stat_list *lp)
{ stat_list old = (stat_list) dcg_predetach ((void **) lp);
  if (old == stat_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a field_list without detaching elements */
void nonrec_detach_field_list (field_list *lp)
{ field_list old = (field_list) dcg_predetach ((void **) lp);
  if (old == field_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a vfield_list without detaching elements */
void nonrec_detach_vfield_list (vfield_list *lp)
{ vfield_list old = (vfield_list) dcg_predetach ((void **) lp);
  if (old == vfield_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a type_list without detaching elements */
void nonrec_detach_type_list (type_list *lp)
{ type_list old = (type_list) dcg_predetach ((void **) lp);
  if (old == type_list_nil) return;
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

/* Append element to def_list */
def_list append_def_list (def_list l, def el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_def_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to stat_list */
stat_list append_stat_list (stat_list l, stat el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_stat_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to field_list */
field_list append_field_list (field_list l, field el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_field_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to vfield_list */
vfield_list append_vfield_list (vfield_list l, vfield el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_vfield_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to type_list */
type_list append_type_list (type_list l, type el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_type_list (l, l -> size << 1);
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

/* Concatenate two def_lists */
def_list concat_def_list (def_list l1, def_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_def_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two stat_lists */
stat_list concat_stat_list (stat_list l1, stat_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_stat_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two field_lists */
field_list concat_field_list (field_list l1, field_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_field_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two vfield_lists */
vfield_list concat_vfield_list (vfield_list l1, vfield_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_vfield_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two type_lists */
type_list concat_type_list (type_list l1, type_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_type_list (l1, l1 -> size + l2 -> size);
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

/* Insert element into def_list at pos 'pos' */
def_list insert_def_list (def_list l, int pos, def el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_def_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_def_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into stat_list at pos 'pos' */
stat_list insert_stat_list (stat_list l, int pos, stat el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_stat_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_stat_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into field_list at pos 'pos' */
field_list insert_field_list (field_list l, int pos, field el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_field_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_field_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into vfield_list at pos 'pos' */
vfield_list insert_vfield_list (vfield_list l, int pos, vfield el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_vfield_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_vfield_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into type_list at pos 'pos' */
type_list insert_type_list (type_list l, int pos, type el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_type_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_type_list (l, l -> size << 1);
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

/* Delete element from def_list at pos 'pos' */
def_list delete_def_list (def_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_def_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from stat_list at pos 'pos' */
stat_list delete_stat_list (stat_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_stat_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from field_list at pos 'pos' */
field_list delete_field_list (field_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_field_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from vfield_list at pos 'pos' */
vfield_list delete_vfield_list (vfield_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_vfield_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from type_list at pos 'pos' */
type_list delete_type_list (type_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_type_list: position %d out of range", pos);
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

/* Comparison test for a def */
int cmp_def (def a, def b)
{ int s;
  if (a == b) return (0);
  if (a == def_nil) return (-1);
  if (b == def_nil) return (1);
  if ((s = cmp_string (a -> lhs, b -> lhs))) return (s);
  if ((s = cmp_int ((int)(a -> tag), (int)(b -> tag)))) return (s);
  switch (a -> tag)
    { case TAGPrimitive:
	break;
      case TAGEnum:
	if ((s = cmp_string_list (a -> Enum.elems, b -> Enum.elems))) return (s);
	break;
      case TAGRecord:
	if ((s = cmp_field_list (a -> Record.fixed, b -> Record.fixed))) return (s);
	if ((s = cmp_vfield_list (a -> Record.variant, b -> Record.variant))) return (s);
	break;
      default:
	dcg_bad_tag (a -> tag, "cmp_def");
    };
  return (0);
}

/* Comparison test for a stat */
int cmp_stat (stat a, stat b)
{ int s;
  if (a == b) return (0);
  if (a == stat_nil) return (-1);
  if (b == stat_nil) return (1);
  if ((s = cmp_int ((int)(a -> tag), (int)(b -> tag)))) return (s);
  switch (a -> tag)
    { case TAGUse:
	if ((s = cmp_type_list (a -> Use.utype, b -> Use.utype))) return (s);
	break;
      case TAGImport:
	if ((s = cmp_string (a -> Import.imp, b -> Import.imp))) return (s);
	break;
      default:
	dcg_bad_tag (a -> tag, "cmp_stat");
    };
  return (0);
}

/* Comparison test for a field */
int cmp_field (field a, field b)
{ int s;
  if (a == b) return (0);
  if (a == field_nil) return (-1);
  if (b == field_nil) return (1);
  if ((s = cmp_string (a -> fname, b -> fname))) return (s);
  if ((s = cmp_type (a -> ftype, b -> ftype))) return (s);
  if ((s = cmp_int (a -> ftrav, b -> ftrav))) return (s);
  return (0);
}

/* Comparison test for a vfield */
int cmp_vfield (vfield a, vfield b)
{ int s;
  if (a == b) return (0);
  if (a == vfield_nil) return (-1);
  if (b == vfield_nil) return (1);
  if ((s = cmp_string (a -> cons, b -> cons))) return (s);
  if ((s = cmp_field_list (a -> parts, b -> parts))) return (s);
  return (0);
}

/* Comparison test for a type */
int cmp_type (type a, type b)
{ int s;
  if (a == b) return (0);
  if (a == type_nil) return (-1);
  if (b == type_nil) return (1);
  if ((s = cmp_int ((int)(a -> tag), (int)(b -> tag)))) return (s);
  switch (a -> tag)
    { case TAGTname:
	if ((s = cmp_string (a -> Tname.tname, b -> Tname.tname))) return (s);
	break;
      case TAGTlist:
	if ((s = cmp_type (a -> Tlist.etyp, b -> Tlist.etyp))) return (s);
	break;
      default:
	dcg_bad_tag (a -> tag, "cmp_type");
    };
  return (0);
}

/* Comparison test for a def_list */
int cmp_def_list (def_list a, def_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == def_list_nil) return (-1);
  if (b == def_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_def (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a stat_list */
int cmp_stat_list (stat_list a, stat_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == stat_list_nil) return (-1);
  if (b == stat_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_stat (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a field_list */
int cmp_field_list (field_list a, field_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == field_list_nil) return (-1);
  if (b == field_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_field (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a vfield_list */
int cmp_vfield_list (vfield_list a, vfield_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == vfield_list_nil) return (-1);
  if (b == vfield_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_vfield (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a type_list */
int cmp_type_list (type_list a, type_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == type_list_nil) return (-1);
  if (b == type_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_type (a -> array[ix], b -> array[ix])))
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

/* Estimate printing a def */
int est_def (def old)
{ int size = 0;
  if (old == def_nil) return (2);
  size += est_string (old -> lhs) + 2;
  size += PTRWIDTH;
  size += PTRWIDTH;
  switch (old -> tag)
    { case TAGPrimitive:
	size += 11;
	break;
      case TAGEnum:
	size += 6;
	size += est_string_list (old -> Enum.elems) + 2;
	break;
      case TAGRecord:
	size += 8;
	size += est_field_list (old -> Record.fixed) + 2;
	size += est_vfield_list (old -> Record.variant) + 2;
	break;
      default:
	dcg_bad_tag (old -> tag, "est_def");
    };
  return (size + 2);
}

/* Estimate printing a stat */
int est_stat (stat old)
{ int size = 0;
  if (old == stat_nil) return (2);
  switch (old -> tag)
    { case TAGUse:
	size += 5;
	size += est_type_list (old -> Use.utype) + 2;
	break;
      case TAGImport:
	size += 8;
	size += est_string (old -> Import.imp) + 2;
	break;
      default:
	dcg_bad_tag (old -> tag, "est_stat");
    };
  return (size + 2);
}

/* Estimate printing a field */
int est_field (field old)
{ int size = 0;
  if (old == field_nil) return (2);
  size += est_string (old -> fname) + 2;
  size += est_type (old -> ftype) + 2;
  size += est_int (old -> ftrav) + 2;
  return (size + 2);
}

/* Estimate printing a vfield */
int est_vfield (vfield old)
{ int size = 0;
  if (old == vfield_nil) return (2);
  size += est_string (old -> cons) + 2;
  size += est_field_list (old -> parts) + 2;
  return (size + 2);
}

/* Estimate printing a type */
int est_type (type old)
{ int size = 0;
  if (old == type_nil) return (2);
  switch (old -> tag)
    { case TAGTname:
	size += 7;
	size += est_string (old -> Tname.tname) + 2;
	break;
      case TAGTlist:
	size += 7;
	size += est_type (old -> Tlist.etyp) + 2;
	break;
      default:
	dcg_bad_tag (old -> tag, "est_type");
    };
  return (size + 2);
}

/* Estimate printing a def_list */
int est_def_list (def_list old)
{ int ix;
  int size = 0;
  if (old == def_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_def (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a stat_list */
int est_stat_list (stat_list old)
{ int ix;
  int size = 0;
  if (old == stat_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_stat (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a field_list */
int est_field_list (field_list old)
{ int ix;
  int size = 0;
  if (old == field_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_field (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a vfield_list */
int est_vfield_list (vfield_list old)
{ int ix;
  int size = 0;
  if (old == vfield_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_vfield (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a type_list */
int est_type_list (type_list old)
{ int ix;
  int size = 0;
  if (old == type_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_type (old -> array[ix]) + 2;
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

/* Pretty print a def */
void ppp_def (FILE *f, int horiz, int ind, def old)
{ int mhoriz;
  if (old == def_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_def (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_string (f, mhoriz, ind + 2, old -> lhs);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> nrlsts);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> implsts);
  pppdelim (f, mhoriz, ind, ',');
  switch (old -> tag)
    { case TAGPrimitive:
	pppstring (f, "Primitive");
	break;
      case TAGEnum:
	pppstring (f, "Enum");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string_list (f, mhoriz, ind + 2, old -> Enum.elems);
	break;
      case TAGRecord:
	pppstring (f, "Record");
	pppdelim (f, mhoriz, ind, ',');
	ppp_field_list (f, mhoriz, ind + 2, old -> Record.fixed);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vfield_list (f, mhoriz, ind + 2, old -> Record.variant);
	break;
      default:
	dcg_bad_tag (old -> tag, "ppp_def");
    };
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a stat */
void ppp_stat (FILE *f, int horiz, int ind, stat old)
{ int mhoriz;
  if (old == stat_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_stat (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  switch (old -> tag)
    { case TAGUse:
	pppstring (f, "Use");
	pppdelim (f, mhoriz, ind, ',');
	ppp_type_list (f, mhoriz, ind + 2, old -> Use.utype);
	break;
      case TAGImport:
	pppstring (f, "Import");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Import.imp);
	break;
      default:
	dcg_bad_tag (old -> tag, "ppp_stat");
    };
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a field */
void ppp_field (FILE *f, int horiz, int ind, field old)
{ int mhoriz;
  if (old == field_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_field (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_string (f, mhoriz, ind + 2, old -> fname);
  pppdelim (f, mhoriz, ind, ',');
  ppp_type (f, mhoriz, ind + 2, old -> ftype);
  pppdelim (f, mhoriz, ind, ',');
  ppp_int (f, mhoriz, ind + 2, old -> ftrav);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a vfield */
void ppp_vfield (FILE *f, int horiz, int ind, vfield old)
{ int mhoriz;
  if (old == vfield_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_vfield (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_string (f, mhoriz, ind + 2, old -> cons);
  pppdelim (f, mhoriz, ind, ',');
  ppp_field_list (f, mhoriz, ind + 2, old -> parts);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a type */
void ppp_type (FILE *f, int horiz, int ind, type old)
{ int mhoriz;
  if (old == type_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_type (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  switch (old -> tag)
    { case TAGTname:
	pppstring (f, "Tname");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Tname.tname);
	break;
      case TAGTlist:
	pppstring (f, "Tlist");
	pppdelim (f, mhoriz, ind, ',');
	ppp_type (f, mhoriz, ind + 2, old -> Tlist.etyp);
	break;
      default:
	dcg_bad_tag (old -> tag, "ppp_type");
    };
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a def_list */
void ppp_def_list (FILE *f, int horiz, int ind, def_list old)
{ int ix, mhoriz;
  if (old == def_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_def_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_def (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a stat_list */
void ppp_stat_list (FILE *f, int horiz, int ind, stat_list old)
{ int ix, mhoriz;
  if (old == stat_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_stat_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_stat (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a field_list */
void ppp_field_list (FILE *f, int horiz, int ind, field_list old)
{ int ix, mhoriz;
  if (old == field_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_field_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_field (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a vfield_list */
void ppp_vfield_list (FILE *f, int horiz, int ind, vfield_list old)
{ int ix, mhoriz;
  if (old == vfield_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_vfield_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_vfield (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a type_list */
void ppp_type_list (FILE *f, int horiz, int ind, type_list old)
{ int ix, mhoriz;
  if (old == type_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_type_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_type (f, mhoriz, ind + 2, old -> array[ix]);
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

/* Recursively save a def */
void save_def (BinFile bf, def old)
{ /* save fixed fields before variant ones */
  save_string (bf, old -> lhs);
  save_char (bf, (char) old -> tag);
  switch (old -> tag)
    { case TAGPrimitive:
	break;
      case TAGEnum:
	save_string_list (bf, old -> Enum.elems);
	break;
      case TAGRecord:
	save_field_list (bf, old -> Record.fixed);
	save_vfield_list (bf, old -> Record.variant);
	break;
      default:
	dcg_bad_tag (old -> tag, "save_def");
    };
}

/* Recursively save a stat */
void save_stat (BinFile bf, stat old)
{ /* save fixed fields before variant ones */
  save_char (bf, (char) old -> tag);
  switch (old -> tag)
    { case TAGUse:
	save_type_list (bf, old -> Use.utype);
	break;
      case TAGImport:
	save_string (bf, old -> Import.imp);
	break;
      default:
	dcg_bad_tag (old -> tag, "save_stat");
    };
}

/* Recursively save a field */
void save_field (BinFile bf, field old)
{ /* save fixed fields before variant ones */
  save_string (bf, old -> fname);
  save_type (bf, old -> ftype);
  save_int (bf, old -> ftrav);
}

/* Recursively save a vfield */
void save_vfield (BinFile bf, vfield old)
{ /* save fixed fields before variant ones */
  save_string (bf, old -> cons);
  save_field_list (bf, old -> parts);
}

/* Recursively save a type */
void save_type (BinFile bf, type old)
{ /* save fixed fields before variant ones */
  save_char (bf, (char) old -> tag);
  switch (old -> tag)
    { case TAGTname:
	save_string (bf, old -> Tname.tname);
	break;
      case TAGTlist:
	save_type (bf, old -> Tlist.etyp);
	break;
      default:
	dcg_bad_tag (old -> tag, "save_type");
    };
}

/* Recursively save a def_list */
void save_def_list (BinFile bf, def_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_def (bf, l -> array[ix]);
}

/* Recursively save a stat_list */
void save_stat_list (BinFile bf, stat_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_stat (bf, l -> array[ix]);
}

/* Recursively save a field_list */
void save_field_list (BinFile bf, field_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_field (bf, l -> array[ix]);
}

/* Recursively save a vfield_list */
void save_vfield_list (BinFile bf, vfield_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_vfield (bf, l -> array[ix]);
}

/* Recursively save a type_list */
void save_type_list (BinFile bf, type_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_type (bf, l -> array[ix]);
}

/* Recursively save a string_list */
void save_string_list (BinFile bf, string_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_string (bf, l -> array[ix]);
}

/* Recursively load a def */
void load_def (BinFile bf, def *x)
{ /* load fixed fields before variant ones */
  def new = (def) dcg_malloc (sizeof (struct str_def));
  char ch;
  load_string (bf, &new -> lhs);
  new -> nrlsts = int_nil;
  new -> implsts = int_nil;
  load_char (bf, &ch);
  new -> tag = (tags_def) ch;
  switch (new -> tag)
    { case TAGPrimitive:
	break;
      case TAGEnum:
	load_string_list (bf, &new -> Enum.elems);
	break;
      case TAGRecord:
	load_field_list (bf, &new -> Record.fixed);
	load_vfield_list (bf, &new -> Record.variant);
	break;
      default:
	dcg_bad_tag (new -> tag, "load_def");
    };
  *x = new;
}

/* Recursively load a stat */
void load_stat (BinFile bf, stat *x)
{ /* load fixed fields before variant ones */
  stat new = (stat) dcg_malloc (sizeof (struct str_stat));
  char ch;
  load_char (bf, &ch);
  new -> tag = (tags_stat) ch;
  switch (new -> tag)
    { case TAGUse:
	load_type_list (bf, &new -> Use.utype);
	break;
      case TAGImport:
	load_string (bf, &new -> Import.imp);
	break;
      default:
	dcg_bad_tag (new -> tag, "load_stat");
    };
  *x = new;
}

/* Recursively load a field */
void load_field (BinFile bf, field *x)
{ /* load fixed fields before variant ones */
  field new = (field) dcg_malloc (sizeof (struct str_field));
  load_string (bf, &new -> fname);
  load_type (bf, &new -> ftype);
  load_int (bf, &new -> ftrav);
  *x = new;
}

/* Recursively load a vfield */
void load_vfield (BinFile bf, vfield *x)
{ /* load fixed fields before variant ones */
  vfield new = (vfield) dcg_malloc (sizeof (struct str_vfield));
  load_string (bf, &new -> cons);
  load_field_list (bf, &new -> parts);
  *x = new;
}

/* Recursively load a type */
void load_type (BinFile bf, type *x)
{ /* load fixed fields before variant ones */
  type new = (type) dcg_malloc (sizeof (struct str_type));
  char ch;
  load_char (bf, &ch);
  new -> tag = (tags_type) ch;
  switch (new -> tag)
    { case TAGTname:
	load_string (bf, &new -> Tname.tname);
	break;
      case TAGTlist:
	load_type (bf, &new -> Tlist.etyp);
	break;
      default:
	dcg_bad_tag (new -> tag, "load_type");
    };
  *x = new;
}

/* Recursively load a def_list */
void load_def_list (BinFile bf, def_list *l)
{ int ix, size;
  def_list new;
  loadsize (bf, &size);
  new = init_def_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_def (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a stat_list */
void load_stat_list (BinFile bf, stat_list *l)
{ int ix, size;
  stat_list new;
  loadsize (bf, &size);
  new = init_stat_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_stat (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a field_list */
void load_field_list (BinFile bf, field_list *l)
{ int ix, size;
  field_list new;
  loadsize (bf, &size);
  new = init_field_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_field (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a vfield_list */
void load_vfield_list (BinFile bf, vfield_list *l)
{ int ix, size;
  vfield_list new;
  loadsize (bf, &size);
  new = init_vfield_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_vfield (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a type_list */
void load_type_list (BinFile bf, type_list *l)
{ int ix, size;
  type_list new;
  loadsize (bf, &size);
  new = init_type_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_type (bf, &new -> array[ix]);
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

/* Recursively duplicate a def */
def rdup_def (def old)
{ /* allocate new record */
  def new;
  if (old == def_nil) return (old);
  new = (def) dcg_malloc (sizeof (struct str_def));

  /* duplicate fixed fields before variant ones */
  new -> lhs = rdup_string (old -> lhs);
  new -> tag = old -> tag;
  switch (old -> tag)
    { case TAGPrimitive:
	break;
      case TAGEnum:
	new -> Enum.elems = rdup_string_list (old -> Enum.elems);
	break;
      case TAGRecord:
	new -> Record.fixed = rdup_field_list (old -> Record.fixed);
	new -> Record.variant = rdup_vfield_list (old -> Record.variant);
	break;
      default:
	dcg_bad_tag (old -> tag, "rdup_def");
    };
  return (new);
}

/* Recursively duplicate a stat */
stat rdup_stat (stat old)
{ /* allocate new record */
  stat new;
  if (old == stat_nil) return (old);
  new = (stat) dcg_malloc (sizeof (struct str_stat));

  /* duplicate fixed fields before variant ones */
  new -> tag = old -> tag;
  switch (old -> tag)
    { case TAGUse:
	new -> Use.utype = rdup_type_list (old -> Use.utype);
	break;
      case TAGImport:
	new -> Import.imp = rdup_string (old -> Import.imp);
	break;
      default:
	dcg_bad_tag (old -> tag, "rdup_stat");
    };
  return (new);
}

/* Recursively duplicate a field */
field rdup_field (field old)
{ /* allocate new record */
  field new;
  if (old == field_nil) return (old);
  new = (field) dcg_malloc (sizeof (struct str_field));

  /* duplicate fixed fields before variant ones */
  new -> fname = rdup_string (old -> fname);
  new -> ftype = rdup_type (old -> ftype);
  new -> ftrav = rdup_int (old -> ftrav);
  return (new);
}

/* Recursively duplicate a vfield */
vfield rdup_vfield (vfield old)
{ /* allocate new record */
  vfield new;
  if (old == vfield_nil) return (old);
  new = (vfield) dcg_malloc (sizeof (struct str_vfield));

  /* duplicate fixed fields before variant ones */
  new -> cons = rdup_string (old -> cons);
  new -> parts = rdup_field_list (old -> parts);
  return (new);
}

/* Recursively duplicate a type */
type rdup_type (type old)
{ /* allocate new record */
  type new;
  if (old == type_nil) return (old);
  new = (type) dcg_malloc (sizeof (struct str_type));

  /* duplicate fixed fields before variant ones */
  new -> tag = old -> tag;
  switch (old -> tag)
    { case TAGTname:
	new -> Tname.tname = rdup_string (old -> Tname.tname);
	break;
      case TAGTlist:
	new -> Tlist.etyp = rdup_type (old -> Tlist.etyp);
	break;
      default:
	dcg_bad_tag (old -> tag, "rdup_type");
    };
  return (new);
}

/* Recursively duplicate a def_list */
def_list rdup_def_list (def_list old)
{ def_list new;
  int ix;
  if (old == def_list_nil) return (old);
  new = init_def_list (old -> size);
  for (ix = 0; ix < old -> size; ix++)
    app_def_list (new, rdup_def (old -> array[ix]));
  return (new);
}

/* Recursively duplicate a stat_list */
stat_list rdup_stat_list (stat_list old)
{ stat_list new;
  int ix;
  if (old == stat_list_nil) return (old);
  new = init_stat_list (old -> size);
  for (ix = 0; ix < old -> size; ix++)
    app_stat_list (new, rdup_stat (old -> array[ix]));
  return (new);
}

/* Recursively duplicate a field_list */
field_list rdup_field_list (field_list old)
{ field_list new;
  int ix;
  if (old == field_list_nil) return (old);
  new = init_field_list (old -> size);
  for (ix = 0; ix < old -> size; ix++)
    app_field_list (new, rdup_field (old -> array[ix]));
  return (new);
}

/* Recursively duplicate a vfield_list */
vfield_list rdup_vfield_list (vfield_list old)
{ vfield_list new;
  int ix;
  if (old == vfield_list_nil) return (old);
  new = init_vfield_list (old -> size);
  for (ix = 0; ix < old -> size; ix++)
    app_vfield_list (new, rdup_vfield (old -> array[ix]));
  return (new);
}

/* Recursively duplicate a type_list */
type_list rdup_type_list (type_list old)
{ type_list new;
  int ix;
  if (old == type_list_nil) return (old);
  new = init_type_list (old -> size);
  for (ix = 0; ix < old -> size; ix++)
    app_type_list (new, rdup_type (old -> array[ix]));
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

/* Recursively check the ref counts of a def */
void check_not_freed_def (def curr)
{ if (curr == def_nil) return;
  check_not_freed_string (curr -> lhs);
  switch (curr -> tag)
    { case TAGPrimitive:
	break;
      case TAGEnum:
	check_not_freed_string_list (curr -> Enum.elems);
	break;
      case TAGRecord:
	check_not_freed_field_list (curr -> Record.fixed);
	check_not_freed_vfield_list (curr -> Record.variant);
	break;
      default:
	dcg_bad_tag (curr -> tag, "check_not_freed_def");
    };
}

/* Recursively check the ref counts of a stat */
void check_not_freed_stat (stat curr)
{ if (curr == stat_nil) return;
  switch (curr -> tag)
    { case TAGUse:
	check_not_freed_type_list (curr -> Use.utype);
	break;
      case TAGImport:
	check_not_freed_string (curr -> Import.imp);
	break;
      default:
	dcg_bad_tag (curr -> tag, "check_not_freed_stat");
    };
}

/* Recursively check the ref counts of a field */
void check_not_freed_field (field curr)
{ if (curr == field_nil) return;
  check_not_freed_string (curr -> fname);
  check_not_freed_type (curr -> ftype);
  check_not_freed_int (curr -> ftrav);
}

/* Recursively check the ref counts of a vfield */
void check_not_freed_vfield (vfield curr)
{ if (curr == vfield_nil) return;
  check_not_freed_string (curr -> cons);
  check_not_freed_field_list (curr -> parts);
}

/* Recursively check the ref counts of a type */
void check_not_freed_type (type curr)
{ if (curr == type_nil) return;
  switch (curr -> tag)
    { case TAGTname:
	check_not_freed_string (curr -> Tname.tname);
	break;
      case TAGTlist:
	check_not_freed_type (curr -> Tlist.etyp);
	break;
      default:
	dcg_bad_tag (curr -> tag, "check_not_freed_type");
    };
}

/* Recursively check the refcounts of a def_list */
void check_not_freed_def_list (def_list curr)
{ int ix;
  if (curr == def_list_nil) return;
  for (ix = 0; ix < curr -> size; ix++)
    check_not_freed_def (curr -> array[ix]);
}

/* Recursively check the refcounts of a stat_list */
void check_not_freed_stat_list (stat_list curr)
{ int ix;
  if (curr == stat_list_nil) return;
  for (ix = 0; ix < curr -> size; ix++)
    check_not_freed_stat (curr -> array[ix]);
}

/* Recursively check the refcounts of a field_list */
void check_not_freed_field_list (field_list curr)
{ int ix;
  if (curr == field_list_nil) return;
  for (ix = 0; ix < curr -> size; ix++)
    check_not_freed_field (curr -> array[ix]);
}

/* Recursively check the refcounts of a vfield_list */
void check_not_freed_vfield_list (vfield_list curr)
{ int ix;
  if (curr == vfield_list_nil) return;
  for (ix = 0; ix < curr -> size; ix++)
    check_not_freed_vfield (curr -> array[ix]);
}

/* Recursively check the refcounts of a type_list */
void check_not_freed_type_list (type_list curr)
{ int ix;
  if (curr == type_list_nil) return;
  for (ix = 0; ix < curr -> size; ix++)
    check_not_freed_type (curr -> array[ix]);
}

/* Recursively check the refcounts of a string_list */
void check_not_freed_string_list (string_list curr)
{ int ix;
  if (curr == string_list_nil) return;
  for (ix = 0; ix < curr -> size; ix++)
    check_not_freed_string (curr -> array[ix]);
}

