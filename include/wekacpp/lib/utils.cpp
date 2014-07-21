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

#include <cmath>
#include <stdexcept>
#include "utils.h"

#define LOGPI	1.14472988584940017414
#define MAXLOG	7.09782712893383996732E2
#define PI		3.14159265358979323846
#define SQRTH	7.07106781186547524401E-1

namespace weka {

using namespace std;

static inline double
polevl (double x, double coef[], int N)
{
	double ans = coef[0];
	for (int i = 1; i <= N; i++)
		ans = ans * x + coef[i];
	return ans;
}

static inline double
p1evl (double x, double coef[], int N)
{
	double ans = x + coef[0];
	for (int i = 1; i < N; i++)
		ans = ans * x + coef[i];
	return ans;
}

double
lnFunc (double num)
{
	// constant hard coded for efficiency reasons
	if (num < 1e-6) {
		return 0;
    } else {
		return num * log(num);
    }
}

// returns natural logarithm of gamma function
double
lnGamma(double x)
{
	static double A[] = {
		8.11614167470508450300E-4,
		-5.95061904284301438324E-4,
		7.93650340457716943945E-4,
		-2.77777777730099687205E-3,
		8.33333333333331927722E-2
	};
	static double B[] = {
		-1.37825152569120859100E3,
		-3.88016315134637840924E4,
		-3.31612992738871184744E5,
		-1.16237097492762307383E6,
		-1.72173700820839662146E6,
		-8.53555664245765465627E5
	};
	static double C[] = {
		-3.51815701436523470549E2,
		-1.70642106651881159223E4,
		-2.20528590553854454839E5,
		-1.13933444367982507207E6,
		-2.53252307177582951285E6,
		-2.01889141433532773231E6
	};
    double p, q, w, z;
 
    if (x < -34.0) {
		q = -x;
		w = lnGamma(q);
		p = floor(q);
		if (p == q)
			throw new runtime_error("overflow in lngamma");
		z = q - p;
		if (z > 0.5) {
			p += 1.0;
			z = p - q;
		}
		z = q * sin(PI * z);
		if (z == 0.0)
			throw new runtime_error("overflow in lngamma");
		z = LOGPI - log(z) - w;
		return z;
    }
 
    if (x < 13.0) {
		z = 1.0;
		while (x >= 3.0) {
			x -= 1.0;
			z *= x;
		}
		while (x < 2.0) {
			if (x == 0.0)
				throw new runtime_error("overflow in lngamma"); 
			z /= x;
			x += 1.0;
		}
		if (z < 0.0)
			z = -z;
		if( x == 2.0 )
			return log(z);
		x -= 2.0;
		p = x * polevl(x, B, 5) / p1evl(x, C, 6);
		return log(z) + p;
    }
 
	if (x > 2.556348e305)
		throw new runtime_error("overflow in lngamma");
 
    q = (x - 0.5) * log(x) - x + 0.91893853320467274178;
  
    if (x > 1.0e8) 
		return q;
 
    p = 1.0 / (x * x);
    if( x >= 1000.0 )
		q += ((7.9365079365079365079365e-4 * p - 2.7777777777777777777778e-3) *
				p + 0.0833333333333333333333) / x;
    else
		q += polevl(p, A, 4) / x;

    return q;
}

double
errorFunction (double x)
{ 
    static double T[] = {
		9.60497373987051638749E0,
		9.00260197203842689217E1,
		2.23200534594684319226E3,
		7.00332514112805075473E3,
		5.55923013010394962768E4
    };
    static double U[] = {
		//1.00000000000000000000E0,
		3.35617141647503099647E1,
		5.21357949780152679795E2,
		4.59432382970980127987E3,
		2.26290000613890934246E4,
		4.92673942608635921086E4
    };
    double y, z;
	if (fabs(x) > 1.0)
		return 1.0 - errorFunctionComplemented(x);
    z = x * x;
    y = x * polevl(z, T, 4) / p1evl(z, U, 5);
    return y;
}

double
errorFunctionComplemented (double a)
{
    static double P[] = {
		2.46196981473530512524E-10,
		5.64189564831068821977E-1,
		7.46321056442269912687E0,
		4.86371970985681366614E1,
		1.96520832956077098242E2,
		5.26445194995477358631E2,
		9.34528527171957607540E2,
		1.02755188689515710272E3,
		5.57535335369399327526E2
    };
    static double Q[] = {
		//1.0
		1.32281951154744992508E1,
		8.67072140885989742329E1,
		3.54937778887819891062E2,
		9.75708501743205489753E2,
		1.82390916687909736289E3,
		2.24633760818710981792E3,
		1.65666309194161350182E3,
		5.57535340817727675546E2
    };
	static double R[] = {
		5.64189583547755073984E-1,
		1.27536670759978104416E0,
		5.01905042251180477414E0,
		6.16021097993053585195E0,
		7.40974269950448939160E0,
		2.97886665372100240670E0
    };
	static double S[] = {
		//1.00000000000000000000E0, 
		2.26052863220117276590E0,
		9.39603524938001434673E0,
		1.20489539808096656605E1,
		1.70814450747565897222E1,
		9.60896809063285878198E0,
		3.36907645100081516050E0
    };
	double x, y, z, p, q;
	if (a < 0.0)
		x = -a;
	else
		x = a;
  	if (x < 1.0)
		return 1.0 - errorFunction(a);
	z = -a * a;
	if (z < -MAXLOG) {
		if (a < 0)
			return 2.0;
		else
			return 0.0;
	}
	z = exp(z);
	if (x < 8.0) {
		p = polevl(x, P, 8);
		q = p1evl(x, Q, 8);
	} else {
		p = polevl(x, R, 5);
		q = p1evl(x, S, 6);
	}
	y = (z * p) / q;
	if (a < 0)
		y = 2.0 - y;
    if (y == 0.0) {
		if (a < 0)
			return 2.0;
		else
			return 0.0;
	}
	return y;
}

double
normalProbability (double a)
{ 
    double x, y, z;
	x = a * SQRTH;
    z = fabs(x);
	if (z < SQRTH) {
		y = 0.5 + 0.5 * errorFunction(x);
	} else {
		 y = 0.5 * errorFunctionComplemented(z);
		 if( x > 0 )
			 y = 1.0 - y;
	} 
	return y;
}

double
entropyForRow (vector<double>& row)
{
	double returnValue = 0, sum = 0;
	for (int i = 0; i < (int)row.size(); i++) {
		returnValue -= lnFunc(row[i]);
		sum += row[i];
	}
	if (sum == 0.0) {
		return 0;
	} else {
		return (returnValue + lnFunc(sum)) / (sum * log(2));
	}
}

double
entropyConditionedOnRows (vector<vector<double> >& rows)
{
	double returnValue = 0, sumForRow, total = 0;
	for (int i = 0; i < (int)rows.size(); i++) {
		sumForRow = 0;
		for (int j = 0; j < (int)rows[i].size(); j++) {
			returnValue = returnValue + lnFunc(rows[i][j]);
			sumForRow += rows[i][j];
		}
		returnValue = returnValue - lnFunc(sumForRow);
		total += sumForRow;
    }
	if (total == 0.0)
		return 0;
    return -returnValue / (total * log(2));
}

}

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
