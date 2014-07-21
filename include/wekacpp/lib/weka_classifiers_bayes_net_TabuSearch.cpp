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

#include <weka/core/Debug.h>
#include <weka/core/Instance.h>
#include <weka/classifiers/bayes/BayesNet.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <weka/classifiers/bayes/net/local/LocalScoreSearchAlgorithm.h>
#include <weka/classifiers/bayes/net/local/HillClimber.h>
#include <weka/classifiers/bayes/net/local/TabuSearch.h>
#include <stdexcept>

namespace weka {
	namespace classifiers {
		namespace bayes {
			namespace local {

using namespace std;

TabuSearch::TabuSearch(int nTabuList) :
	HillClimber(),
	m_nRuns(100),
	m_nTabuList(nTabuList),
	m_oTabuList(new Operation[nTabuList])
{
}

TabuSearch::TabuSearch(BayesNet * bayesNet, Instances * instances,
		int nTabuList) :
	HillClimber(bayesNet, instances),
	m_nRuns(100),
	m_nTabuList(nTabuList),
	m_oTabuList(new Operation[nTabuList])
{
}

TabuSearch::~TabuSearch ()
{
	delete [] m_oTabuList;
}

void
TabuSearch::search(BayesNet * bayesNet, Instances * instances)
{
	Debug.dprintf("%s: starting: %d attributes %d classes\n", 
		"TabuSearch::search [local]", instances->numAttributes(),
		instances->numClasses());

	int iCurrentTabuList = 0;
	m_oTabuList = new Operation[m_nTabuList];

	initCache(bayesNet, instances);

	double fBestScore;	
	double fCurrentScore = 0.0;
	for (int i = 0; i < instances->numAttributes(); i++)
		fCurrentScore += calcNodeScore(i);
	fBestScore = fCurrentScore;
	Debug.dprintf("%s: fBestScore %f\n", "TabuSearch::search [local]",
		fBestScore);

	string name("bestBayesNet");
	BayesNet * bestBayesNet = new BayesNet(name);
	bestBayesNet->setInstances(instances);
	bestBayesNet->initStructure();
	copyParentSets(bestBayesNet, bayesNet);
		
	// go do the search        
	for (int iRun = 0; iRun < m_nRuns; iRun++) {
		bool done = false;
		Operation oOperation = getOptimalOperation(bayesNet, instances, &done);
		// sanity check
		if (done)
			throw new runtime_error("could not find any step to make, "
							"tabu list too long?");
		performOperation(bayesNet, instances, oOperation);

		// update tabu list
		m_oTabuList[iCurrentTabuList] = oOperation;
		iCurrentTabuList = (iCurrentTabuList + 1) % m_nTabuList;

		// keep track of best network seen so far
		fCurrentScore += oOperation.m_fDeltaScore;
		if (fCurrentScore > fBestScore) {
			fBestScore = fCurrentScore;
			copyParentSets(bestBayesNet, bayesNet);
			Debug.dprintf("%s: run %d fBestScore %f\n",
				"TabuSearch::search [local]", iRun, fBestScore);
		}
	}
       
	copyParentSets(bayesNet, bestBayesNet);
	
	// free up memory
	delete bestBayesNet;
	delete m_Cache;

	Debug.dprintf("%s: done\n", "TabuSearch::search [local]");
}

} // namespace local
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
