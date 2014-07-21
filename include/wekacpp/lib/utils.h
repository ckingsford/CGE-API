/*
 * Derived from:
 *
 * Utils.java
 * Copyright (C) 1999-2004 University of Waikato, Hamilton, New Zealand
 *
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <vector>

namespace weka {

using namespace std;

static inline
double log2 (double val)
{
	return log(val) / log(2);
}

double lnFunc(double x);
double lnGamma(double x);
double errorFunction (double x);
double errorFunctionComplemented (double a);
double normalProbability (double a);

double entropyForRow (vector<double>& row);
double entropyConditionedOnRows (vector<vector<double> >& rows);

}

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
