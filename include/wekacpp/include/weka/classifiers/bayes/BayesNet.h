/*
 * Derived from:
 *
 * BayesNet.java
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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_BAYESNET__
#define __WEKACPP_CLASSIFIERS_BAYES_BAYESNET__

#include <weka/core/Instance.h>
#include <weka/core/Instances.h>
#include <weka/estimators/Estimator.h>
#include <weka/classifiers/Classifier.h>
#include <weka/classifiers/bayes/net/BayesNetEstimator.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <weka/classifiers/bayes/net/ParentSet.h>
#include <weka/classifiers/bayes/net/local/LocalScoreSearchAlgorithm.h>
#include <string>
#include <streambuf>
#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace weka::estimators;

class BayesNet : public Classifier
{
 protected:
	SearchAlgorithm * m_SearchAlgorithm;
	BayesNetEstimator * m_BayesNetEstimator;
	int m_NumClasses;
	vector<ParentSet *> m_ParentSets;
	vector<vector<Estimator *> > m_Distributions;
	Instances * m_Instances;

	Instances * normalizeDataSet(Instances * instances);
	Instance * normalizeInstance(Instance * instance);
	void buildStructure();

 public:
	void initStructure();
	void initCPTs();
	void estimateCPTs();

 public:
	static BayesNet * fromXMLBIF03(streambuf * s);
	static BayesNet * fromXMLBIF03(streambuf& s);
	static BayesNet * fromXMLBIF03(string& xml);

	BayesNet(string* name);
	BayesNet(string& name);
	~BayesNet();

	SearchAlgorithm *
	getSearchAlgorithm()
	{
		return m_SearchAlgorithm;
	}

	void
	setSearchAlgorithm(SearchAlgorithm * newSearchAlgorithm)
	{
		if (m_SearchAlgorithm != NULL)
			delete m_SearchAlgorithm;
		m_SearchAlgorithm = newSearchAlgorithm;
	}

	BayesNetEstimator *
	getEstimator()
	{
		return m_BayesNetEstimator;
    }

	void
	setEstimator(BayesNetEstimator * newBayesNetEstimator)
	{
		if (m_BayesNetEstimator != NULL)
			delete m_BayesNetEstimator;
        m_BayesNetEstimator = newBayesNetEstimator;
    }

	string
	getName()
	{
        return m_Instances->relationName();
    }

	int
	getNrOfNodes()
	{
        return m_Instances->numAttributes();
    }

	string
	getNodeName(int iNode)
	{
        return m_Instances->attribute(iNode)->name();
    }

	int
	getCardinality(int iNode)
	{
        return m_Instances->attribute(iNode)->numValues();
    }

	string
	getNodeValue(int iNode, int iValue)
	{
        return m_Instances->attribute(iNode)->value(iValue);
    }

	int
	getNrOfParents(int iNode)
	{
        return m_ParentSets[iNode]->getNrOfParents();
    }

	int
	getParent(int iNode, int iParent)
	{
        return m_ParentSets[iNode]->getParent(iParent);
    }

	vector<ParentSet *> *
	getParentSets() 
	{ 
		return &m_ParentSets;
	}

	vector<vector<Estimator *> > *
	getDistributions()
	{
		return &m_Distributions;
	}

	int
	getParentCardinality(int iNode)
	{
        return m_ParentSets[iNode]->getCardinalityOfParents();
    }

	double
	getProbability(int iNode, int iParent, int iValue)
	{
        return m_Distributions[iNode][iParent]->getProbability(iValue);
    }

	ParentSet *
	getParentSet(int iNode)
	{
        return m_ParentSets[iNode];
    }

	void
	setParentSet (int iNode, ParentSet * parentSet)
	{
		m_ParentSets.resize(iNode);
		if (m_ParentSets[iNode] != NULL)
			delete m_ParentSets[iNode];
		m_ParentSets[iNode] = parentSet;
	}

	Instances * 
	getInstances()
	{
		return m_Instances;
	}

	void
	setInstances(Instances * instances)
	{
		if (m_Instances != NULL)
			delete m_Instances;
		m_Instances = instances;
		m_NumClasses = instances->numClasses();
	}

	double
	measureBayesScore()
	{
		LocalScoreSearchAlgorithm s(this, m_Instances);
  	  	return s.logScore(Estimator::BAYES);
	}

	double
	measureBDeuScore()
	{
		LocalScoreSearchAlgorithm s(this, m_Instances);
  	  	return s.logScore(Estimator::BDeu);
	}

	double
	measureMDLScore()
	{
		LocalScoreSearchAlgorithm s(this, m_Instances);
  	  	return s.logScore(Estimator::MDL);
	}

	double
	measureAICScore()
	{
  	  	LocalScoreSearchAlgorithm s(this, m_Instances);
  	  	return s.logScore(Estimator::AIC);
	}

	double
	measureEntropyScore()
	{
  	  	LocalScoreSearchAlgorithm s(this, m_Instances);
  	  	return s.logScore(Estimator::ENTROPY);
	}

	void buildClassifier(Instances * instances);
	void updateClassifier (Instance * instance);
    vector<double> distributionForInstance(Instance * instance);
	vector<double> countsForInstance(Instance * instance);

	void toDOT(streambuf * s);
	void toDOT(streambuf& s);
	void toDOT(string& xml);

	void XMLBIF03DTD(streambuf * s);
	void toXMLBIF03(streambuf * s);
	void XMLBIF03DTD(streambuf& s);
	void toXMLBIF03(streambuf& s);
	void XMLBIF03DTD(string& xml);
	void toXMLBIF03(string& xml);
};

} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_BAYESNET__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
