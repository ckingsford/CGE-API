/*
 * Derived from:
 *
 * BayesNetGenerator.java
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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_BAYESNETGENERATOR__
#define __WEKACPP_CLASSIFIERS_BAYES_BAYESNETGENERATOR__

#include <weka/core/Instance.h>
#include <weka/core/Instances.h>
#include <weka/classifiers/bayes/net/BayesNetEstimator.h>
#include <weka/classifiers/bayes/net/ParentSet.h>
#include <weka/classifiers/bayes/BayesNet.h>
#include <string>
#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {

class BayesNetGenerator : public BayesNet
{
	int m_nNrOfNodes;
	int m_nCardinality;
	int m_nNrOfArcs;

	void init (int nNodes, int nValues);
	void generateTree (int nNodes);
    vector<int> getOrder();

  public:
	BayesNetGenerator(string& name, int nrOfNodes, int nCardinality, 
			int nrOfArcs) :
		BayesNet(name),
		m_nNrOfNodes(nrOfNodes),
		m_nCardinality(nCardinality),
		m_nNrOfArcs(nrOfArcs)
	{
	}
	~BayesNetGenerator();

	int
	getNrOfNodes()
	{
		return m_nNrOfNodes;
	}

	void
	setNrOfNodes(int nrOfNodes)
	{
		m_nNrOfNodes = nrOfNodes;
	}

	int
	getCardinality()
	{
		return m_nCardinality;
	}

	void
	setCardinality(int cardinality)
	{
		m_nCardinality = cardinality;
	}

	int
	getNrOfArcs()
	{
		return m_nNrOfArcs;
	}

	void
	setNrOfArcs (int nrOfArcs)
	{
		m_nNrOfArcs = nrOfArcs;
	}

	void generateRandomNetwork();
	void generateRandomNetworkStructure(int nNodes, int nArcs);
	void generateRandomDistributions(int nNodes, int nValues);
};

} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_BAYESNETGENERATOR__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
