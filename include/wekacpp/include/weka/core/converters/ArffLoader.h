/*
 * Derived from:
 *
 * ArffLoader.java
 * Copyright (C) 2004 University of Waikato, Hamilton, New Zealand
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

#ifndef __WEKACPP_CORE_CONVERTERS_ARFFLOADER__
#define __WEKACPP_CORE_CONVERTERS_ARFFLOADER__

#include <weka/core/Attribute.h>
#include <weka/core/Instance.h>
#include <weka/core/Instances.h>
#include <string>
#include <streambuf>

namespace weka {
	namespace converters {

using namespace std;

class ArffLoader
{
 public:
	struct Token
	{
		enum {
			ERR,
			COMMA,
			LBRACK,
			RBRACK,
			LPAREN,
			RPAREN,
			LBRACE,
			RBRACE,
			IDENT,
			NUMBER,
			STRING,
			ATTRIBUTE,
			DATA,
			RELATION,
		} kind;
		string text;

		Token() : 
			kind(ERR)
		{
		}

		string toString();
	};

 private:
	enum {
		IN_ERR,
		IN_HEADER,
		IN_DATA,
	} m_Phase;

	streambuf * m_Stream;
	bool m_bHeaderRead;
	string m_RelationName;
	vector<Attribute *> m_Attributes;
	Instances * m_Header;
	int m_Line;

	Token getNextToken();
	void readHeader();

 public:
	ArffLoader(streambuf * s);
	~ArffLoader();

	Instances * getStructure();
	Instances * getDataSet();
	Instance * getNextInstance();
};

} // namespace converters
} // namespace weka

#endif /* __WEKACPP_CORE_CONVERTERS_ARFFLOADER__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
