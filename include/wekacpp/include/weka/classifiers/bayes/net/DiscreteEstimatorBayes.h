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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_NET_DISCRETEESTIMATORBAYES__
#define __WEKACPP_CLASSIFIERS_BAYES_NET_DISCRETEESTIMATORBAYES__

#include <weka/estimators/Estimator.h>
#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace weka::estimators;
using namespace std;

class DiscreteEstimatorBayes : public Estimator
{
	vector<double> m_Counts;
	double m_SumOfCounts;
	int m_nSymbols;
	double m_fPrior;

 public:
	DiscreteEstimatorBayes(int nSymbols, double fPrior);

	void
	addValue(double data, double weight)
	{
		m_Counts[(int)data] += weight;
		m_SumOfCounts += weight;
	}

	double
	getProbability(double data)
	{
		if (m_SumOfCounts == 0) {
			// this can only happen if numSymbols = 0 in constructor
			return 0;
		} 
		return (double) m_Counts[(int) data] / m_SumOfCounts;
	}

	double
	getCount(double data)
	{
		if (m_SumOfCounts == 0) {
			// this can only happen if numSymbols = 0 in constructor
			return 0;
		} 
		return m_Counts[(int)data];
	} 

	int
	getNumSymbols()
	{
		return m_Counts.size();
	}

	double logScore(int nType);
};

} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_NET_DISCRETEESTMATORBAYES__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
