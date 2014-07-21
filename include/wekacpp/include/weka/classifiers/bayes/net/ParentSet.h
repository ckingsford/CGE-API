/*
 * Derived from:
 *
 * ParentSet.java
 * Copyright (C) 2001 University of Waikato, Hamilton, New Zealand
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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_NET_PARENTSET__
#define __WEKACPP_CLASSIFIERS_BAYES_NET_PARENTSET__

#include <weka/core/Instances.h>
#include <stdexcept>
#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {

class ParentSet
{
	vector<int> m_nParents;
	int m_nNrOfParents;
	int m_nCardinalityOfParents;

 public:
	ParentSet();
	ParentSet(int nMaxNrOfParents);
	ParentSet(ParentSet& other);
	ParentSet(ParentSet * other);

	void
	maxParentSetSize(int nSize)
	{
		m_nParents.resize(nSize);
	}

	int
	getParent (int iParent)
	{
		return m_nParents.at(iParent);
	} 

	void
	setParent (int iParent, int nNode)
	{
		if ((int)m_nParents.size() < iParent)
			throw new runtime_error("MaxNrOfParents exceeded");
		m_nParents[iParent] = nNode;
	}

	int
	getNrOfParents ()
	{
		return m_nNrOfParents;
	}

	bool
	contains (int iNode)
	{
		for (int iParent = 0; iParent < m_nNrOfParents; iParent++)
			if (m_nParents[iParent] == iNode)
				return true;
		return false;
	}

	int 
	getCardinalityOfParents()
	{
		return m_nCardinalityOfParents;
	} 

	void addParent(int nParent, Instances * instances);
	void addParent (int nParent, int iParent, Instances * instances);
	int deleteParent(int nParent, Instances * instances);
	void deleteLastParent(Instances * instances);
};

} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_NET_PARENTSET__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
