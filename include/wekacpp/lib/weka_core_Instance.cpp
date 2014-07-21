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

#include <weka/core/Attribute.h>
#include <weka/core/Instance.h>
#include <weka/core/Instances.h>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace weka {

using namespace std;

Instance::Instance(int numAttributes)
{
	m_AttrValues.resize(numAttributes);
	for (int i = 0; i < numAttributes; i++)
		m_AttrValues[i] = missingValue();
	m_Weight = 1;
	m_Dataset = NULL;
}

Instance *
Instance::mergeInstance(Instance& instance)
{
	vector<double> newVals;
	newVals.resize(numAttributes() + instance.numAttributes());
	int m = 0;
	for (int j = 0; j < numAttributes(); j++, m++)
		newVals[m] = value(j);
	for (int j = 0; j < instance.numAttributes(); j++, m++)
		newVals[m] = instance.value(j);
	return new Instance(1.0, newVals);
}

void
Instance::setValue(int attrIndex, double value)
{
	if ((int)m_AttrValues.size() <= attrIndex) {
		int i = (int) m_AttrValues.size();
		m_AttrValues.resize(attrIndex + 1);
		while (i < attrIndex) {
			m_AttrValues[i] = missingValue();
			i++;
		}
	}
	m_AttrValues[attrIndex] = value;
}

void
Instance::setValue(int attrIndex, string& value)
{
	int valIndex;
	if (m_Dataset == NULL)
		throw new runtime_error("instance does not have access to a dataset");
	if (!attribute(attrIndex)->isNominal() && 
		!attribute(attrIndex)->isString()) 
		throw new runtime_error("attribute neither nominal nor string");
	valIndex = attribute(attrIndex)->indexOfValue(value);
	if (valIndex == -1) {
		if (attribute(attrIndex)->isNominal()) {
			throw new runtime_error("value not defined for given nominal "
				"attribute");
		} else {
			string * s = new string(value);
			attribute(attrIndex)->addValue(s);
			valIndex = attribute(attrIndex)->indexOfValue(value);
		}
	}
	setValue(attrIndex, (double)valIndex); 
}

void
Instance::setValue(Attribute& attr, string& value)
{
	if (!attr.isNominal() && !attr.isString())
		throw new runtime_error("attribute neither nominal nor string");
	int valIndex = attr.indexOfValue(value);
	if (valIndex == -1) {
		if (attr.isNominal()) {
			throw new runtime_error("value not defined for given "
				"nominal attribute");
		} else {
			attr.addValue(new string(value));
			valIndex = attr.indexOfValue(value);
		}
	}
	setValue(attr.index(), (double)valIndex);
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
