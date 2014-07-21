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

#include <weka/core/Attribute.h>
#include <weka/core/Instance.h>
#include <weka/core/Instances.h>
#include <weka/classifiers/Classifier.h>
#include <weka/classifiers/bayes/net/BayesNetEstimator.h>
#include <weka/classifiers/bayes/net/DiscreteEstimatorBayes.h>
#include <weka/classifiers/bayes/BayesNet.h>
#include <weka/classifiers/bayes/BayesNetGenerator.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace std;

void
BayesNetGenerator::init (int nNodes, int nValues)
{
	vector<Attribute *> attrs(nNodes);
	vector<string *> strs(nValues + 1);
	int iAttrs = 0;

	for (int iValue = 0; iValue < nValues; iValue++) {
		char buf[32];
		sprintf(buf, "Value%d", (iValue + 1));
		strs[iValue] = new string(buf);
	}
	for (int iNode = 0; iNode < nNodes; iNode++) {
		char name[32];
		sprintf(name, "Node%d", (iNode + 1));
		string s(name);
		Attribute * attr = new StringAttribute(s, strs);
		attrs[iAttrs++] = attr;
	}
	string s("RandomNet");
	m_Instances = new Instances(s, attrs, 100);
	m_Instances->setClassIndex(nNodes - 1);
	initStructure();
		
	// initialize conditional distribution tables
	m_Distributions.resize(nNodes);
	for (int iNode = 0; iNode < nNodes; iNode++) {
		m_Distributions[iNode].resize(1);
		m_Distributions[iNode][0] = 
			new DiscreteEstimatorBayes(nValues, getEstimator()->getAlpha());
	}
}	

void
BayesNetGenerator::generateTree(int nNodes)
{
	bool bConnected[nNodes];
	// start adding an arc at random
	int nNode1 = random() % nNodes;
	int nNode2 = random() % nNodes;
	if (nNode1 == nNode2)
		nNode2 = (nNode1 + 1) % nNodes;
	if (nNode2 < nNode1) {
		int h = nNode1;
		nNode1 = nNode2;
		nNode2 = h;
	}
	m_ParentSets[nNode2]->addParent(nNode1, m_Instances);
	bConnected[nNode1] = true;
	bConnected[nNode2] = true;
	// Repeatedly, select one of the connected nodes, and one of 
	// the unconnected nodes and add an arc.
	// All arcs point from lower to higher ordered nodes
	// so that acyclicity is ensured.
	for (int iArc = 2; iArc < nNodes; iArc++ ) {
		int nNode = random() % nNodes;
		nNode1 = 0; //  one of the connected nodes
		while (nNode >= 0) {
			nNode1 = (nNode1 + 1) % nNodes;
			while (!bConnected[nNode1]) {
				nNode1 = (nNode1 + 1) % nNodes;
			}
			nNode--;
		}
		nNode = random() % nNodes;
		nNode2 = 0; //  one of the unconnected nodes
		while (nNode >= 0) {
			nNode2 = (nNode2 + 1) % nNodes;
			while (bConnected[nNode2]) {
				nNode2 = (nNode2 + 1) % nNodes;
			}
			nNode--;
		}
		if (nNode2 < nNode1) {
			int h = nNode1;
			nNode1 = nNode2;
			nNode2 = h;
		}
		m_ParentSets[nNode2]->addParent(nNode1, m_Instances);
		bConnected[nNode1] = true;
		bConnected[nNode2] = true;
	}
}

vector<int>
BayesNetGenerator::getOrder ()
{
	int nNrOfAtts = m_Instances->numAttributes();
	vector<int> order(nNrOfAtts);
	bool bDone[nNrOfAtts];
	for (int iAtt = 0; iAtt < nNrOfAtts; iAtt++) {
	    int iAtt2 = 0; 
	    bool allParentsDone = false;
	    while (!allParentsDone && iAtt2 < nNrOfAtts) {
			if (!bDone[iAtt2]) {
				allParentsDone = true;
				int iParent = 0;
				while (allParentsDone && 
					   iParent < m_ParentSets[iAtt2]->getNrOfParents())
				{
					allParentsDone = 
						bDone[m_ParentSets[iAtt]->getParent(iParent++)];
				}
				if (allParentsDone && 
					iParent == m_ParentSets[iAtt2]->getNrOfParents())
				{
					order[iAtt] = iAtt2;
					bDone[iAtt2] = true;
				} else {
					iAtt2++;
				}
			} else {
				iAtt2++;
			}
	    }
	    if (!allParentsDone && iAtt2 == nNrOfAtts)
			throw new runtime_error("cycle detected in graph");
	}
	return order;
}

