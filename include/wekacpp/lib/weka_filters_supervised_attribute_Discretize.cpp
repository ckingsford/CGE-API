/*
 * Derived from:
 *
 * Filter.java
 * Copyright (C) 1999 University of Waikato, Hamilton, New Zealand
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
#include <weka/core/Instances.h>
#include <weka/filters/Filter.h>
#include <weka/filters/supervised/attribute/Discretize.h>
#include <stdexcept>

#include "utils.h"

namespace weka {
	namespace filters {

using namespace std;

static bool
FayyadAndIranisMDL (vector<double>& priorCounts, 
	vector<vector<double> >& bestCounts, int numInstances, int numCutPoints)
{
	double priorEntropy, entropy, gain; 
	double entropyLeft, entropyRight, delta;
	int numClassesTotal, numClassesRight, numClassesLeft;

	// compute entropy before split.

    priorEntropy = entropyForRow(priorCounts);

	// compute entropy after split.

	entropy = entropyConditionedOnRows(bestCounts);

	// compute information gain.

	gain = priorEntropy - entropy;

	// number of classes occuring in the set

	numClassesTotal = 0;
	for (int i = 0; i < (int)priorCounts.size(); i++)
		if (priorCounts[i] > 0)
			numClassesTotal++;

	// number of classes occuring in the left subset

	numClassesLeft = 0;
	for (int i = 0; i < (int)bestCounts[0].size(); i++)
		if (bestCounts[0][i] > 0)
			numClassesLeft++;

	// number of classes occuring in the right subset

    numClassesRight = 0;
    for (int i = 0; i < (int)bestCounts[1].size(); i++)
		if (bestCounts[1][i] > 0)
			numClassesRight++;

    // entropy of the left and the right subsets

    entropyLeft = entropyForRow(bestCounts[0]);
    entropyRight = entropyForRow(bestCounts[1]);

    // compute terms for MDL formula

    delta = log2(pow(3.0, numClassesTotal) - 2.0) - 
		(((double) numClassesTotal * priorEntropy) - 
			(numClassesRight * entropyRight) - 
			(numClassesLeft * entropyLeft));

    // check if split is to be accepted

    return (gain > (log2(numCutPoints) + delta) / (double) numInstances);
}

Discretize::Discretize() :
	Filter(),
	m_UseBetterEncoding(false)
{
}

Discretize::~Discretize()
{
}

vector<double>
Discretize::cutPointsForSubset (int attIndex, int first, int lastPlusOne)
{
	vector<double> v;
	double currentEntropy, currentCutPoint = -HUGE;
	int numInstances = 0, numCutPoints = 0;

    // compute class counts.

	vector<vector<double> > counts(2);
	for (int i = 0; i < 2; i++) {
		counts[i].resize(m_InputSet->numClasses());
		for (int j = 0; j < m_InputSet->numClasses(); j++)
			counts[i][j] = 0.0;
	}

    for (int i = first; i < lastPlusOne; i++) {
		numInstances += (int) m_InputSet->instance(i)->weight();
		counts[1][(int)m_InputSet->instance(i)->classValue()] +=
			m_InputSet->instance(i)->weight();
    }

	// save prior counts

    vector<double> priorCounts(m_InputSet->numClasses());
	for (int i = 0; i < m_InputSet->numClasses(); i++)
		priorCounts[i] = counts[1][i];

	// entropy of the full set

	double priorEntropy = entropyForRow(priorCounts);
	double bestEntropy = priorEntropy;

	// find best entropy

	vector<vector<double> > bestCounts(2);
	for (int i = 0; i < 2; i++) {
		bestCounts[i].resize(m_InputSet->numClasses());
		for (int j = 0; j < m_InputSet->numClasses(); j++)
			bestCounts[i][j] = 0.0;
	}

	double bestCutPoint = -1;
	int bestIndex = -1;

	for (int i = first; i < (lastPlusOne - 1); i++) {
		counts[0][(int)m_InputSet->instance(i)->classValue()] +=
			m_InputSet->instance(i)->weight();
		counts[1][(int)m_InputSet->instance(i)->classValue()] -=
			m_InputSet->instance(i)->weight();
		if (m_InputSet->instance(i)->value(attIndex) < 
			m_InputSet->instance(i + 1)->value(attIndex))
		{
			currentCutPoint =
				(m_InputSet->instance(i)->value(attIndex) + 
					m_InputSet->instance(i + 1)->value(attIndex)) / 2.0;
			currentEntropy = entropyConditionedOnRows(counts);
			if (currentEntropy < bestEntropy) {
				bestCutPoint = currentCutPoint;
				bestEntropy = currentEntropy;
				bestIndex = i;
				for (int j = 0; j < m_InputSet->numClasses(); j++) {
					bestCounts[0][j] = counts[0][j];
					bestCounts[1][j] = counts[1][j];
				}
			}
			numCutPoints++;
		}
	}

	// use worse encoding?

	if (!m_UseBetterEncoding)
		numCutPoints = (lastPlusOne - first) - 1;

	// check if gain is zero

	bool accept = true;
	double gain = priorEntropy - bestEntropy;
	if (gain <= 0)
		accept = false;

	// check if split is to be accepted

	if (accept)
		accept = FayyadAndIranisMDL(priorCounts, bestCounts, numInstances,
					(int)numCutPoints);

	// recurse

	if (accept) {
		// select split points for the left and right subsets

		vector<double> left = cutPointsForSubset(attIndex, first,
			bestIndex + 1);
		vector<double> right = cutPointsForSubset(attIndex, bestIndex + 1,
			lastPlusOne);

		// merge cutpoints

		if (!left.size() && !right.size()) {
			v.resize(1);
			v[0] = bestCutPoint;
		} else if (!right.size()) {
			v.resize(left.size() + 1);
			for (int i = 0; i < (int)left.size(); i++)
				v[i] = left[i];
			v[left.size()] = bestCutPoint;
		} else if (!left.size()) {
			v.resize(right.size() + 1);
			v[0] = bestCutPoint;
			for (int i = 0; i < (int)right.size(); i++)
				v[i+1] = right[i];
		} else {
			int i = 0;
			v.resize(left.size() + right.size() + 1);
			while (i < (int)left.size()) {
				v[i] = left[i];
				i++;
			}
			v[i] = bestCutPoint;
			i++;
			for (int j = 0; j < (int)right.size(); j++) {
				v[i] = right[j];
				i++;
			}
		}
	} else {
		// otherwise, do nothing
	}

	return v;
}

vector<double>
Discretize::cutPointsForAttribute (int attIndex)
{
	// sort input instances on this attribute
	m_InputSet->sort(attIndex);
	// find cut points for this attribute
	return cutPointsForSubset(attIndex, 0, m_InputSet->numInstances());
}

void
Discretize::calculateCutPoints (void)
{
	m_CutPoints.resize(m_Format->numAttributes());
	for (int i = 0; i < (int)m_Format->numAttributes(); i++) {
		// skip cases where we don't need to convert
		if (m_Format->attribute(i)->type() != Attribute::NOMINAL)
			continue;
		if (m_InputSet->attribute(i)->type() != Attribute::NUMERIC)
			continue;
		// find cut points for attribute
		m_CutPoints[i] = cutPointsForAttribute(i);
	}
}

void
Discretize::updateOutputFormat (void)
{
	Debug.dprintf("%s: updating ranges for nominal attributes for '%s':\n",
				  __FUNCTION__, m_Format->relationName().c_str());
	for (int i = 0; i < (int)m_Format->numAttributes(); i++) {
		if (m_Format->attribute(i)->type() != Attribute::NOMINAL)
			continue;
		if (m_InputSet->attribute(i)->type() != Attribute::NUMERIC)
			continue;
		NominalAttribute * attr = (NominalAttribute *) m_Format->attribute(i);
		string s;
		if (!m_CutPoints[i].size()) {
			s.append("'All'");
			attr->setValue(0, new string(s));
			Debug.dprintf("\t%s: %d, %d: %s\n", 
				m_Format->attribute(i)->name().c_str(), i, 0, s.c_str());
			s.clear();
		} else {
			for (int j = 0; j <= (int)m_CutPoints[i].size(); j++) {
				if (j == 0) {
					char buf[40];
					s.append("'(-inf-");
					sprintf(buf, "%lf", m_CutPoints[i][j]);
					s.append(buf);
					s.append("]'");
					attr->setValue(j, new string(s));
					Debug.dprintf("\t%s: %d, %d: %s\n",
						m_Format->attribute(i)->name().c_str(), i, j,
						s.c_str());
					s.clear();
				} else if (j == (int)m_CutPoints[i].size()) {
					char buf[40];
					s.append("'(");
					sprintf(buf, "%lf", m_CutPoints[i][j-1]);
					s.append(buf);
					s.append("-inf)'");
					attr->setValue(j, new string(s));
					Debug.dprintf("\t%s: %d, %d: %s\n",
						m_Format->attribute(i)->name().c_str(), i, j,
						s.c_str());
					s.clear();
				} else {
					char buf[40];
					s.append("'(");
					sprintf(buf, "%lf", m_CutPoints[i][j-1]);
					s.append(buf);
					s.append("-");
					sprintf(buf, "%lf", m_CutPoints[i][j]);
					s.append(buf);
					s.append("]'");
					attr->setValue(j, new string(s));
					Debug.dprintf("\t%s: %d, %d: %s\n",
						m_Format->attribute(i)->name().c_str(), i, j,
						s.c_str());
					s.clear();
				}
			}
		}
	}
}

Instance *
Discretize::convertInstance (Instance * instance)
{
	instance = new Instance(instance);
	instance->setDataset(m_Format);
	//Debug.dprintf("%s:", __FUNCTION__);
	for (int i = 0; i < (int)m_Format->numAttributes(); i++) {
		if (m_Format->attribute(i)->type() != Attribute::NOMINAL)
			continue;
		if (m_InputSet->attribute(i)->type() != Attribute::NUMERIC)
			continue;
		double val = instance->value(i);
		if (!m_CutPoints[i].size()) {
			instance->setValue(i, 0);
			//Debug.dprintf(" %lf->0", val);
		} else {
			int j;
			for (j = 0; j < (int)m_CutPoints[i].size(); j++)
				if (val <= m_CutPoints[i][j])
					break;
			instance->setValue(i, j);
			//Debug.dprintf(" %lf->%u", val, j);
	    }
	}
	//Debug.dprintf("\n");
	return instance;
}

bool
Discretize::batchFinished (void)
{
	if (m_InputSet == NULL)
		return false;
	// calculate cut points
	calculateCutPoints();
	// update ranges for nominal attributes
	updateOutputFormat();
	// convert instances
	for (int i = 0; i < m_InputSet->numInstances(); i++) {
		Instance * instance = m_InputSet->instance(i);
		m_OutputQueue.push_back(convertInstance(instance));
	}
	// call super
	Filter::batchFinished();
	return (numPendingOutput() != 0);
}

} // namespace filters
} // namespace weka

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:

