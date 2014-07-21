/*
 * Derived from:
 *
 * GlobalScoreSearchAlgorithm.java
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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_NET_GLOBAL_GLOBALSCORESEARCHALGORITHM__
#define __WEKACPP_CLASSIFIERS_BAYES_NET_GLOBAL_GLOBALSCORESEARCHALGORITHM__

#include <weka/core/Instances.h>
#include <weka/estimators/Estimator.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace weka::estimators;
using namespace std;

class GlobalScoreSearchAlgorithm : public SearchAlgorithm 
{
 public:
	/** constant for score type: LOO-CV */
	static const int LOOCV = 0;
	/** constant for score type: k-fold-CV */
	static const int KFOLDCV = 1;
	/** constant for score type: Cumulative-CV */
	static const int CUMCV = 2;

 private:
	BayesNet * m_BayesNet;
	bool m_bUseProb;
	int m_nNrOfFolds;
	int m_nCVType;

 protected:
	double accuracyIncrease(Instance * instance);
	double leaveOneOutCV(BayesNet * bayesNet);
	double cumulativeCV(BayesNet * bayesNet);
	double kFoldCV(BayesNet * bayesNet, int nNrOfFolds);

 public:
	GlobalScoreSearchAlgorithm() :
		m_BayesNet(NULL),
		m_bUseProb(true),
		m_nNrOfFolds(10),
		m_nCVType(LOOCV)
	{
	}

	GlobalScoreSearchAlgorithm(BayesNet * bayesNet, Instances * instances) :
		m_BayesNet(bayesNet),
		m_bUseProb(true),
		m_nNrOfFolds(10),
		m_nCVType(LOOCV)
	{
	}

	virtual ~GlobalScoreSearchAlgorithm()
	{
	}

	BayesNet *
	getBayesNet (void)
	{
		return m_BayesNet;
	}

	void
	setBayesNet (BayesNet * net)
	{
		m_BayesNet = net;
	}

	bool
	getUseProb (void)
	{
		return m_bUseProb;
	}

	void
	setUseProb (bool val)
	{
		m_bUseProb = val;
	}

	bool
	getCVType (void)
	{
		return m_nCVType;
	}

	void 
	setCVType (bool val)
	{
		m_nCVType = val;
	}

	double calcScore(BayesNet * bayesNet);
	double calcScoreWithExtraParent(int nNode, int nCandidateParent);
	double calcScoreWithMissingParent(int nNode, int nCandidateParent);
	double calcScoreWithReversedParent(int nNode, int nCandidateParent);
};

} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_NET_GLOBAL_GLOBALSCORESEARCHALGORITHM__*/

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
