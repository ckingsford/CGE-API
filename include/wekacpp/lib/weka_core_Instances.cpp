/*
 * Derived from:
 *
 * Instances.java
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

#include <weka/core/Instances.h>
#include <weka/core/Instance.h>
#include <stdexcept>
#include <vector>

namespace weka {

using namespace std;

int
Instances::partition (int attIndex, int l, int r)
{
    double pivot = instance((l + r) / 2)->value(attIndex);
    while (l < r) {
		while ((instance(l)->value(attIndex) < pivot) && (l < r))
			l++;
		while ((instance(r)->value(attIndex) > pivot) && (l < r))
			r--;
		if (l < r) {
			swap(l, r);
			l++;
			r--;
		}
    }
    if ((l == r) && (instance(r)->value(attIndex) > pivot))
		r--;
    return r;
}

void
Instances::quickSort (int attIndex, int left, int right)
{
    if (left < right) {
		int middle = partition(attIndex, left, right);
		quickSort(attIndex, left, middle);
		quickSort(attIndex, middle + 1, right);
	}
}

int
Instances::select (int attIndex, int left, int right, int k)
{
	if (left == right) {
		return left;
    } else {
		int middle = partition(attIndex, left, right);
		if ((middle - left + 1) >= k) {
			return select(attIndex, left, middle, k);
		} else {
			return select(attIndex, middle + 1, right,
							k - (middle - left + 1));
		}
    }
}

Instances::Instances (string& name, vector<Attribute *>& attrs,
		int capacity) :
	m_RelationName(name)
{
	m_Attributes.resize(attrs.size());
	for (int i = 0; i < (int)attrs.size(); i++)
		m_Attributes[i] = attrs[i]->clone();
}

Instances::Instances(Instances& other) :
	m_RelationName(other.relationName())
{
	m_ClassIndex = other.m_ClassIndex;
	m_Attributes.resize(other.m_Attributes.size());
	for (int i = 0; i < (int)other.m_Attributes.size(); i++)
		m_Attributes[i] = other.m_Attributes[i]->clone();
	m_Instances.resize(other.m_Instances.size());
	for (int i = 0; i < (int)other.m_Instances.size(); i++)
		m_Instances[i] = new Instance(other.m_Instances[i]);
}

Instances::Instances(Instances * other) :
	m_RelationName(other->relationName())
{
	m_ClassIndex = other->m_ClassIndex;
	m_Attributes.resize(other->m_Attributes.size());
	for (int i = 0; i < (int)other->m_Attributes.size(); i++)
		m_Attributes[i] = other->m_Attributes[i]->clone();
	m_Instances.resize(other->m_Instances.size());
	for (int i = 0; i < (int)other->m_Instances.size(); i++)
		m_Instances[i] = new Instance(other->m_Instances[i]);
}

Instances::~Instances()
{
	for (int i = 0; i < (int)m_Instances.size(); i++)
		if (m_Instances[i] != NULL)
			delete m_Instances[i];
	for (int i = 0; i < (int)m_Attributes.size(); i++)
		if (m_Attributes[i] != NULL)
			delete m_Attributes[i];
}

void
Instances::add(Instance * instance)
{
	instance->setDataset(this);
	m_Instances.push_back(instance);
}

void
Instances::add(Instance& instance)
{
	add(new Instance(instance));
}

vector<double>
Instances::attributeToDoubleArray(int index)
{
	vector <double> v;
    for (int i = 0; i < instance(i)->numValues(); i++)
		v[i] = instance(i)->value(index);
    return v;
}

bool
Instances::checkInstance (Instance& instance)
{
    if (instance.numAttributes() != numAttributes())
		return false;
	for (int i = 0; i < numAttributes(); i++) {
		if (instance.isMissing(i)) {
			continue;
		} else if (attribute(i)->isNominal() || attribute(i)->isString()) {
			if (instance.value(i) != (double)(int)instance.value(i)) {
				return false;
			} else if ((instance.value(i) < 0) ||
					   (instance.value(i) > attribute(i)->numValues())) {
				return false;
			}
		}
	}
	return true;
}

void
Instances::deleteAll ()
{
	vector<Instance *>::iterator i = m_Instances.begin();
	while (i != m_Instances.end()) {
		delete *i;
		i++;
	}
	m_Instances.clear();
}

void
Instances::deleteInstance (int index)
{
	vector<Instance *>::iterator i = m_Instances.begin();
	if (index == 0) {
		delete *i;
		m_Instances.erase(i);
		return;
	} else {
		while (index && (i != m_Instances.end())) {
			i++;
			if (--index == 0) {
				delete *i;
				m_Instances.erase(i);
				return;
			}
		}
	}
	throw new runtime_error("index not found");
}

void
Instances::deleteWithMissing (int attIndex)
{
	vector<Instance *>::iterator i = m_Instances.begin();
	while (i != m_Instances.end()) {
		if ((*i)->isMissing(attIndex)) {
			delete *i;
			i = m_Instances.erase(i);
		} else {
			i++;
		}
	}
}

double
Instances::kthSmallestValue (int attIndex, int k)
{
    if (!attribute(attIndex)->isNumeric())
		throw new runtime_error("attribute must be numeric to compute "
			"kth-smallest value");

    // move all instances with missing values to end
	int i = 0, j =  numInstances() - 1;
    while (i <= j) {
		if (instance(j)->isMissing(attIndex)) {
			j--;
		} else {
			if (instance(i)->isMissing(attIndex)) {
				swap(i,j);
				j--;
			}
			i++;
		}
	}

	// select instance
	if ((k < 0) || (k > j))
		throw new runtime_error("value for k for computing kth-smallest "
			"value too large");
    return instance(select(attIndex, 0, j, k))->value(attIndex);
}

double
Instances::meanOrMode (int attIndex)
{
    if (attribute(attIndex)->isNumeric()) {
		double result = 0, found = 0;
		for (int j = 0; j < numInstances(); j++) {
			if (!instance(j)->isMissing(attIndex)) {
				found += instance(j)->weight();
				result += instance(j)->weight()*instance(j)->value(attIndex);
			}
		}
		if (found <= 0) {
			return 0;
		} else {
			return result / found;
		}
    } else if (attribute(attIndex)->isNominal()) {
		vector<double> counts;
		for (int i = 0; i < numInstances(); i++) {
			if (!instance(i)->isMissing(attIndex)) {
				counts[(int)instance(i)->value(attIndex)] += 
					instance(i)->weight();
			}
		}
		int max = 0;
		for (int i = 0; i < (int)counts.size(); i++)
			if (counts[max] > counts[i])
				max = i;
		return (double) max;
    } else {
		return 0;
    }
}

// __vpart, __vsort, and vsort are quicksort

static inline int
__vpart (vector<double>& v, int top, int bottom)
{
	double x = v[top], temp;
	int i = top - 1;
	int j = bottom + 1;
	do {
		do {
			j--;
		} while (x > v[j]);
		do {
			i++;
		} while (x < v[i]);
		if (i < j) { 
			temp = v[i];    // switch elements at positions i and j
			v[i] = v[j];
			v[j] = temp;
		}
	} while (i < j);    
	return j;           // returns middle index
}

static void		// cannot inline recursive function
__vsort (vector<double>& v, int top, int bottom)
{
	int middle;
	if (top < bottom) {
		middle = __vpart(v, top, bottom);
		__vsort(v, top, middle);
		__vsort(v, middle+1, bottom);    // sort bottom partition
     }
     return;
}

static inline void
vsort (vector<double>& v)
{
	__vsort(v, 0, v.size());
}

int
Instances::numDistinctValues (int attIndex)
{
    if (attribute(attIndex)->isNumeric()) {
		vector<double> attVals = attributeToDoubleArray(attIndex);
		vsort(attVals);
		double prev = 0.0;
		int counter = 0;
		for (int i = 0; i < (int) attVals.size(); i++) {
			Instance * current = instance(i);
			if (current->isMissing(attIndex))
				break;
			if ((i == 0) || (current->value(attIndex) > prev)) {
				prev = current->value(attIndex);
				counter++;
			}
		}
		return counter;
    } else {
		return attribute(attIndex)->numValues();
    }
}

Instances *
Instances::resample()
{
    Instances * newData = new Instances(m_RelationName, m_Attributes, 0);
    while (newData->numInstances() < numInstances())
		newData->add(new Instance(instance(random() % numInstances())));
    return newData;
}

Instances *
Instances::resampleWithWeights ()
{
    vector<double> weights;
    for (int i = 0; i < numInstances(); i++)
		weights[i] = instance(i)->weight();
    return resampleWithWeights(weights);
}

Instances *
Instances::resampleWithWeights (vector<double>& weights)
{
    if ((int)weights.size() != numInstances())
		throw new runtime_error("weights.length != numInstances");
    Instances * newData = new Instances(m_RelationName, m_Attributes, 0);
    if (numInstances() == 0)
		return newData;
    vector<double> probabilities;
    double sumProbs = 0;
    for (int i = 0; i < numInstances(); i++) {
		sumProbs += (double) random();
		probabilities[i] = sumProbs;
    }
	// normalize
	double sumWeights = sumOfWeights();
	for (int i = 0; i < (int)probabilities.size(); i++)
		probabilities[i] /= sumProbs / sumWeights;
    // make sure that rounding errors don't mess things up
    probabilities[numInstances() - 1] = sumWeights;
    int k = 0; int l = 0;
    sumProbs = 0;
    while ((k < numInstances() && (l < numInstances()))) {
		if (weights[l] < 0)
			throw new runtime_error("weights must be positive");
		sumProbs += weights[l];
		while ((k < numInstances()) && (probabilities[k] <= sumProbs)) { 
			newData->add(new Instance(instance(l)));
			newData->instance(k)->setWeight(1);
			k++;
		}
		l++;
	}
	return newData;
}

void
Instances::sort(int attIndex)
{
	int i,j;
	// move all instances with missing values to end
	j = numInstances() - 1;
	i = 0;
	while (i <= j) {
		if (instance(j)->isMissing(attIndex)) {
			j--;
		} else {
			if (instance(i)->isMissing(attIndex)) {
				swap(i,j);
				j--;
			}
			i++;
		}
	}
	quickSort(attIndex, 0, j);
}

double
Instances::sumOfWeights ()
{
	double sum = 0;
	for (int i = 0; i < numInstances(); i++)
		sum += instance(i)->weight();
	return sum;
}

void
Instances::swap (int i, int j)
{
	Instance * temp = m_Instances[i];
	m_Instances[i] = m_Instances[j];
	m_Instances[j] = temp;
}

double
Instances::variance (int attIndex)
{
    if (!attribute(attIndex)->isNumeric())
		throw new runtime_error("attribute is not numeric");
    double sum = 0, sumSquared = 0, sumOfWeights = 0;
    for (int i = 0; i < numInstances(); i++) {
		if (!instance(i)->isMissing(attIndex)) {
			sum += instance(i)->weight() * instance(i)->value(attIndex);
			sumSquared += instance(i)->weight() * 
				instance(i)->value(attIndex) *
				instance(i)->value(attIndex);
			sumOfWeights += instance(i)->weight();
		}
    }
    if (sumOfWeights <= 1)
		return 0;
    double result = (sumSquared - (sum * sum / sumOfWeights)) / 
							(sumOfWeights - 1);
    // We don't like negative variance
    if (result < 0) {
		return 0;
    } else {
		return result;
    }
}

}

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
