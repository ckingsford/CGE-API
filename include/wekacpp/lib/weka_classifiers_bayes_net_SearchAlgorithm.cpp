/*
 * Derived from:
 *
 * SearchAlgorithm.java
 * Copyright (C) 2003 University of Waikato, Hamilton, New Zealand
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

#include <weka/core/Instance.h>
#include <weka/classifiers/bayes/BayesNet.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <stdexcept>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace std;

SearchAlgorithm::~SearchAlgorithm ()
{
}

bool
SearchAlgorithm::addArcMakesSense (BayesNet * bayesNet, Instances * instances,
	int head, int tail)
{
	if (head == tail)
		return false;

	// sanity check: arc should not be in parent set already
	if (isArc(bayesNet, head, tail))
		return false;

	// sanity check: arc should not introduce a cycle
	int nNodes = instances->numAttributes();
	bool bDone[nNodes];

	for (int i = 0; i < nNodes; i++)
		bDone[i] = false;

	// check for cycles
	bayesNet->getParentSet(head)->addParent(tail, instances);

	for (int i = 0; i < nNodes; i++) {
		// find a node for which all parents are 'done'
		bool bFound = false;
		for (int j = 0; !bFound && j < nNodes; j++) {
			if (!bDone[j]) {
				bool bHasNoParents = true;
				for (int k = 0;
					 k < bayesNet->getParentSet(j)->getNrOfParents();
					 k++)
				{
					if (!bDone[bayesNet->getParentSet(j)->getParent(k)]) {
						bHasNoParents = false;
					}
				}
				if (bHasNoParents) {
					bDone[j] = true;
					bFound = true;
				}
			}
		}
		if (!bFound) {
			bayesNet->getParentSet(head)->deleteLastParent(instances);
			return false;
		}
	}

	bayesNet->getParentSet(head)->deleteLastParent(instances);

	return true;
}

bool
SearchAlgorithm::reverseArcMakesSense (BayesNet * bayesNet,
	Instances * instances, int head, int tail)
{
	if (head == tail)
		return false;

	// sanity check: arc should be in parent set already
	if (!isArc(bayesNet, head, tail))
		return false;

	// sanity check: arc should not introduce a cycle
	int nNodes = instances->numAttributes();
	bool bDone[nNodes];

	for (int i = 0; i < nNodes; i++)
		bDone[i] = false;

	// check for cycles
	bayesNet->getParentSet(tail)->addParent(head, instances);

	for (int i = 0; i < nNodes; i++) {
		// find a node for which all parents are 'done'
		bool bFound = false;
		for (int j = 0; !bFound && j < nNodes; j++) {
			if (!bDone[j]) {
				ParentSet * parentSet = bayesNet->getParentSet(j);
				bool bHasNoParents = true;
				for (int k = 0; k < parentSet->getNrOfParents(); k++) {
					if (!bDone[parentSet->getParent(k)]) {
						// this one has a parent which is not 'done' UNLESS
						// it is the arc to be reversed
						if (!(j == head && parentSet->getParent(k) == tail)) {
							bHasNoParents = false;
						}
					}
				}
				if (bHasNoParents) {
					bDone[j] = true;
					bFound = true;
				}
			}
		}

		if (!bFound) {
			bayesNet->getParentSet(tail)->deleteLastParent(instances);
			return false;
		}
	}

	bayesNet->getParentSet(tail)->deleteLastParent(instances);

	return true;
}

bool
SearchAlgorithm::isArc (BayesNet * bayesNet, int head, int tail)
{
	for (int i = 0;
			 i < bayesNet->getParentSet(head)->getNrOfParents();
			 i++)
		if (bayesNet->getParentSet(head)->getParent(i) == tail)
			return true;
	return false;
}

void
SearchAlgorithm::buildStructure (BayesNet * bayesNet, Instances * instances)
{
	if (m_bInitAsNaiveBayes) {
		int i = instances->classIndex();
		// initialize parent sets to have arrow from classifier node to
		// each of the other nodes
		for (int j = 0; j < instances->numAttributes(); j++)
			if (j != i)
				bayesNet->getParentSet(j)->addParent(i, instances);
	}
	search(bayesNet, instances);
	if (m_bMarkovBlanketClassifier)
		doMarkovBlanketCorrection(bayesNet, instances);
}

void
SearchAlgorithm::search (BayesNet * bayesNet, Instances * instances)
{
}

void
SearchAlgorithm::doMarkovBlanketCorrection (BayesNet * bayesNet,
	Instances * instances)
{
	// Add class node as parent if it is not in the Markov Boundary
	int i = instances->classIndex();
	ParentSet * ancestors = new ParentSet();
	int nOldSize = 0;
	ancestors->addParent(i, instances);
	while (nOldSize != ancestors->getNrOfParents()) {
		nOldSize = ancestors->getNrOfParents();
		for (int j = 0; j < nOldSize; j++) {
			int iCurrent = ancestors->getParent(j);
			ParentSet * p = bayesNet->getParentSet(iCurrent);
			for (int k = 0; k < p->getNrOfParents(); k++) {
				if (!ancestors->contains(p->getParent(k)))
					ancestors->addParent(p->getParent(k), instances);
			}
		}
	}
	for (int j = 0; j < instances->numAttributes(); j++) {
		bool bIsInMarkovBoundary = (j == i) ||
				bayesNet->getParentSet(j)->contains(i) ||
				bayesNet->getParentSet(i)->contains(j);
		for (int k = 0; 
			 !bIsInMarkovBoundary && k < instances->numAttributes();
			 k++)
		{
			bIsInMarkovBoundary = bayesNet->getParentSet(k)->contains(j) &&
				bayesNet->getParentSet(k)->contains(i);
		}
		if (!bIsInMarkovBoundary) {
			if (ancestors->contains(j)) {
				if (bayesNet->getParentSet(i)->getCardinalityOfParents() 
						< 1024)
				{
					bayesNet->getParentSet(i)->addParent(j, instances);
				} else {
					// too bad
				}
			} else {
				bayesNet->getParentSet(j)->addParent(i, instances);
			}
		}
	}
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
