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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_NET_SEARCHALGORITHM__
#define __WEKACPP_CLASSIFIERS_BAYES_NET_SEARCHALGORITHM__

#include <weka/core/Instances.h>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace std;

class BayesNet;

class SearchAlgorithm 
{
 protected:
	int m_nMaxNrOfParents;
	bool m_bInitAsNaiveBayes;
	bool m_bMarkovBlanketClassifier;

	virtual bool addArcMakesSense(BayesNet * bayesNet, Instances * instances,
					int iAttributeHead, int iAttributeTail);
	virtual bool reverseArcMakesSense(BayesNet * bayesNet, 
					Instances * instances, int iAttributeHead,
					int iAttributeTail);
	virtual bool isArc(BayesNet * bayesNet, int iAttributeHead,
					int iAttributeTail);

 public:
	SearchAlgorithm(int maxNrOfParents = 2 /* tree augmented bayes */,
			bool initAsNaiveBayes = true, 
			bool markovBlanketClassifier = false) :
		m_nMaxNrOfParents(maxNrOfParents),
		m_bInitAsNaiveBayes(initAsNaiveBayes),
		m_bMarkovBlanketClassifier(markovBlanketClassifier)
	{
	}

	virtual ~SearchAlgorithm();

	virtual void buildStructure(BayesNet * bayesNet, Instances * instances);
	virtual void search(BayesNet * bayesNet, Instances * instances);
	virtual void doMarkovBlanketCorrection(BayesNet * bayesNet,
		Instances * instances);

	int
	getMaxNrOfParents (void)
	{
		return m_nMaxNrOfParents;
	}

	void
	setMaxNrOfParents (int parents)
	{
		m_nMaxNrOfParents = parents;
	}

	bool
	getInitAsNaiveBayes (void)
	{
		return m_bInitAsNaiveBayes;
	}

	void
	setInitAsNaiveBayes (bool val)
	{
		m_bInitAsNaiveBayes = val;
	}

	void
	setMarkovBlanketClassifier (bool bMarkovBlanketClassifier)
	{
        m_bMarkovBlanketClassifier = bMarkovBlanketClassifier;
    }

	bool
	getMarkovBlanketClassifier ()
	{
        return m_bMarkovBlanketClassifier;
    }

};

} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_NET_SEARCHALGORITHM__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
