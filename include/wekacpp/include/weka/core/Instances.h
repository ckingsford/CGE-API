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

#ifndef __WEKACPP_CORE_INSTANCES__
#define __WEKACPP_CORE_INSTANCES__

#include <weka/core/Attribute.h>
#include <cstdlib>
#include <stdexcept>
#include <string>

namespace weka {

using namespace std;

class Instance;

class Instances
{
	string m_RelationName;
	vector<Attribute *> m_Attributes;
	vector<Instance *> m_Instances;
	int m_ClassIndex;

 protected:
	int partition(int attIndex, int l, int r);
	void quickSort(int attIndex, int left, int right);
	int select(int attIndex, int left, int right, int k);

 public:
	Instances(string& name, vector<Attribute *>& attrs, int capacity = 0);
	Instances(Instances& other);
	Instances(Instances * other);

	~Instances();

	Attribute *
	attribute(int i)
	{
		return m_Attributes.at(i);
	}

	Attribute *
	attribute(string& name)
	{
		vector<Attribute *>::iterator i = m_Attributes.begin();
		while (i != m_Attributes.end())
			if ((*i)->name() == name)
				return (*i);
		return NULL;
	}

	vector<double>
	attributeToDoubleArray(Attribute& attr)
	{
		return attributeToDoubleArray(attr.index());
	}

	bool
	checkForAttributeType(int attType)
	{
		vector<Attribute *>::iterator i = m_Attributes.begin();
		while (i != m_Attributes.end())
			if ((*i)->type() == attType)
				return true;
		return false;
	}

	bool
	checkForStringAttributes()
	{
		return checkForAttributeType(Attribute::STRING);
	}

	Attribute *
	classAttribute()
	{
		if (m_ClassIndex < 0)
			throw new runtime_error("class index is not set");
		return attribute(m_ClassIndex);
	}

	int
	classIndex()
	{
		return m_ClassIndex;
	}

	void
	deleteWithMissing(Attribute * attr)
	{
		deleteWithMissing(attr->index());
	}

	void
	deleteWithMissingClass()
	{
		if (m_ClassIndex < 0)
			throw new runtime_error("class index is not set");
		deleteWithMissing(m_ClassIndex);
	}

	Instance *
	instance (int i)
	{
		return m_Instances.at(i);
	}

	Instance *
	firstInstance()
	{
		vector<Instance *>::iterator i = m_Instances.begin();
		if (i != m_Instances.end())
			return *i;
	}

	Instance *
	lastInstance()
	{
		vector<Instance *>::reverse_iterator i = m_Instances.rbegin();
		if (i != m_Instances.rend())
			return *i;
	}

	double
	kthSmallestValue(Attribute * attr, int k)
	{
		return kthSmallestValue(attr->index(), k);
	}

	double
	meanOrMode(Attribute& attr)
	{
		return meanOrMode(attr.index());
	}

	int
	numAttributes()
	{
		return m_Attributes.size();
	}

	int
	numClasses()
	{
		if (m_ClassIndex < 0)
			throw new runtime_error("class index is not set");
		if (!classAttribute()->isNominal())
			return 1;
		return classAttribute()->numValues();
	}

	int
	numDistinctValues(Attribute * attr)
	{
		return numDistinctValues(attr->index());
	}

	int
	numInstances()
	{
		return m_Instances.size();
	}

	void
	randomize()
	{
		for (int j = numInstances() - 1; j > 0; j--)
			swap(j, (int)(random() % (j + 1)));
	}

	string
	relationName()
	{
		return m_RelationName;
	}

	void
	setAttribute(int i, Attribute * attr)
	{
		if (m_Attributes[i] != NULL)
			delete m_Attributes[i];
		m_Attributes[i] = attr;
	}

	void
	setClass(Attribute * attr)
	{
		m_ClassIndex = attr->index();
	}

	void
	setClassIndex(int classIndex)
	{
		if (classIndex >= numAttributes())
			throw new runtime_error("invalid class index");
		m_ClassIndex = classIndex;
	}

	void
	setRelationName(string& newName)
	{
		m_RelationName = newName;
	}

	void 
	sort(Attribute& attr)
	{
		sort(attr.index());
	}

	void 
	sort(Attribute * attr)
	{
		sort(attr->index());
	}

	double
	variance(Attribute& attr)
	{
		return variance(attr.index());
	}

	void add(Instance * instance);
	void add(Instance& instance);
	vector<double> attributeToDoubleArray(int attIndex);
	bool checkInstance(Instance& instance);
	void deleteAll();
	void deleteInstance(int index);
	void deleteAttributeAt(int position);
	void deleteWithMissing(int attIndex);
	double kthSmallestValue(int attIndex, int k);
	double meanOrMode(int attIndex);
	int numDistinctValues(int attIndex);
	Instances * resample();
	Instances * resampleWithWeights();
	Instances * resampleWithWeights(vector<double>& weights);
	void sort(int attIndex);
	double sumOfWeights();
	void swap(int i, int j);
	double variance(int attIndex);
};

}

#endif /* __WEKACPP_CORE_INSTANCES__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
