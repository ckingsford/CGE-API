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

#include <weka/core/Debug.h>
#include <cstdarg>
#include <cstdio>
#include <iostream>

namespace weka {

using namespace std;

void
Debug::setStream (ostream * os)
{
	m_oStream = os;
	m_fStream = NULL;
}

void
Debug::setStream (FILE * fp)
{
	m_fStream = fp;
	m_oStream = NULL;
}

void
Debug::dprintf (const char * fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	if (m_fStream != NULL) {
		vfprintf(m_fStream, (char *)fmt, va);
	} else if (m_oStream != NULL) {
		char buf[2048];
		int len = vsprintf(buf, (char *)fmt, va);
		m_oStream->write(buf, len);
	}
	va_end(va);
}

class Debug Debug;

}

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
