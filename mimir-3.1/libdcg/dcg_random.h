/*
   File: dcg_random.h
   Defines a random generator

   Copyright (C) 2008 Marc Seutter

   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library.  If not, see <http://www.gnu.org/licenses/>.

   CVS ID: "$Id: dcg_random.h,v 1.8 2011/08/17 19:14:44 marcs Exp $"
*/
#ifndef IncDcgRandom
#define IncDcgRandom

/* Exported functions */
int dcg_dice (int sides);
int dcg_random_in (int range);
int dcg_nat_gauss (int range);

double dcg_my_random ();
double dcg_my_gauss ();
void dcg_init_random ();

#endif /* IncDcgRandom */
