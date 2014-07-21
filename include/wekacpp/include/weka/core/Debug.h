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

#ifndef __WEKACPP_CORE_DEBUG__
#define __WEKACPP_CORE_DEBUG__

#include <cstdio>
#include <iostream>

namespace weka {

using namespace std;

class Debug
{
	FILE * m_fStream;
	ostream * m_oStream;

 public:
	Debug() :
		m_fStream(NULL),
		m_oStream(NULL)
	{
	}

	void setStream (ostream * os);
	void setStream (FILE * fp);
	void dprintf (const char * fmt, ...);
};

extern Debug Debug;

}

#endif /* __WEKACPP_CORE_DEBUG__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
