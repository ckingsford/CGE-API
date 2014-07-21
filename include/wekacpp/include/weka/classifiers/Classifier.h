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

#ifndef __WEKACPP_CLASSIFIERS_CLASSIFIER__
#define __WEKACPP_CLASSIFIERS_CLASSIFIER__

#include <weka/core/Instances.h>
#include <weka/core/Instance.h>
#include <string>
#include <vector>

namespace weka {
	namespace classifiers {

using namespace std;

class Classifier
{
 protected:
	string m_Name;
	bool m_Debug;

 public:
	Classifier (string& name) :
		m_Name(name)
	{
	}

	virtual ~Classifier();

	bool
	getDebug()
	{
		return m_Debug;
	}

	void 
	setDebug (bool value)
	{
		m_Debug = value;
	}

	virtual void buildClassifier(Instances * data) = 0;
	virtual double classifyInstance(Instance * i);
	virtual void updateClassifier (Instance * instance) = 0;
	virtual vector<double> distributionForInstance(Instance * i) = 0;
};

} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_CLASSIFIER__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