void 
BayesNetGenerator::generateRandomNetwork()
{
	// generate from scratch
	init(m_nNrOfNodes, m_nCardinality);
	generateRandomNetworkStructure(m_nNrOfNodes, m_nNrOfArcs);
	generateRandomDistributions(m_nNrOfNodes, m_nCardinality);
}

void
BayesNetGenerator::generateRandomNetworkStructure (int nNodes, int nArcs) 
{
	if (nArcs < nNodes - 1)
		throw new runtime_error("invalid number of arcs");

	if (nArcs > nNodes * (nNodes - 1) / 2)
		throw new runtime_error("invalid number of arcs");

	if (nArcs == 0)
		return; // deal with pathalogical case for nNodes = 1

	// first generate tree connecting all nodes
	generateTree(nNodes);

	// The tree contains nNodes - 1 arcs, so there are 
	// nArcs - (nNodes-1) to add at random.
	// All arcs point from lower to higher ordered nodes
	// so that acyclicity is ensured.
	for (int iArc = nNodes - 1; iArc < nArcs; iArc++) {
		bool bDone = false;
		while (!bDone) {
			int nNode1 = random() % nNodes;
			int nNode2 = random() % nNodes;
			if (nNode1 == nNode2) 
				nNode2 = (nNode1 + 1) % nNodes;
			if (nNode2 < nNode1) {
				int h = nNode1;
				nNode1 = nNode2;
				nNode2 = h;
			}
			if (!m_ParentSets[nNode2]->contains(nNode1)) {
				m_ParentSets[nNode2]->addParent(nNode1, m_Instances);
				bDone = true;
			}
		}
	}
}

void
BayesNetGenerator::generateRandomDistributions(int nNodes, int nValues)
{
	// Reserve space for CPTs
	int nMaxParentCardinality = 1;
	for (int iAttribute = 0; iAttribute < nNodes; iAttribute++)
		if (m_ParentSets[iAttribute]->getCardinalityOfParents() > 
				nMaxParentCardinality)
			nMaxParentCardinality = 
				m_ParentSets[iAttribute]->getCardinalityOfParents();

	// Reserve plenty of memory
	m_Distributions.resize(m_Instances->numAttributes());
	for (int i = 0; i < m_Instances->numAttributes(); i++)
		m_Distributions[i].resize(nMaxParentCardinality);

	// estimate CPTs
	for (int iAttribute = 0; iAttribute < nNodes; iAttribute++) {
		int nPs[nValues + 1];
		nPs[0] = 0;
		nPs[nValues] = 1000;
		for (int iParent = 0; 
			 iParent < m_ParentSets[iAttribute]->getCardinalityOfParents();
			 iParent++)
		{
			// fill array with random nr's
			for (int iValue = 1; iValue < nValues; iValue++)
				nPs[iValue] = random() % 1000;
			// sort
			for (int iValue = 1; iValue < nValues; iValue++)  {
				for (int iValue2 = iValue + 1; iValue2 < nValues; iValue2++)  {
					if (nPs[iValue2] < nPs[iValue]) {
						int h = nPs[iValue2];
						nPs[iValue2] = nPs[iValue];
						nPs[iValue] = h;
					}
				}
			}
			// assign to probability tables
			DiscreteEstimatorBayes * est = new DiscreteEstimatorBayes(nValues,
				getEstimator()->getAlpha());
			for (int iValue = 0; iValue < nValues; iValue++)
				est->addValue(iValue, nPs[iValue + 1] - nPs[iValue]);
			m_Distributions[iAttribute][iParent] = est;
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
