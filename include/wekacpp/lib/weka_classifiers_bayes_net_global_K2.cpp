/*
 * Derived from:
 *
 * K2.java
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

#include <weka/core/Debug.h>
#include <weka/core/Instance.h>
#include <weka/estimators/Estimator.h>
#include <weka/classifiers/bayes/BayesNet.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <weka/classifiers/bayes/net/global/GlobalScoreSearchAlgorithm.h>
#include <weka/classifiers/bayes/net/global/K2.h>
#include <cmath>
#include <stdexcept>
#include "utils.h"

namespace weka {
	namespace classifiers {
		namespace bayes {
			namespace global {

using namespace std;

K2::K2() :
	GlobalScoreSearchAlgorithm()
{
}

K2::K2(BayesNet * bayesNet, Instances * instances) :
	GlobalScoreSearchAlgorithm(bayesNet, instances)
{
}

K2::~K2()
{
}

void
K2::search (BayesNet * bayesNet, Instances * instances)
{	
	Debug.dprintf("%s: starting: %d attributes and %d classes\n",
		"K2::search [global]", instances->numAttributes(),
		instances->numClasses());

	int nNrOfAtt = instances->numAttributes();

	int nOrder[nNrOfAtt];
	int nAttribute = 0;
	nOrder[0] = instances->classIndex();

	for (int iOrder = 1; iOrder < instances->numAttributes(); iOrder++) {
		if (nAttribute == instances->classIndex())
		    nAttribute++;
		nOrder[iOrder] = nAttribute++;
	} 

	if (m_bRandomOrder) {
		Debug.dprintf("%s: reordering randomly\n", "K2::search [global]");
		int iClass;
		if (m_bInitAsNaiveBayes) {
			iClass = 0; 
		} else {
			iClass = -1;
		}
		for (int iOrder = 0; iOrder < instances->numAttributes(); iOrder++) {
			int iOrder2 = random() % instances->numAttributes();
			if (iOrder != iClass && iOrder2 != iClass) {
				int nTmp = nOrder[iOrder];
				nOrder[iOrder] = nOrder[iOrder2];
				nOrder[iOrder2] = nTmp;
			}
		}
	}

	// determine base scores
	double fBaseScore = calcScore(bayesNet);
	Debug.dprintf("%s: base score: %f\n", "K2::search [global]", fBaseScore);

	// K2 algorithm: greedy search restricted by ordering 
	for (int iOrder = 1; iOrder < instances->numAttributes(); iOrder++) {
		int iAttribute = nOrder[iOrder];
		double fBestScore = fBaseScore;
		bool bProgress = 
			(bayesNet->getParentSet(iAttribute)->getNrOfParents() < 
				m_nMaxNrOfParents);
		while (bProgress && 
			   (bayesNet->getParentSet(iAttribute)->getNrOfParents() <
					m_nMaxNrOfParents))
		{
			int nBestAttribute = -1;
			for (int iOrder2 = 0; iOrder2 < iOrder; iOrder2++) {
				int iAttribute2 = nOrder[iOrder2];
				double fScore = 
					calcScoreWithExtraParent(iAttribute, iAttribute2);
				if (fScore > fBestScore) {
					fBestScore = fScore;
					nBestAttribute = iAttribute2;
				}
			}
			if (nBestAttribute != -1) {
				bayesNet->getParentSet(iAttribute)
					-> addParent(nBestAttribute, instances);
				fBaseScore = fBestScore;
				bProgress = true;
				Debug.dprintf("  added link: %d -> %d\n", iAttribute,
					nBestAttribute);
			} else {
				bProgress = false;
			}
		}
		Debug.dprintf("%s: iOrder %d fBestScore %f\n", "K2::search [global]",
			iOrder, fBestScore);
	}

	Debug.dprintf("%s: done\n", "K2::search [global]");
}

} // namespace global
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
