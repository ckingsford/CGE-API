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

#ifndef __WEKACPP_FILTERS_FILTER__
#define __WEKACPP_FILTERS_FILTER__

#include <weka/core/Instances.h>
#include <weka/core/Instance.h>
#include <list>
#include <stdexcept>

namespace weka {
	namespace filters {

using namespace std;

class Filter
{
 protected:
	Instances * m_Format;
	Instances * m_InputSet;
	list<Instance *> m_OutputQueue;

 public:
	Filter():
		m_Format(NULL),
		m_InputSet(NULL)
	{
	}

	virtual ~Filter();

	Instances *
	getOutputFormat (void)
	{
		return m_Format;
	}

	Instance * 
	output (void)
	{
		if (m_OutputQueue.empty())
			return NULL;
		Instance * instance = m_OutputQueue.front();
		m_OutputQueue.pop_front();
		return instance;
	}

	Instance *
	outputPeek (void)
	{
		if (m_OutputQueue.empty())
			return NULL;
		return m_OutputQueue.front();
	}

	int
	numPendingOutput (void)
	{
		return m_OutputQueue.size();
	}

	bool
	isOutputFormatDefined (void)
	{
		return m_Format != NULL;
	}

	void
	resetQueue (void)
	{
		while (!m_OutputQueue.empty()) {
			Instance * instance = m_OutputQueue.front();
			m_OutputQueue.pop_front();
			delete instance;
		}
	}

	virtual void setInputFormat(Instances * instances);
	virtual void setOutputFormat(Instances * instances);
	virtual bool input(Instance * instance);
	virtual bool batchFinished(void);
	virtual Instances * useFilter(Instances * in);

};

} // namespace filters
} // namespace weka

#endif /* __WEKACPP_ESTIMATORS_ESTIMATOR__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
