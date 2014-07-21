/*
 * Derived from:
 *
 * LocalScoreSearchAlgorithm.java
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
#include <weka/classifiers/bayes/net/local/LocalScoreSearchAlgorithm.h>
#include <cmath>
#include <stdexcept>
#include "utils.h"

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace weka::estimators;
using namespace std;

double
LocalScoreSearchAlgorithm::calcScoreOfCounts (vector<int>& nCounts,
	int nCardinality, int numValues, Instances * instances)
{
	// calculate scores using the distributions
	double fLogScore = 0.0;
	for (int iParent = 0; iParent < nCardinality; iParent++) {
		switch (m_nScoreType) {
		case Estimator::BAYES: {
			double nSumOfCounts = 0;
			for (int iSymbol = 0; iSymbol < numValues; iSymbol++) {
				if (m_fAlpha + nCounts[iParent * numValues + iSymbol] != 0) {
					fLogScore += 
						lnGamma(m_fAlpha + nCounts[iParent*numValues+iSymbol]);
					nSumOfCounts += m_fAlpha +
						nCounts[iParent * numValues + iSymbol];
				}
			}
			if (nSumOfCounts != 0)
				fLogScore -= lnGamma(nSumOfCounts);
			if (m_fAlpha != 0) {
				fLogScore -= numValues * lnGamma(m_fAlpha);
				fLogScore += lnGamma(numValues * m_fAlpha);
			}
		}	break;
		case Estimator::BDeu: {
			double nSumOfCounts = 0;
			for (int iSymbol = 0; iSymbol < numValues; iSymbol++) {
				if (m_fAlpha + nCounts[iParent * numValues + iSymbol] != 0) {
					fLogScore +=
						lnGamma(1.0/(numValues * nCardinality) +
								nCounts[iParent * numValues + iSymbol]);
					nSumOfCounts += 1.0/(numValues * nCardinality) + 
						nCounts[iParent * numValues + iSymbol];
				}
			}
			fLogScore -= lnGamma(nSumOfCounts);
			fLogScore -= numValues * lnGamma(1.0);
			fLogScore += lnGamma(numValues * 1.0);
		}	break;
		case Estimator::MDL:
		case Estimator::AIC:
		case Estimator::ENTROPY: {
			double nSumOfCounts = 0;
			for (int iSymbol = 0; iSymbol < numValues; iSymbol++)
				nSumOfCounts += nCounts[iParent * numValues + iSymbol];
			for (int iSymbol = 0; iSymbol < numValues; iSymbol++) {
				if (nCounts[iParent * numValues + iSymbol] > 0) {
					fLogScore += nCounts[iParent * numValues + iSymbol] *
						log(nCounts[iParent * numValues + iSymbol] /
							nSumOfCounts);
				}
			}
		}	break;

		default:
			break;
		}
	}
	switch (m_nScoreType) {
	case Estimator::MDL: {
		fLogScore -= 0.5 * nCardinality * (numValues - 1) * 
			log(instances->numInstances());
		// it seems safe to assume that numInstances>0 here
	}	break;
	case Estimator::AIC:
		fLogScore -= nCardinality * (numValues - 1);
		break;
	}
	return fLogScore;
}

double
LocalScoreSearchAlgorithm::logScore (int nType)
{
	if (nType < 0)
		nType = m_nScoreType;
	double fLogScore = 0.0;
	Instances * instances = m_BayesNet->getInstances();
	vector<vector<Estimator *> > * distributions = 
		m_BayesNet->getDistributions();
	for (int iAttribute = 0;
		 iAttribute < instances->numAttributes();
		 iAttribute++)
	{
		for (int iParent = 0;
			 iParent < m_BayesNet->getParentSet(iAttribute)->getCardinalityOfParents(); iParent++) 
		{
			fLogScore += (*distributions)[iAttribute][iParent]->logScore(nType);
            switch (nType) {
			case Estimator::MDL:
				fLogScore -= 0.5 * m_BayesNet->getParentSet(iAttribute)->getCardinalityOfParents() *  (instances->attribute(iAttribute)->numValues() - 1) * log(instances->numInstances());
				break;
			case Estimator::AIC:
				fLogScore -= m_BayesNet->getParentSet(iAttribute)->getCardinalityOfParents() * (instances->attribute(iAttribute)->numValues() - 1);
				break;
            }
		}
	}
	return fLogScore;
}

void
LocalScoreSearchAlgorithm::buildStructure (BayesNet * bayesNet,
	Instances * instances)
{
	m_BayesNet = bayesNet;
	SearchAlgorithm::buildStructure(bayesNet, instances);
}

double
LocalScoreSearchAlgorithm::calcNodeScore (int nNode)
{
	Instances * instances = m_BayesNet->getInstances();
	ParentSet * oParentSet = m_BayesNet->getParentSet(nNode);
	int nCardinality = oParentSet->getCardinalityOfParents();
	int numValues = instances->attribute(nNode)->numValues();
	vector<int> nCounts(nCardinality * numValues);
	for (int iParent = 0; iParent < nCardinality * numValues; iParent++)
		nCounts[iParent] = 0;
	for (int iInstance = 0;
		 iInstance < instances->numInstances();
		 iInstance++)
	{
		Instance * instance = instances->instance(iInstance);
		double iCPT = 0;
		for (int iParent = 0; 
			 iParent < oParentSet->getNrOfParents();
			 iParent++)
		{
			int nParent = oParentSet->getParent(iParent);
			iCPT = iCPT * instances->attribute(nParent)->numValues() +
				instance->value(nParent);
		}
		nCounts[numValues * ((int)iCPT) + (int)instance->value(nNode)]++;
	}
	return calcScoreOfCounts(nCounts, nCardinality, numValues, instances);
}

double
LocalScoreSearchAlgorithm::calcScoreWithExtraParent (int nNode,
	int nCandidateParent)
{
	ParentSet * oParentSet = m_BayesNet->getParentSet(nNode);
	// sanity check: nCandidateParent should not be in parent set already
	if (oParentSet->contains(nCandidateParent))
		return -1e100;
	// set up candidate parent
	oParentSet->addParent(nCandidateParent, m_BayesNet->getInstances());
	// calculate the score
	double logScore = calcNodeScore(nNode);
	// delete temporarily added parent
	oParentSet->deleteLastParent(m_BayesNet->getInstances());
	return logScore;
}

double
LocalScoreSearchAlgorithm::calcScoreWithMissingParent (int nNode,
	int nCandidateParent)
{
	ParentSet * oParentSet = m_BayesNet->getParentSet(nNode);
	// sanity check: nCandidateParent should be in parent set already
	if (!oParentSet->contains(nCandidateParent))
		return -1e100;
	// set up candidate parent
	int iParent =
		oParentSet->deleteParent(nCandidateParent, m_BayesNet->getInstances());
	// calculate the score
	double logScore = calcNodeScore(nNode);
	// restore temporarily deleted parent
	oParentSet->addParent(nCandidateParent, iParent, 
					m_BayesNet->getInstances());
	return logScore;
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
