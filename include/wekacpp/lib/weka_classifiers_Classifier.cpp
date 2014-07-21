/*
 * Derived from:
 *
 * Classifier.java
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

#include <weka/core/Instances.h>
#include <weka/core/Instance.h>
#include <weka/core/Attribute.h>
#include <weka/classifiers/Classifier.h>
#include <string>
#include <vector>

namespace weka {
	namespace classifiers {

using namespace std;

Classifier::~Classifier()
{
}

double
Classifier::classifyInstance (Instance * instance)
{
	vector<double> dist = distributionForInstance(instance);
    switch (instance->classAttribute()->type()) {
    case Attribute::NOMINAL: {
		double max = 0;
		int maxIndex = 0;
		for (int i = 0; i < (int)dist.size(); i++) {
			if (dist[i] > max) {
				maxIndex = i;
				max = dist[i];
			}
		}
		if (max > 0)
			return maxIndex;
	}	break;
	case Attribute::NUMERIC:
		return dist[0];
    default:
		break;
    }
	return Instance::missingValue();
}

} // namespace classifiers
} // namespace weka

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
