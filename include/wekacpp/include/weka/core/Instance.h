/*
 * Derived from:
 *
 * Instance.java
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

#ifndef __WEKACPP_CORE_INSTANCE__
#define __WEKACPP_CORE_INSTANCE__

#include <weka/core/Attribute.h>
#include <weka/core/Instances.h>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace weka {

using namespace std;

class Instance
{
	Instances * m_Dataset;
	vector<double> m_AttrValues;
	double m_Weight;

 protected:
	Instance();

 public:
	static double missingValue() { return NAN; }

	Instance(const Instance * instance) :
		m_Dataset(instance->m_Dataset),
		m_AttrValues(instance->m_AttrValues),
		m_Weight(instance->m_Weight)
	{
	}

	Instance(const Instance& instance) :
		m_Dataset(instance.m_Dataset),
		m_AttrValues(instance.m_AttrValues),
		m_Weight(instance.m_Weight)
	{
	}

	Instance(double weight, vector<double>& values) :
		m_Dataset(NULL),
		m_AttrValues(values),
		m_Weight(weight)
	{
	}

	Instance(int numAttributes);

	Attribute *
	attribute(int index)
	{
		if (m_Dataset == NULL)
			throw new runtime_error("instance does not have access to a "
				"dataset");
		return m_Dataset->attribute(index);
	}

	Attribute *
	classAttribute()
	{
		if (m_Dataset == NULL)
			throw new runtime_error("instance does not have access to a "
				"dataset");
		return m_Dataset->classAttribute();
	}

	int
	classIndex()
	{
		if (m_Dataset == NULL)
			throw new runtime_error("instance does not have access to a "
				"dataset");
		return m_Dataset->classIndex();
	}

	bool
	classIsMissing()
	{
		if (classIndex() < 0)
			throw new runtime_error("class is not set");
		return isMissing(classIndex());
	}

	double
	classValue()
	{
		if (classIndex() < 0)
			throw new runtime_error("class is not set");
		return m_AttrValues[classIndex()];
	}

	Instances *
	dataset()
	{
		return m_Dataset;
	}

	bool
	hasMissingValue()
	{
		if (m_Dataset == NULL)
			throw new runtime_error("instance does not have access to a "
				"dataset");
		for (int i = 0; i < numAttributes(); i++) {
			if (i != classIndex()) {
				if (isMissing(i)) {
					return true;
				}
			}
		}
		return false;
	}

	Instances *
	getDataset (void)
	{
		return m_Dataset;
	}

	int
	index(int position)
	{
		return position;
	}

	bool
	isMissing(int attrIndex)
	{
		if (m_AttrValues.at(attrIndex) == missingValue())
			return true;
		return false;
	}

	bool
	isMissing(Attribute& attr)
	{
		return isMissing(attr.index());
	}

	bool
	isMissing(Attribute * attr)
	{
		return isMissing(attr->index());
	}

	bool
	isMissingValue(double val)
	{
		return (val == missingValue());
	}

	int
	numAttributes()
	{
		return m_AttrValues.size();
	}

	int
	numClasses()
	{
		if (m_Dataset == NULL)
			throw new runtime_error("instance does not have access to a "
				"dataset");
		return m_Dataset->numClasses();
	}

	int
	numValues()
	{
		return m_AttrValues.size();
	}

	void
	replaceMissingValues(vector<double>& array)
	{
		if (array.size() != m_AttrValues.size())
			throw new runtime_error("unequal number of attributes");
 		for (unsigned int i = 0; i < m_AttrValues.size(); i++)
 			if (isMissing(i))
 				m_AttrValues[i] = array[i];
	}

	void
	setClassMissing()
	{
		if (classIndex() < 0)
			throw new runtime_error("class is not set");
		setMissing(classIndex());
	}

	void
	setClassValue(double value)
	{
		if (classIndex() < 0)
			throw new runtime_error("class is not set");
		setValue(classIndex(), value);
	}

	void
	setClassValue(string& value)
	{
		if (classIndex() < 0)
			throw new runtime_error("class is not set");
		setValue(classIndex(), value);
	}

	void
	setDataset(Instances * instances)
	{
		m_Dataset = instances;
	}

	void
	setMissing(int attrIndex)
	{
		setValue(attrIndex, missingValue());
	}

	void
	setMissing(Attribute& attr)
	{
		setMissing(attr.index());
	}

	void
	setValue(Attribute& attr, double value)
	{
		setValue(attr.index(), value);
	}

	void
	setWeight(double weight)
	{
		m_Weight = weight;
	}

	double
	value(int attrIndex)
	{
		return m_AttrValues.at(attrIndex);
	}

	double
	value(Attribute& attr)
	{
		return value(attr.index());
	}

	double
	value(Attribute * attr)
	{
		return value(attr->index());
	}

	double
	weight()
	{
		return m_Weight;
	}

	Instance * 	mergeInstance(Instance& instance);
	void setValue(int attrIndex, double value);
	void setValue(int attrIndex, string& value);
	void setValue(Attribute& attr, string& value);
};

}

#endif /* __WEKACPP_CORE_INSTANCE__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
