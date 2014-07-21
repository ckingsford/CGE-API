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

#ifndef __WEKACPP_CORE_ATTRIBUTE__
#define __WEKACPP_CORE_ATTRIBUTE__

#include <map>
#include <string>
#include <vector>

#include <cmath>
#ifndef NAN
# define NAN __builtin_nan("")
#endif

namespace weka {

using namespace std;

class Attribute;

class Attribute
{
  protected:
	string m_Name;
	int m_Type;
	double m_Weight;

  public:
	static const int NUMERIC = 0;
	static const int NOMINAL = 1;
	static const int STRING = 2;

	Attribute(string& attrName, int type);

	virtual ~Attribute();

	string
	name()
	{
		return m_Name;
	}

	int
	type()
	{
		return m_Type;
	}

	double
	weight()
	{
		return m_Weight;
	}

	void
	setWeight(double weight)
	{
		m_Weight = weight;
	}

	virtual Attribute * clone (void);

	virtual string value(int index);

	virtual bool isNominal();
	virtual bool isNumeric();
	virtual bool isString();
	virtual int numValues();
	virtual int index();
	virtual void setIndex(int index);
	virtual int indexOfValue(string& value);
	virtual int indexOfValue(string * value);
	virtual void addValue(string * value);
	virtual void setValue(int index, string * s);
	virtual void deleteAllValues(void);
};

class NumericAttribute : public Attribute
{
	int m_Index;
	double m_Value;

  public:
	double m_LowerBound;
	bool m_LowerBoundIsOpen;
	double m_UpperBound;
	bool m_UpperBoundIsOpen;

	NumericAttribute(string& attrName);
	NumericAttribute(string& attrName, int index);

	virtual ~NumericAttribute();

	int
	indexOfValue(double value)
	{
		return 0;
	}

	int
	index()
	{
		return m_Index;
	}

	bool
	isNominal()	
	{
		return false;
	}

	bool
	isNumeric()
	{
		return true;
	}

	bool
	isString()
	{
		return false;
	}

	int
	numValues()
	{ 
		return 1;
	}

	void
	setIndex(int index)
	{
	}

	double
	getLowerBound()
	{
		return m_LowerBound;
	}

	bool
	lowerBoundIsOpen()
	{
		return (m_LowerBound != NAN);
	}

	double
	getUpperBound()
	{
		return m_UpperBound;
	}

	bool
	upperBoundIsOpen()
	{
		return (m_UpperBound != NAN);
	}

	Attribute * clone (void);

	string value(int index);

	void setValue(int index, string * s);
	bool isInRange (double value);
	void setNumericRange(string& range);
};

class NominalAttribute : public Attribute
{
	struct Range
	{
		double lowerBound;
		double upperBound;
		bool lowerBoundIsOpen;
		bool upperBoundIsOpen;
	};

	vector<string *> m_Values;
	// if this attribute is actually a numeric attribute transformed through
	// discretization, we parse the range from the attribute name and hold
	// range information in m_Ranges
	vector<Range> m_Ranges;
	int m_Index;

  public:
	NominalAttribute(string& attrName);
	NominalAttribute(string& attrName, int index);
	NominalAttribute(string& attrName, vector<string *>& attrValues);
	NominalAttribute(string& attrName, vector<string *>& attrValues,
		int index);

	virtual ~NominalAttribute();

	int
	indexOfValue(string& value)
	{
		return indexOfValue(&value);
	}

	int
	index()
	{
		return m_Index;
	}

	string
	value(int index)
	{
		return *m_Values.at(index);
	}

	bool
	isNominal()
	{
		return true;
	}

	bool
	isNumeric()	
	{
		return false;
	}

	bool
	isString()
	{
		return false;
	}

	int
	numValues()
	{
		return m_Values.size();
	}

	void
	setIndex(int index)
	{
		m_Index = index;
	}

	double
	getLowerBound(int index)
	{
		return m_Ranges.at(index).lowerBound;
	}

	bool
	lowerBoundIsOpen(int index)
	{
		return m_Ranges.at(index).lowerBoundIsOpen;
	}

	double
	getUpperBound(int index)
	{
		return m_Ranges.at(index).upperBound;
	}

	bool
	upperBoundIsOpen(int index)
	{
		return m_Ranges.at(index).upperBoundIsOpen;
	}

	Attribute * clone (void);

	bool isRange(string * s);
	void makeRange(int index, string * s);
	bool isInRange (int index, double value);
	int indexOfValue(string * value);
	void addValue(string * s);	
	void setValue(int index, string * s);
	void deleteAllValues(void);
};

class StringAttribute : public Attribute
{
	vector<string *> m_Values;
	int m_Index;

 public:
	StringAttribute(string& attrName);
	StringAttribute(string& attrName, int index);
	StringAttribute(string& attrName, vector<string *>& attrValues);
	StringAttribute(string& attrName, vector<string *>& attrValues,
			int index);

	virtual ~StringAttribute();

	int
	indexOfValue(string& value)
	{
		return indexOfValue(&value);
	}

	int
	index()
	{
		return m_Index;
	}

	string
	value(int index)
	{
		return *m_Values.at(index);
	}

	bool
	isNominal()
	{
		return false;
	}

	bool
	isNumeric()
	{
		return false;
	}

	bool
	isString()
	{
		return true;
	}

	int
	numValues()
	{
		return m_Values.size();
	}

	void
	setIndex(int index)
	{
		m_Index = index;
	}

	Attribute * clone (void);

	int indexOfValue(string * value);
	void addValue(string * s);
	void setValue(int index, string * s);
	void deleteAllValues(void);
};

}

#endif /* __WEKACPP_CORE_ATTRIBUTE__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
