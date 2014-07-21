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

#include <weka/core/Instance.h>
#include <weka/estimators/Estimator.h>
#include <weka/classifiers/bayes/BayesNet.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <weka/classifiers/bayes/net/global/GlobalScoreSearchAlgorithm.h>
#include <cmath>
#include <stdexcept>
#include "utils.h"

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace weka::estimators;
using namespace std;

double
GlobalScoreSearchAlgorithm::accuracyIncrease (Instance * instance)
{
	if (m_bUseProb) {
		vector<double> fProb = m_BayesNet->distributionForInstance(instance);
		return fProb[(int)instance->classValue()] * instance->weight();
	} else {
		if (m_BayesNet->classifyInstance(instance) == instance->classValue())
			return instance->weight();
	}
	return 0;
}

double
GlobalScoreSearchAlgorithm::leaveOneOutCV (BayesNet * bayesNet)
{
	m_BayesNet = bayesNet;
	double fAccuracy = 0.0;
	double fWeight = 0.0;
	Instances * instances = bayesNet->getInstances();
	bayesNet->estimateCPTs();
	for (int iInstance = 0;
		 iInstance < instances->numInstances();
		 iInstance++)
	{
		Instance instance(instances->instance(iInstance));
		instance.setWeight(-instance.weight());
		bayesNet->updateClassifier(&instance);
		fAccuracy += accuracyIncrease(&instance);
		fWeight += instance.weight();
		instance.setWeight(-instance.weight());
		bayesNet->updateClassifier(&instance);
	}
	return fAccuracy / fWeight;
}

double
GlobalScoreSearchAlgorithm::cumulativeCV (BayesNet * bayesNet)
{
	m_BayesNet = bayesNet;
	double fAccuracy = 0.0;
	double fWeight = 0.0;
	Instances * instances = bayesNet->getInstances();
	bayesNet->initCPTs();
	for (int iInstance = 0; 
		 iInstance < instances->numInstances();
		 iInstance++)
	{
		Instance instance(instances->instance(iInstance));
		fAccuracy += accuracyIncrease(&instance);
		bayesNet->updateClassifier(&instance);
		fWeight += instance.weight();
	}
	return fAccuracy / fWeight;
}

double
GlobalScoreSearchAlgorithm::kFoldCV (BayesNet * bayesNet, int nNrOfFolds)
{
	m_BayesNet = bayesNet;
	double fAccuracy = 0.0;
	double fWeight = 0.0;
	Instances * instances = bayesNet->getInstances();

	// estimate CPTs based on complete data set
	bayesNet->estimateCPTs();

	int nFoldStart = 0;
	int nFoldEnd = instances->numInstances() / nNrOfFolds;
	int iFold = 1;
	while (nFoldStart < instances->numInstances()) {
		// remove influence of fold iFold from the probability distribution
		for (int iInstance = nFoldStart; iInstance < nFoldEnd; iInstance++) {
			Instance instance(instances->instance(iInstance));
			instance.setWeight(-instance.weight());
			bayesNet->updateClassifier(&instance);
		}
		// measure accuracy on fold iFold
		for (int iInstance = nFoldStart; iInstance < nFoldEnd; iInstance++) {
			Instance instance(instances->instance(iInstance));
			instance.setWeight(-instance.weight());
			fAccuracy += accuracyIncrease(&instance);
			instance.setWeight(-instance.weight());
			fWeight += instance.weight();
		}
		// restore influence of fold iFold from the probability distrib
		for (int iInstance = nFoldStart; iInstance < nFoldEnd; iInstance++) {
			Instance instance(instances->instance(iInstance));
			instance.setWeight(-instance.weight());
			bayesNet->updateClassifier(&instance);
		}
		// go to next fold
		nFoldStart = nFoldEnd;
		iFold++;
		nFoldEnd = iFold * instances->numInstances() / nNrOfFolds;
	}

	return fAccuracy / fWeight;
}

double
GlobalScoreSearchAlgorithm::calcScore (BayesNet * bayesNet)
{
	switch (m_nCVType) {
	case LOOCV: 
		return leaveOneOutCV(bayesNet);
	case CUMCV: 
		return cumulativeCV(bayesNet);
	case KFOLDCV: 
		return kFoldCV(bayesNet, m_nNrOfFolds);
	default:
		throw new runtime_error("unrecognized cross validation type");
	}
}

double
GlobalScoreSearchAlgorithm::calcScoreWithExtraParent (int nNode,
	int nCandidateParent)
{
	ParentSet * oParentSet = m_BayesNet->getParentSet(nNode);
	Instances * instances = m_BayesNet->getInstances();

	// sanity check: nCandidateParent should not be in parent set already
	for (int iParent = 0; iParent < oParentSet->getNrOfParents(); iParent++)
		if (oParentSet->getParent(iParent) == nCandidateParent)
			return -1e100;

	// set up candidate parent
	oParentSet->addParent(nCandidateParent, instances);

	// calculate the score
	double fAccuracy = calcScore(m_BayesNet);

	// delete temporarily added parent
	oParentSet->deleteLastParent(instances);

	return fAccuracy;
}

double
GlobalScoreSearchAlgorithm::calcScoreWithMissingParent (int nNode,
	int nCandidateParent)
{
	ParentSet * oParentSet = m_BayesNet->getParentSet(nNode);
	Instances * instances = m_BayesNet->getInstances();

	// sanity check: nCandidateParent should be in parent set already
	if (!oParentSet->contains(nCandidateParent))
		return -1e100;

	// set up candidate parent
	int iParent = oParentSet->deleteParent(nCandidateParent, instances);

	// calculate the score
	double fAccuracy = calcScore(m_BayesNet);

	// reinsert temporarily deleted parent
	oParentSet->addParent(nCandidateParent, iParent, instances);

	return fAccuracy;
}

double
GlobalScoreSearchAlgorithm::calcScoreWithReversedParent (int nNode,
	int nCandidateParent)
{
	ParentSet * oParentSet = m_BayesNet->getParentSet(nNode);
	ParentSet * oParentSet2 = m_BayesNet->getParentSet(nCandidateParent);
	Instances * instances = m_BayesNet->getInstances();

	// sanity check: nCandidateParent should be in parent set already
	if (!oParentSet->contains(nCandidateParent))
		return -1e100;

	// set up candidate parent
	int iParent = oParentSet->deleteParent(nCandidateParent, instances);
	oParentSet2->addParent(nNode, instances);

	// calculate the score
	double fAccuracy = calcScore(m_BayesNet);

	// restate temporarily reversed arrow
	oParentSet2->deleteLastParent(instances);
	oParentSet->addParent(nCandidateParent, iParent, instances);

	return fAccuracy;
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
