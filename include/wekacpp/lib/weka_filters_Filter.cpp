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

#include <weka/core/Instance.h>
#include <weka/core/Instances.h>
#include <weka/filters/Filter.h>
#include <stdexcept>

namespace weka {
	namespace filters {

using namespace std;

Filter::~Filter()
{
	if (m_InputSet != NULL)
		delete m_InputSet;
	resetQueue();
}

void
Filter::setInputFormat (Instances * instances)
{
	if (m_InputSet != NULL)
		delete m_InputSet;
	// shallow copy only
	string name(instances->relationName());
	vector<Attribute *> attrs;
	for (int i = 0; i < instances->numAttributes(); i++)
		attrs.push_back(instances->attribute(i));
	m_InputSet = new Instances(name, attrs);
	m_InputSet->setClassIndex(instances->classIndex());
}

void
Filter::setOutputFormat (Instances * instances)
{
	m_Format = instances;
}

bool
Filter::input (Instance * instance)
{
	if (m_InputSet == NULL)
		return false;
	m_InputSet->add(new Instance(instance));
	return true;
}

bool
Filter::batchFinished (void)
{
	if (m_InputSet == NULL)
		return false;
	m_InputSet->deleteAll();
	return true;
}

Instances *
Filter::useFilter (Instances * in)
{
	Instances * out = new Instances(m_Format), * old = m_Format;
	for (int i = 0; i < in->numInstances(); i++)
		this->input(in->instance(i));
	this->setOutputFormat(out);
	this->batchFinished();
	this->setOutputFormat(old);
	Instance * instance;
	while ((instance = this->output()) != NULL)
		out->add(instance);
	return out;
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

