/*
 * Derived from:
 *
 * HillClimber.java
 * Copyright (C) 2001 University of Waikato, Hamilton, New Zealand
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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_HILLCLIMBER__
#define __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_HILLCLIMBER__

#include <weka/core/Instances.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <weka/classifiers/bayes/net/local/LocalScoreSearchAlgorithm.h>
#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {
			namespace local {

using namespace std;

class HillClimber : public LocalScoreSearchAlgorithm 
{
 protected:

	struct Operation {
		static const int OPERATION_ADD = 1;
		static const int OPERATION_DEL = 2;
		static const int OPERATION_REVERSE = 3;

		double m_fDeltaScore;
		int m_nHead;
		int m_nTail;
		int m_nOperation;

		Operation() :
			m_fDeltaScore(-1e100),
			m_nHead(-1),
			m_nTail(-1),
			m_nOperation(-1)
		{
		}

		Operation(int nTail, int nHead, int nOperation) :
			m_fDeltaScore(-1e100)
		{
			m_nHead = nHead;
			m_nTail = nTail;
			m_nOperation = nOperation;
		}

		void
		operator=(Operation operation)
		{
			m_fDeltaScore = operation.m_fDeltaScore;
			m_nHead = operation.m_nHead;
			m_nTail = operation.m_nTail;
			m_nOperation = operation.m_nOperation;
		}

		bool
		operator==(Operation& operation)
		{
			if ((m_nHead == operation.m_nHead) &&
					(m_nTail == operation.m_nTail) &&
					(m_nOperation == operation.m_nOperation))
				return true;
			return false;
		}
	};

	struct Cache {
		int m_nNrOfNodes;
		double ** m_fDeltaScoreAdd;
		double ** m_fDeltaScoreDel;

		Cache(int nNrOfNodes)
		{
			m_nNrOfNodes = nNrOfNodes;
			m_fDeltaScoreAdd = new double*[nNrOfNodes];
			for (int i = 0; i < nNrOfNodes; i++) {
				m_fDeltaScoreAdd[i] = new double[nNrOfNodes];
				for (int j = 0; j < nNrOfNodes; j++)
					m_fDeltaScoreAdd[i][j] = 0.0;
			}
			m_fDeltaScoreDel = new double*[nNrOfNodes];
			for (int i = 0; i < nNrOfNodes; i++) {
				m_fDeltaScoreDel[i] = new double[nNrOfNodes];
				for (int j = 0; j < nNrOfNodes; j++)
					m_fDeltaScoreDel[i][j] = 0.0;
			}
		}

		~Cache()
		{
			for (int i = 0; i < m_nNrOfNodes; i++) {
				delete [] m_fDeltaScoreAdd[i];
				delete [] m_fDeltaScoreDel[i];
			}
			delete [] m_fDeltaScoreAdd;
			delete [] m_fDeltaScoreDel;
		}

		void
		put (Operation& oOperation, double fValue)
		{
			if (oOperation.m_nOperation == Operation::OPERATION_ADD)
				m_fDeltaScoreAdd[oOperation.m_nTail][oOperation.m_nHead] =
					fValue;
			else
				m_fDeltaScoreDel[oOperation.m_nTail][oOperation.m_nHead] =
					fValue;
		}

		double
		get (Operation& oOperation)
		{
			switch(oOperation.m_nOperation) {
			case Operation::OPERATION_ADD:
				return m_fDeltaScoreAdd[oOperation.m_nTail][oOperation.m_nHead];
			case Operation::OPERATION_DEL:
				return m_fDeltaScoreDel[oOperation.m_nTail][oOperation.m_nHead];
			case Operation::OPERATION_REVERSE:
				return m_fDeltaScoreDel[oOperation.m_nTail][oOperation.m_nHead] + m_fDeltaScoreAdd[oOperation.m_nHead][oOperation.m_nTail];
			}
			return 0;
		}
	};

 protected:
	Cache * m_Cache;
    bool m_bUseArcReversal;

	void initCache(BayesNet * bayesNet, Instances * instances);
	void updateCache(int iAttributeHead, int nNrOfAtts, ParentSet * set);
    Operation getOptimalOperation(BayesNet * net, Instances * instances,
		bool * done);
	void performOperation(BayesNet * bayesNet, Instances * instances,
		Operation& oOperation);
	Operation findBestArcToAdd(BayesNet * bayesNet, Instances * instances,
		Operation& oBestOperation);
	Operation findBestArcToDelete(BayesNet * bayesNet, Instances * instances,
		Operation& oBestOperation);
	Operation findBestArcToReverse(BayesNet * bayesNet, Instances * instances,
		Operation& oBestOperation);
 	virtual bool isNotTabu(Operation& oOperation);

	void
	applyArcAddition(BayesNet * bayesNet, int iHead, int iTail,
			Instances * instances)
	{
		ParentSet * bestParentSet = bayesNet->getParentSet(iHead);
		bestParentSet->addParent(iTail, instances);
		updateCache(iHead, instances->numAttributes(), bestParentSet);
	}

	void
	applyArcDeletion(BayesNet * bayesNet, int iHead, int iTail,
		Instances * instances)
	{
		ParentSet * bestParentSet = bayesNet->getParentSet(iHead);
		bestParentSet->deleteParent(iTail, instances);
		updateCache(iHead, instances->numAttributes(), bestParentSet);
	}

 public:
	HillClimber();
	HillClimber(BayesNet * bayesNet, Instances * instances);

	virtual ~HillClimber();

	bool
	getUseArcReversal (void)
	{
		return m_bUseArcReversal;
	}

	void
	setUseArcReversal (bool val)
	{
		m_bUseArcReversal = val;
	}

	virtual void search(BayesNet * bayesNet, Instances * instances);
};

} // namespace local
} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_HILLCLIMBER__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
