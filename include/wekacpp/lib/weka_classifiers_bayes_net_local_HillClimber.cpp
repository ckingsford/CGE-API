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
#include <stdexcept>

namespace weka {
	namespace classifiers {
		namespace bayes {
			namespace local {

using namespace std;

HillClimber::HillClimber() :
	LocalScoreSearchAlgorithm(),
	m_Cache(NULL),
	m_bUseArcReversal(true)
{
}

HillClimber::HillClimber(BayesNet * bayesNet, Instances * instances) :
	LocalScoreSearchAlgorithm(bayesNet, instances),
	m_Cache(NULL),
	m_bUseArcReversal(true)
{
}

HillClimber::~HillClimber ()
{
	if (m_Cache != NULL)
		delete m_Cache;
}

void
HillClimber::search (BayesNet * bayesNet, Instances * instances)
{
	Debug.dprintf("%s: starting: %d attributes %d classes\n", 
		"HillClimber::search [local]", instances->numAttributes(),
		instances->numClasses());

	initCache(bayesNet, instances);

	// go do the search
	bool done = false;
	Operation oOperation = getOptimalOperation(bayesNet, instances, &done);
	while (!done && (oOperation.m_fDeltaScore > 0)) {
		performOperation(bayesNet, instances, oOperation);
		oOperation = getOptimalOperation(bayesNet, instances, &done);
	}
	
	// free up memory
	delete m_Cache;

	Debug.dprintf("%s: done\n", "HillClimber::search [local]");
}

void
HillClimber::initCache(BayesNet * bayesNet, Instances * instances)
{
	int nNrOfAtts = instances->numAttributes();
	double fBaseScores[nNrOfAtts];

	m_Cache = new Cache(nNrOfAtts);
	if (m_Cache == NULL)
		throw new runtime_error("out of memory");

	for (int i = 0; i < nNrOfAtts; i++)
		updateCache(i, nNrOfAtts, bayesNet->getParentSet(i));

	for (int i = 0; i < nNrOfAtts; i++)
		fBaseScores[i] = calcNodeScore(i);

	for (int head = 0; head < nNrOfAtts; head++) {
		for (int tail = 0; tail < nNrOfAtts; tail++) {
			if (head != tail) {
				Operation oOperation(tail, head, Operation::OPERATION_ADD);
				m_Cache->put(oOperation, 
					calcScoreWithExtraParent(head, tail) - fBaseScores[head]);
			}
		}
	}
}

bool
HillClimber::isNotTabu (Operation& oOperation)
{
	return true;
}

HillClimber::Operation
HillClimber::getOptimalOperation (BayesNet * bayesNet, Instances * instances,
	bool * done)
{
	Operation oBestOperation;
	oBestOperation = findBestArcToAdd(bayesNet, instances, oBestOperation);
	oBestOperation = findBestArcToDelete(bayesNet, instances, oBestOperation);
	if (m_bUseArcReversal)
		oBestOperation = findBestArcToReverse(bayesNet, instances,
							oBestOperation);
	if (oBestOperation.m_fDeltaScore == -1E100)
		*done = true;
	return oBestOperation;
}

void
HillClimber::performOperation (BayesNet * bayesNet, Instances * instances,
	Operation& oOperation)
{
	switch (oOperation.m_nOperation) {
	case Operation::OPERATION_ADD:
		applyArcAddition(bayesNet, oOperation.m_nHead, oOperation.m_nTail,
			instances);
		Debug.dprintf("  added link: %d -> %d\n", oOperation.m_nHead,
			oOperation.m_nTail);
		break;
	case Operation::OPERATION_DEL:
		applyArcDeletion(bayesNet, oOperation.m_nHead, oOperation.m_nTail,
			instances);
		Debug.dprintf("  deleted link: %d -/> %d\n", oOperation.m_nHead,
			oOperation.m_nTail);
		break;
	case Operation::OPERATION_REVERSE:
		applyArcDeletion(bayesNet, oOperation.m_nHead, oOperation.m_nTail,
			instances);
		applyArcAddition(bayesNet, oOperation.m_nTail, oOperation.m_nHead,
			instances);
		Debug.dprintf("  reversed link: %d <- %d\n", oOperation.m_nHead,
			oOperation.m_nTail);
		break;
	}
}

HillClimber::Operation
HillClimber::findBestArcToAdd (BayesNet * bayesNet, Instances * instances,
	Operation& oBestOperation)
{
	int nNrOfAtts = instances->numAttributes();
	for (int head = 0; head < nNrOfAtts; head++) {
		if (bayesNet->getParentSet(head)->getNrOfParents() < m_nMaxNrOfParents)
		{
			for (int tail = 0; tail < nNrOfAtts; tail++) {
				if (addArcMakesSense(bayesNet, instances, head, tail)) {
					Operation oOperation(tail, head, Operation::OPERATION_ADD);
					if (m_Cache->get(oOperation) > 
							oBestOperation.m_fDeltaScore)
					{
						if (isNotTabu(oOperation)) {
							oBestOperation = oOperation;
							oBestOperation.m_fDeltaScore =
								m_Cache->get(oOperation);
						}
					}
				}
			}
		}
	}
	return oBestOperation;
}

HillClimber::Operation
HillClimber::findBestArcToDelete (BayesNet * bayesNet, Instances * instances,
	Operation& oBestOperation)
{
	int nNrOfAtts = instances->numAttributes();
	// find best arc to delete
	for (int iNode = 0; iNode < nNrOfAtts; iNode++) {
		ParentSet * parentSet = bayesNet->getParentSet(iNode);
		for (int iParent = 0; 
			 iParent < parentSet->getNrOfParents();
			 iParent++)
		{
			Operation oOperation(parentSet->getParent(iParent), iNode,
							Operation::OPERATION_DEL);
			if (m_Cache->get(oOperation) > oBestOperation.m_fDeltaScore) {
				if (isNotTabu(oOperation)) {
					oBestOperation = oOperation;
					oBestOperation.m_fDeltaScore = m_Cache->get(oOperation);
				}
			}
		}
	}
	return oBestOperation;
}

HillClimber::Operation
HillClimber::findBestArcToReverse (BayesNet * bayesNet, Instances * instances,
	Operation& oBestOperation)
{
	int nNrOfAtts = instances->numAttributes();
	for (int iNode = 0; iNode < nNrOfAtts; iNode++) {
		ParentSet * parentSet = bayesNet->getParentSet(iNode);
		for (int iParent = 0; iParent < parentSet->getNrOfParents(); iParent++)
		{
			int iTail = parentSet->getParent(iParent);
			// is reversal allowed?
			if (reverseArcMakesSense(bayesNet, instances, iNode, iTail) && 
					(bayesNet->getParentSet(iTail)->getNrOfParents() < 
						m_nMaxNrOfParents))
			{
				// go check if reversal results in the best step forward
				Operation oOperation(parentSet->getParent(iParent), iNode,
							Operation::OPERATION_REVERSE);
				if (m_Cache->get(oOperation) > oBestOperation.m_fDeltaScore) {
					if (isNotTabu(oOperation)) {
						oBestOperation = oOperation;
						oBestOperation.m_fDeltaScore =
							m_Cache->get(oOperation);
					}
				}
			}
		}
	}
	return oBestOperation;
}

void
HillClimber::updateCache(int iHead, int nNrOfAtts, ParentSet * parentSet)
{
	// update cache entries for arrows heading towards iHead
	double fBaseScore = calcNodeScore(iHead);
	int nNrOfParents = parentSet->getNrOfParents();
	for (int iTail = 0; iTail < nNrOfAtts; iTail++) {
		if (iTail != iHead) {
			if (!parentSet->contains(iTail)) {
				// add entries to cache for adding arcs
				if (nNrOfParents < m_nMaxNrOfParents) {
					Operation oOperation(iTail, iHead,
								Operation::OPERATION_ADD);
					m_Cache->put(oOperation,
						calcScoreWithExtraParent(iHead, iTail) - fBaseScore);
				}
			} else {
				// add entries to cache for deleting arcs
				Operation oOperation(iTail, iHead, Operation::OPERATION_DEL);
				m_Cache->put(oOperation,
					calcScoreWithMissingParent(iHead, iTail) - fBaseScore);
			}
		}
	}
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
