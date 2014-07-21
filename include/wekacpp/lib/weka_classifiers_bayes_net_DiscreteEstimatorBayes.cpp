/*
 * Derived from:
 *
 * BayesNetEstimator.java
 * Copyright (C) 2004 University of Waikato, Hamilton, New Zealand
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
#include <weka/classifiers/bayes/net/DiscreteEstimatorBayes.h>
#include <cmath>
#include "utils.h"

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace weka::estimators;
using namespace std;

DiscreteEstimatorBayes::DiscreteEstimatorBayes(int nSymbols, double fPrior)
{
	m_fPrior = fPrior;
	m_nSymbols = nSymbols;
	m_Counts.resize(m_nSymbols);
	for (int iSymbol = 0; iSymbol < m_nSymbols; iSymbol++)
		m_Counts[iSymbol] = m_fPrior;
	m_SumOfCounts = m_fPrior * (double) m_nSymbols;
}    

double
DiscreteEstimatorBayes::logScore(int nType)
{
	double fScore = 0.0;

	switch (nType) {

	case Estimator::BAYES: {
 		for (int iSymbol = 0; iSymbol < m_nSymbols; iSymbol++)
 			fScore += lnGamma(m_Counts[iSymbol]);
 		fScore -= lnGamma(m_SumOfCounts);
		if (m_fPrior != 0.0) {
 			fScore -= m_nSymbols * lnGamma(m_fPrior);
 			fScore += lnGamma(m_nSymbols * m_fPrior);
 		}
	} 	break;

	case Estimator::BDeu: {
 		for (int iSymbol = 0; iSymbol < m_nSymbols; iSymbol++)
 			fScore += lnGamma(m_Counts[iSymbol]);
 		fScore -= lnGamma(m_SumOfCounts);
 		fScore -= m_nSymbols * lnGamma(1.0);
 		fScore += lnGamma(m_nSymbols * 1.0);
	}	break;

	case Estimator::MDL:
	case Estimator::AIC:
    case Estimator::ENTROPY: {
 		for (int iSymbol = 0; iSymbol < m_nSymbols; iSymbol++) {
 			double fP = getProbability(iSymbol);
 			fScore += m_Counts[iSymbol] * log(fP);
 		} 
    } break;

    default:
		break;
    }

    return fScore;
}

} // namespace bayes
} // namespace classifiers
} // namespace weka
 
// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
