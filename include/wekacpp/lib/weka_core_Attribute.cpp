/*
 * Derived from:
 *
 * Attribute.java
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
#include <string>
#include <stdexcept>

#include <cmath>
#ifndef NAN
# define NAN		__builtin_nan("")
#endif
#ifndef INFINITY
# define INFINITY	__builtin_inf()
#endif

namespace weka {

using namespace std;

Attribute::Attribute(string& attrName, int type) : 
	m_Name(attrName), m_Type(type)
{
}

Attribute::~Attribute()
{
}

Attribute *
Attribute::clone (void)
{
	Attribute * attr = new Attribute(this->m_Name, this->m_Type);
	attr->m_Weight = this->m_Weight;
	return attr;
}

string
Attribute::value(int index)
{
	string s;
	throw new runtime_error("not implemented");
	return s;
}

bool
Attribute::isNominal()
{
	throw new runtime_error("not implemented");
	return false;
}

bool
Attribute::isNumeric()
{
	throw new runtime_error("not implemented");
	return false;
}

bool
Attribute::isString()
{
	throw new runtime_error("not implemented");
	return false;
}

int
Attribute::numValues()
{
	throw new runtime_error("not implemented");
	return 0;
}

int
Attribute::index()
{
	throw new runtime_error("not implemented");
	return 0;
}

void
Attribute::setIndex(int index)
{
	throw new runtime_error("not implemented");
}

int
Attribute::indexOfValue (string * value)
{
	throw new runtime_error("not implemented");
	return 0;
}

int
Attribute::indexOfValue (string& value)
{
	return indexOfValue(&value);
}

void
Attribute::addValue (string * value)
{
	throw new runtime_error("not implemented");
}

void
Attribute::setValue (int index, string * s)
{
	throw new runtime_error("not implemented");
}

void
Attribute::deleteAllValues (void)
{
	throw new runtime_error("not implemented");
}

NominalAttribute::NominalAttribute (string& attrName) : 
	Attribute(attrName, Attribute::NOMINAL)
{
}

NominalAttribute::NominalAttribute (string& attrName, int index) : 
	Attribute(attrName, Attribute::NOMINAL), m_Index(index)
{
}

NominalAttribute::NominalAttribute (string& attrName,
		vector<string *>& attrValues) :
	Attribute(attrName, Attribute::NOMINAL), m_Values(attrValues)
{
	vector<string *>::iterator i = attrValues.begin();
	int j = 0;
	while (i != attrValues.end()) {
		if (isRange(*i))
			makeRange(j, *i);
	}
}

NominalAttribute::NominalAttribute (string& attrName,
		vector<string *>& attrValues, int index) :
	Attribute(attrName, Attribute::NOMINAL), m_Values(attrValues),
	m_Index(index) 
{
	vector<string *>::iterator i = attrValues.begin();
	int j = 0;
	while (i != attrValues.end()) {
		if (isRange(*i))
			makeRange(j, *i);
	}
}

NominalAttribute::~NominalAttribute()
{
}

Attribute *
NominalAttribute::clone (void)
{
	NominalAttribute * attr = new NominalAttribute(this->m_Name);
	attr->m_Type = this->m_Type;
	attr->m_Weight = this->m_Weight;
	attr->m_Values.resize(this->m_Values.size());
	for (int i = 0; i < (int)this->m_Values.size(); i++)
		attr->m_Values[i] = new string(this->m_Values[i]->c_str());
	attr->m_Ranges = this->m_Ranges;
	attr->m_Index = this->m_Index;
	return (Attribute *)attr;
}

bool
NominalAttribute::isRange (string * s)
{
	const char * p = s->c_str();

	while (*p != 0) {
		if ((*p == 'A') && (*(p+1) == 'l') && (*(p+2) == 'l')) {
			return true;
		}
		if ((*p == 'N') && (*(p+1) == 'o') && (*(p+2) == 'n') &&
				(*(p+3) == 'e'))
		{
			return false;
		}
		if ((*p == '(') || (*p == '[')) {
			p++;
			while (*p != 0) {
				if (!strchr("+-.eEinf0123456789", *p))
					return false;
				p++;
				if ((*p == ')') || (*p == ']'))
					return true;
			}
		}
		p++;
	}
	return false;
}

void
NominalAttribute::makeRange (int index, string * s)
{
	const char * p = s->c_str();
	char buf[40];
	int i;

	if ((int)m_Ranges.size() <= index)
		m_Ranges.resize(index + 1);

	// unpack range information if attribute name is in suitable format

	while (*p != 0) {
		if ((*p == 'A') && (*(p+1) == 'l') && (*(p+2) == 'l')) {
			m_Ranges[index].lowerBoundIsOpen = true;
			m_Ranges[index].lowerBound = -INFINITY;
			m_Ranges[index].upperBoundIsOpen = true;
			m_Ranges[index].upperBound = INFINITY;
			return;
		}
		if ((*p == 'N') && (*(p+1) == 'o') && (*(p+2) == 'n') &&
			(*(p+3) == 'e'))
		{
			m_Ranges[index].lowerBoundIsOpen = true;
			m_Ranges[index].lowerBound = NAN;
			m_Ranges[index].upperBoundIsOpen = true;
			m_Ranges[index].upperBound = NAN;
			return;
		}
		if ((*p == '(') || (*p == '['))
			break;
		p++;
	}
	if (*p == 0)
		goto error;
	m_Ranges[index].lowerBoundIsOpen = (*p == '(');

	p++;
	if (*p == 0)
		goto error;

	i = 0;
	buf[i++] = *p++;
	while ((*p != 0) && (*p != '-') && (i < 40))
		buf[i++] = *p++;
	if (i == 40)
		goto error;
	if (*p != '-')
		goto error;
	buf[i] = 0;
	if (!strcmp(buf, "-inf")) {
		m_Ranges[index].lowerBound = -INFINITY;
	} else if (!strcmp(buf, "+inf") || !strcmp(buf, "inf")) {
		m_Ranges[index].lowerBound = INFINITY;
	} else {
		double d;
		if (sscanf(buf, "%lf", &d) != 1)
			goto error;
		m_Ranges[index].lowerBound = d;
	}

	p++;
	if (*p == 0)
		goto error;

	i = 0;
	buf[i++] = *p++;
	while ((*p != 0) && (*p != ')') && (*p != ']') && (i < 40))
		buf[i++] = *p++;
	if (i == 40)
		goto error;
	if (*p == 0)
		goto error;
	buf[i] = 0;
	m_Ranges[index].upperBoundIsOpen = (*p == ')');
	if (!strcmp(buf, "-inf")) {
		m_Ranges[index].upperBound = -INFINITY;
	} else if (!strcmp(buf, "+inf") || !strcmp(buf, "inf")) {
		m_Ranges[index].upperBound = INFINITY;
	} else {
		double d;
		if (sscanf(buf, "%lf", &d) != 1)
			goto error;
		m_Ranges[index].upperBound = d;
	}

	return;

  error:
	throw new runtime_error("invalid range");
}

int
NominalAttribute::indexOfValue (string * value)
{
	vector<string *>::iterator i = m_Values.begin();
	for (int idx = 0; i != m_Values.end(); i++, idx++)
		if (*value == **i)
			return idx;
	return -1;
}

void 
NominalAttribute::addValue (string * s)
{
	int i;
	for (i = 0; i < (int)m_Values.size(); i++)
		if (s == m_Values[i])
			return;
	m_Values.resize(i + 1);
	m_Values[i] = s;
	if (isRange(s))
		makeRange(i, s);
}

void
NominalAttribute::setValue (int index, string * s)
{
	if ((int)m_Values.size() <= index) {
		m_Values.resize(index + 1);
	} else {
		if (m_Values[index] != NULL)
			delete m_Values[index];
	}
	m_Values[index] = s;
	if (isRange(s))
		makeRange(index, s);
}

bool
NominalAttribute::isInRange (int index, double value)
{
	if (getLowerBound(index) == NAN || getUpperBound(index) == NAN)
		return false;
	if (lowerBoundIsOpen(index)) {
		if (value <= getLowerBound(index))
			return false;
	} else {
		if (value < getLowerBound(index))
			return false;
	}
	if (upperBoundIsOpen(index)) {
		if (value >= getUpperBound(index))
			return false;
	} else {
		if (value > getUpperBound(index))
			return false;
	}
	return true;
}

void
NominalAttribute::deleteAllValues (void)
{
	m_Values.clear();
	m_Ranges.clear();
}

NumericAttribute::NumericAttribute (string& attrName) : 
	Attribute(attrName, Attribute::NUMERIC)
{
	m_LowerBound = -INFINITY;
	m_UpperBound = INFINITY;
	m_LowerBoundIsOpen = true;
	m_UpperBoundIsOpen = true;
}

NumericAttribute::NumericAttribute (string& attrName, int index) : 
	Attribute(attrName, Attribute::NUMERIC), m_Index(index)
{
	m_LowerBound = -INFINITY;
	m_UpperBound = INFINITY;
	m_LowerBoundIsOpen = true;
	m_UpperBoundIsOpen = true;
}

NumericAttribute::~NumericAttribute()
{
}

Attribute *
NumericAttribute::clone (void)
{
	NumericAttribute * attr = new NumericAttribute(this->m_Name);
	attr->m_Type = this->m_Type;
	attr->m_Weight = this->m_Weight;
	attr->m_Index = this->m_Index;
	attr->m_Value = this->m_Value;
	attr->m_LowerBound = this->m_LowerBound;
	attr->m_LowerBoundIsOpen = this->m_LowerBoundIsOpen;
	attr->m_UpperBound = this->m_UpperBound;
	attr->m_UpperBoundIsOpen = this->m_UpperBoundIsOpen;
	return (Attribute *)attr;
}

string
NumericAttribute::value (int index)
{
	if (index > 0)
		throw new runtime_error("index out of range");
	char buf[40];
	sprintf(buf, "%lf", m_Value);
	string s(buf);
	return s;
}

void
NumericAttribute::setValue (int index, string * s)
{
	if (index > 0)
		throw new runtime_error("index out of range");
	m_Value = strtod(s->c_str(), NULL);
}

void
NumericAttribute::setNumericRange (string& s)
{
	const char * p = s.c_str();
	char buf[40];
	int i;

	// unpack range information if attribute name is in suitable format

	while ((*p != 0) && (*p != '(') && (*p != '['))
		p++;
	if (*p == 0)
		goto error;
	m_LowerBoundIsOpen = (*p == '(');

	p++;
	if (*p == 0)
		goto error;

	i = 0;
	buf[i++] = *p++;
	while ((*p != 0) && (*p != '-') && (i < 40))
		buf[i++] = *p++;
	if (i == 40)
		goto error;
	if (*p != '-')
		goto error;
	buf[i] = 0;
	if (!strcmp(buf, "-inf")) {
		m_LowerBound = -INFINITY;
	} else if (!strcmp(buf, "+inf") || !strcmp(buf, "inf")) {
		m_LowerBound = INFINITY;
	} else {
		double d;
		if (sscanf(buf, "%lf", &d) != 1)
			goto error;
		m_LowerBound = d;
	}

	p++;
	if (*p == 0)
		goto error;

	i = 0;
	buf[i++] = *p++;
	while ((*p != 0) && (*p != ')') && (*p != ']') && (i < 40))
		buf[i++] = *p++;
	if (i == 40)
		goto error;
	if (*p != 0)
		goto error;
	buf[i] = 0;
	m_UpperBoundIsOpen = (*p == ')');
	if (!strcmp(buf, "-inf")) {
		m_UpperBound = -INFINITY;
	} else if (!strcmp(buf, "+inf") || !strcmp(buf, "inf")) {
		m_UpperBound = INFINITY;
	} else {
		double d;
		if (sscanf(buf, "%lf", &d) != 1)
			goto error;
		m_UpperBound = d;
	}

	return;

  error:
	throw new runtime_error("invalid range");
}

bool
NumericAttribute::isInRange (double value)
{
	if (m_LowerBound == NAN || m_UpperBound == NAN)
		return false;
	if (m_LowerBoundIsOpen) {
		if (value <= m_LowerBound)
			return false;
	} else {
		if (value < m_LowerBound)
			return false;
	}
	if (m_UpperBoundIsOpen) {
		if (value >= m_UpperBound)
			return false;
	} else {
		if (value > m_UpperBound)
			return false;
	}
	return true;
}

StringAttribute::StringAttribute (string& attrName) : 
	Attribute(attrName, Attribute::STRING)
{
}

StringAttribute::StringAttribute (string& attrName, int index) : 
	Attribute(attrName, Attribute::STRING), m_Index(index)
{
}

StringAttribute::StringAttribute (string& attrName,
		vector<string *>& attrValues) :
	Attribute(attrName, Attribute::STRING), m_Values(attrValues)
{
}

StringAttribute::StringAttribute (string& attrName,
		vector<string *>& attrValues, int index) :
	Attribute(attrName, Attribute::STRING), m_Values(attrValues),
	m_Index(index)
{
}

StringAttribute::~StringAttribute()
{
}

Attribute *
StringAttribute::clone (void)
{
	StringAttribute * attr = new StringAttribute(this->m_Name);
	attr->m_Type = this->m_Type;
	attr->m_Weight = this->m_Weight;
	attr->m_Values.resize(this->m_Values.size());
	for (int i = 0; i < (int)this->m_Values.size(); i++)
		attr->m_Values[i] = new string(this->m_Values[i]->c_str());
	attr->m_Index = this->m_Index;
	return (Attribute *)attr;
}

int
StringAttribute::indexOfValue (string * value)
{
	vector<string *>::iterator i = m_Values.begin();
	for (int idx = 0; i != m_Values.end(); i++, idx++)
		if (*value == **i)
			return idx;
	return -1;
}

void
StringAttribute::addValue (string * s)
{
	int i;
	for (i = 0; i < (int)m_Values.size(); i++)
		if (s == m_Values[i])
			return;
	m_Values.resize(i + 1);
	m_Values[i] = s;
}

void
StringAttribute::setValue (int index, string * s)
{
	if ((int)m_Values.size() <= index) {
		m_Values.resize(index + 1);
	} else {
		if (m_Values[index] != NULL)
			delete m_Values[index];
	}
	m_Values[index] = s;
}

void
StringAttribute::deleteAllValues (void)
{
	m_Values.clear();
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
