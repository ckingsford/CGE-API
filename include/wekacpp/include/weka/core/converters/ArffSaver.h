/*
 * Derived from:
 *
 * ArffSaver.java
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

#ifndef __WEKACPP_CORE_CONVERTERS_ARFFSAVER__
#define __WEKACPP_CORE_CONVERTERS_ARFFSAVER__

#include <string>
#include <streambuf>

namespace weka {
	namespace converters {

using namespace std;

class ArffSaver
{
	streambuf * m_Stream;
	bool m_bHeaderWritten;
	Instances * m_Header;

	void writeHeader(Instances * header);
	void writeInstance(Instance * instance);

 public:
	ArffSaver(streambuf * s);
	~ArffSaver();

	void writeBatch(Instances * instances);
	void writeIncremental(Instance * instance);
};

} // namespace converters
} // namespace weka

#endif /* __WEKACPP_CORE_CONVERTERS_ARFFSAVER__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
