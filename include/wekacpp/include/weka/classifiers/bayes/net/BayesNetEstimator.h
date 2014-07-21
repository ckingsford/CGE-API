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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_NET_BAYESNETESTIMATOR__
#define __WEKACPP_CLASSIFIERS_BAYES_NET_BAYESNETESTIMATOR__

#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace std;

class BayesNet;

class BayesNetEstimator 
{
 protected:
	double m_Alpha;

 public:
	BayesNetEstimator():
		m_Alpha(0.5)
	{
	}

	virtual ~BayesNetEstimator();

	double
	getAlpha()
	{
		return m_Alpha;
	}

	void
	setAlpha (double alpha)
	{
		m_Alpha = alpha;
	}

	virtual void estimateCPTs(BayesNet * bayesNet);
	virtual void updateClassifier(BayesNet * bayesNet, Instance * instance);
	virtual vector<double> distributionForInstance(BayesNet * bayesNet,
		Instance * instance);
	virtual void initCPTs(BayesNet * bayesNet);
};

} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_NET_BAYESNETESTIMATOR__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
