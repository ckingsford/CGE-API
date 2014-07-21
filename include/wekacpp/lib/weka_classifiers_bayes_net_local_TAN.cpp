/*
 * Derived from:
 *
 * TAN.java
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
#include <weka/classifiers/bayes/net/local/TAN.h>
#include <stdexcept>

namespace weka {
	namespace classifiers {
		namespace bayes {
			namespace local {

using namespace std;

TAN::TAN ()
{
}

TAN::TAN(BayesNet * bayesNet, Instances * instances) :
	LocalScoreSearchAlgorithm(bayesNet, instances)
{
}

TAN::~TAN ()
{
}

void
TAN::buildStructure (BayesNet * bayesNet, Instances * instances)
{
	Debug.dprintf("%s: starting: %d attributes %d classes\n", 
		"TAN::buildStructure [local]", instances->numAttributes(),
		instances->numClasses());

	// init structure

	m_bInitAsNaiveBayes = true;
	m_nMaxNrOfParents = 2;
	LocalScoreSearchAlgorithm::buildStructure(bayesNet, instances);

	// determine base scores

	int nNrOfAtts = instances->numAttributes();
	double fBaseScores[nNrOfAtts];
	for (int i = 0; i < nNrOfAtts; i++)
		fBaseScores[i] = LocalScoreSearchAlgorithm::calcNodeScore(i);
	double fScore[nNrOfAtts][nNrOfAtts];
	for (int head = 0; head < nNrOfAtts; head++) {
		for (int tail = 0; tail < nNrOfAtts; tail++) {
			if (head != tail) {
				fScore[head][tail] = 
					LocalScoreSearchAlgorithm::calcScoreWithExtraParent(head,
						tail);
			}
		} 
	}
		
	// TAN greedy search (not restricted by ordering like K2)
	// 1. find strongest link
	// 2. find remaining links by adding strongest link to already
	//    connected nodes
	// 3. assign direction to links

	int nClassNode = instances->classIndex();

	int link1[nNrOfAtts];
	int link2[nNrOfAtts];
	bool linked[nNrOfAtts];
	for (int i = 0; i < nNrOfAtts; i++) {
		link1[i] = link2[i] = 0;
		linked[i] = false;
	}

	// 1. find strongest link

	int nBestLinkNode1 = -1;
	int nBestLinkNode2 = -1;
	double fBestDeltaScore = 0.0;
	int iLinkNode1;

	for (iLinkNode1 = 0; iLinkNode1 < nNrOfAtts; iLinkNode1++) {
		if (iLinkNode1 != nClassNode) {
			for (int iLinkNode2 = 0; iLinkNode2 < nNrOfAtts; iLinkNode2++) {
				if ((iLinkNode1 != iLinkNode2) &&
				    (iLinkNode2 != nClassNode) && 
					((nBestLinkNode1 == -1) || 
					 (fScore[iLinkNode1][iLinkNode2] - fBaseScores[iLinkNode1]
							> fBestDeltaScore)))
				{
					fBestDeltaScore = fScore[iLinkNode1][iLinkNode2] -
						fBaseScores[iLinkNode1];
					nBestLinkNode1 = iLinkNode2;
					nBestLinkNode2 = iLinkNode1;
			    } 
			} 
		}
	}
	link1[0] = nBestLinkNode1;
	link2[0] = nBestLinkNode2;
	linked[nBestLinkNode1] = true;
	linked[nBestLinkNode2] = true;

	Debug.dprintf("  strongest link: %d <-> %d (%f)\n", nBestLinkNode1,
		nBestLinkNode2, fBestDeltaScore);

	// 2. find remaining links by adding strongest link to already
	//    connected nodes

	for (int iLink = 1; iLink < nNrOfAtts - 2; iLink++) {
		nBestLinkNode1 = -1;
		for (iLinkNode1 = 0; iLinkNode1 < nNrOfAtts; iLinkNode1++) {
			if (iLinkNode1 != nClassNode) {
				for (int iLinkNode2 = 0; iLinkNode2 < nNrOfAtts; iLinkNode2++)
				{
					if ((iLinkNode1 != iLinkNode2) &&
					    (iLinkNode2 != nClassNode) && 
						(linked[iLinkNode1] || linked[iLinkNode2]) &&
						(!linked[iLinkNode1] || !linked[iLinkNode2]) &&
						((nBestLinkNode1 == -1) || 
						 (fScore[iLinkNode1][iLinkNode2] - 
						  fBaseScores[iLinkNode1] > fBestDeltaScore)))
					{
						fBestDeltaScore = fScore[iLinkNode1][iLinkNode2] -
							fBaseScores[iLinkNode1];
						nBestLinkNode1 = iLinkNode2;
						nBestLinkNode2 = iLinkNode1;
					} 
				} 
			}
		}
		link1[iLink] = nBestLinkNode1;
		link2[iLink] = nBestLinkNode2;
		linked[nBestLinkNode1] = true;
		linked[nBestLinkNode2] = true;

		Debug.dprintf("  link: %d <-> %d (%f)\n", nBestLinkNode1,
			nBestLinkNode2, fBestDeltaScore);
	}
		
	// 3. assign direction to links

	bool hasParent[nNrOfAtts];
	for (int i = 0; i < nNrOfAtts; i++)
		hasParent[i] = false;

	for (int iLink = 0; iLink < nNrOfAtts - 2; iLink++) {
		if (!hasParent[link1[iLink]]) {
			bayesNet->getParentSet(link1[iLink])
				-> addParent(link2[iLink], instances);
			hasParent[link1[iLink]] = true;
			Debug.dprintf("  added link: %d -> %d\n", link1[iLink],
				link2[iLink]);
		} else {
			if (hasParent[link2[iLink]]) {
				throw new runtime_error("too many links");
			}
			bayesNet->getParentSet(link2[iLink])
				-> addParent(link1[iLink], instances);
			hasParent[link2[iLink]] = true;
			Debug.dprintf("  added link: %d -> %d\n", link2[iLink],
				link1[iLink]);
		}
	}

	Debug.dprintf("%s: done\n", "TAN::buildStructure [local]");
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
