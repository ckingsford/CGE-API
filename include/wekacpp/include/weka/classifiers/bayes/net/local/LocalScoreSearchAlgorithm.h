/*
 * Derived from:
 *
 * LocalScoreSearchAlgorithm.java
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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_LOCALSCORESEARCHALGORITHM__
#define __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_LOCALSCORESEARCHALGORITHM__

#include <weka/core/Instances.h>
#include <weka/estimators/Estimator.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace weka::estimators;
using namespace std;

class LocalScoreSearchAlgorithm : public SearchAlgorithm 
{
	BayesNet * m_BayesNet;
	double m_fAlpha;
	int m_nScoreType;

 protected:
	double calcScoreOfCounts(vector<int>& nCounts, int nCardinality,
				int numValues, Instances * instances);

 public:
	LocalScoreSearchAlgorithm() :
		m_fAlpha(0.5), m_nScoreType(Estimator::BAYES)
	{
	}

	LocalScoreSearchAlgorithm(BayesNet * bayesNet, Instances * instances) :
		m_BayesNet(bayesNet), m_fAlpha(0.5), m_nScoreType(Estimator::BAYES) 
	{
	}

	virtual ~LocalScoreSearchAlgorithm()
	{
	}
	
	int
	getScoreType(int nScoreType)
	{
		return m_nScoreType;
	}

	void
	setScoreType(int nScoreType)
	{
		m_nScoreType = nScoreType;
	}

	double logScore(int nType);
	virtual void buildStructure (BayesNet * bayesNet, Instances * instances);
	virtual double calcNodeScore(int nNode);
	virtual double calcScoreWithExtraParent(int nNode, int nCandidateParent);
	virtual double calcScoreWithMissingParent(int nNode, int nCandidateParent);
};

} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_LOCALSCORESEARCHALGORITHM__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
