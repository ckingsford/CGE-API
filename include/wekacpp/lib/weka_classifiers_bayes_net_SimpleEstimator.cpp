/*
 * Derived from:
 *
 * SimpleEstimator.java
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
#include <weka/classifiers/bayes/BayesNet.h>
#include <weka/classifiers/bayes/net/BayesNetEstimator.h>
#include <weka/classifiers/bayes/net/DiscreteEstimatorBayes.h>
#include <weka/classifiers/bayes/net/SimpleEstimator.h>
#include <cmath>
#include <stdexcept>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace std;

void
SimpleEstimator::estimateCPTs (BayesNet * bayesNet)
{
	initCPTs(bayesNet);
	Instances * instances = bayesNet->getInstances();
	for (int i = 0; i < instances->numInstances(); i++)
		updateClassifier(bayesNet, instances->instance(i));
}

void
SimpleEstimator::updateClassifier (BayesNet * bayesNet, Instance * instance)
{
	Instances * instances = bayesNet->getInstances();
	for (int i = 0; i < instances->numAttributes(); i++) {
		double iCPT = 0;
		for (int j = 0; j < bayesNet->getParentSet(i)->getNrOfParents(); j++) {
			int nParent;
			nParent = bayesNet->getParentSet(i)->getParent(j);
			iCPT = iCPT * instances->attribute(nParent)->numValues() +
						instance->value(nParent);
		}
		vector<vector<Estimator *> > * distributions = 
			bayesNet->getDistributions();
		(*distributions)[i][(int)iCPT]->addValue(instance->value(i),
			instance->weight());
	}
}

vector<double>
SimpleEstimator::distributionForInstance (BayesNet * bayesNet,
	Instance * instance)
{
	Instances * instances = bayesNet->getInstances();
	vector<vector<Estimator *> > * distributions =
		bayesNet->getDistributions();
	int nNumClasses = instances->numClasses();
	vector<double> fProbs(nNumClasses);
	for (int i = 0; i < nNumClasses; i++)
		fProbs[i] = 1.0;
	for (int i = 0; i < nNumClasses; i++) {
		double logfP = 0;
		for (int j = 0; j < instances->numAttributes(); j++) {
			double iCPT = 0;
			for (int k = 0;
				k < bayesNet->getParentSet(j)->getNrOfParents();
				k++)
			{
				int nParent = bayesNet->getParentSet(j)->getParent(k);
				if (nParent == instances->classIndex()) {
					iCPT = iCPT * nNumClasses + i;
				} else {
					iCPT = iCPT * instances->attribute(nParent)->numValues() +
						instance->value(nParent);
				}
			}
			if (j == instances->classIndex()) {
				int k = (int)iCPT;
				logfP += log((*distributions)[j][k]->getProbability(i));
			} else {
				int k = (int)iCPT;
				logfP += log((*distributions)[j][k]->
								getProbability(instance->value(j)));
			}
		}
		fProbs[i] += logfP;
	}
	// find maximum
	double fMax = fProbs[0];
	for (int i = 0; i < nNumClasses; i++)
		if (fProbs[i] > fMax)
			fMax = fProbs[i];
	// transform from log-space to normal-space
	for (int i = 0; i < nNumClasses; i++)
		fProbs[i] = exp(fProbs[i] - fMax);
	// normalize
	double fSum = 0.0;
	for (int i = 0; i < nNumClasses; i++)
		fSum += fProbs[i];
    for (int i = 0; i < nNumClasses; i++)
		fProbs[i] /= fSum;
	return fProbs;
}

void
SimpleEstimator::initCPTs (BayesNet * bayesNet)
{
	Instances * instances = bayesNet->getInstances();
	// reserve space for CPTs
	int nMaxParentCardinality = 1;
	for (int i = 0; i < instances->numAttributes(); i++) {
		if (bayesNet->getParentSet(i)->getCardinalityOfParents() >
				nMaxParentCardinality)
			nMaxParentCardinality =
				bayesNet->getParentSet(i)->getCardinalityOfParents();
	}
	vector<vector<Estimator *> > * distributions =
		bayesNet->getDistributions();
	// reserve memory
	distributions->resize(instances->numAttributes());
	for (int i = 0; i < instances->numAttributes(); i++)
		(*distributions)[i].resize(nMaxParentCardinality);
	// estimate CPTs
	for (int i = 0; i < instances->numAttributes(); i++) {
		for (int j = 0; 
			 j < bayesNet->getParentSet(i)->getCardinalityOfParents(); 
			 j++)
		{
			(*distributions)[i][j] = new DiscreteEstimatorBayes(instances->attribute(i)->numValues(), m_Alpha);
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
