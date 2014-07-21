/*
 * Derived from:
 *
 * DiscreteEstimator.java
 * Copyright (C) 1999 University of Waikato, Hamilton, New Zealand
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

#include <weka/estimators/Estimator.h>
#include <weka/estimators/DiscreteEstimator.h>
#include <cmath>
#include <stdexcept>

namespace weka {
	namespace estimators {

using namespace std;

DiscreteEstimator::DiscreteEstimator (int nSymbols, bool laplace)
{
	m_nSymbols = nSymbols;
	m_Counts = new double[nSymbols];
	m_SumOfCounts = 0;
	if (laplace) {
		for (int i = 0; i < nSymbols; i++)
			m_Counts[i] = 1.0;
		m_SumOfCounts = (double) nSymbols;
	}
}

DiscreteEstimator::DiscreteEstimator (int nSymbols, double fPrior)
{
	m_nSymbols = nSymbols;
	m_Counts = new double[nSymbols];
	for (int iSymbol = 0; iSymbol < nSymbols; iSymbol++)
		m_Counts[iSymbol] = fPrior;
	m_SumOfCounts = fPrior * (double) nSymbols;
}

DiscreteEstimator::~DiscreteEstimator()
{
	if (m_Counts != NULL)
		delete [] m_Counts;
}

} // namespace estimators
} // namespace weka

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:

