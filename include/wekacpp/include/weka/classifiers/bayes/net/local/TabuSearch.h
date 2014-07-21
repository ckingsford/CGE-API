/*
 * Derived from:
 *
 * TabuSearch.java
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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_TABUSEARCH__
#define __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_TABUSEARCH__

#include <weka/core/Instances.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <weka/classifiers/bayes/net/local/LocalScoreSearchAlgorithm.h>
#include <weka/classifiers/bayes/net/local/HillClimber.h>
#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {
			namespace local {

using namespace std;

class TabuSearch : public HillClimber
{
    int m_nRuns;
	int m_nTabuList;
	Operation * m_oTabuList;

 protected:
	void
	copyParentSets(BayesNet * dest, BayesNet * source)
	{
		int nNodes = source->getNrOfNodes();
		for (int iNode = 0; iNode < nNodes; iNode++)
			dest->setParentSet(iNode,
					new ParentSet(source->getParentSet(iNode)));
	}

	bool
	isNotTabu (Operation& oOperation)
	{
		for (int iTabu = 0; iTabu < m_nTabuList; iTabu++)
			if (oOperation == m_oTabuList[iTabu])
				return false;
		return true;
	}

 public:
	TabuSearch(int nTabuList = 5);
	TabuSearch(BayesNet * bayesNet, Instances * instances, int nTabuList = 5);

	virtual ~TabuSearch();

	int
	getRuns (void)
	{
		return m_nRuns;
	}

	void
	setRuns (int val)
	{
		m_nRuns = val;
	}

	int
	getTabuListLen (void)
	{
		return m_nTabuList;
	}

	void
	setTabuListLen (int nTabu)
	{
		m_nTabuList = nTabu;
		delete [] m_oTabuList;
		m_oTabuList = new Operation[nTabu];
	}

	void search(BayesNet * bayesNet, Instances * instances);
};

} // namespace local
} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_TABUSEARCH__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
