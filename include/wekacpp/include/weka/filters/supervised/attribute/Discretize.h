/*
 * Derived from:
 *
 * Discretize.java
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

#ifndef __WEKACPP_FILTERS_DISCRETIZE__
#define __WEKACPP_FILTERS_DISCRETIZE__

#include <weka/core/Instances.h>
#include <weka/core/Instance.h>
#include <weka/filters/Filter.h>
#include <stdexcept>

namespace weka {
	namespace filters {

using namespace std;

class Discretize : public Filter
{
	vector<vector<double> > m_CutPoints;
	bool m_UseBetterEncoding;

	vector<double> cutPointsForSubset(int attIndex, int first,
		int lastPlusOne);
	vector<double> cutPointsForAttribute(int attIndex);
	void calculateCutPoints(void);
	void updateOutputFormat(void);
	Instance * convertInstance(Instance * instance);

 public:
	Discretize();
	virtual ~Discretize();

	bool batchFinished(void);
};

} // namespace filters
} // namespace weka

#endif /* __WEKACPP_FILTERS_DISCRETIZE__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:

