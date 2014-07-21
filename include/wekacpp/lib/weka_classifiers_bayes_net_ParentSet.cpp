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

#include <weka/classifiers/bayes/net/ParentSet.h>
#include <stdexcept>
#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace std;

ParentSet::ParentSet()
{
	m_nParents.resize(10);
	m_nNrOfParents = 0;
	m_nCardinalityOfParents = 1;
}

ParentSet::ParentSet(int nMaxNrOfParents)
{
	m_nParents.resize(nMaxNrOfParents);
	m_nNrOfParents = 0;
	m_nCardinalityOfParents = 1;
}

ParentSet::ParentSet(ParentSet& other)
{
	m_nNrOfParents = other.m_nNrOfParents;
	m_nCardinalityOfParents = other.m_nCardinalityOfParents;
	m_nParents.resize(m_nNrOfParents);
	for (int iParent = 0; iParent < m_nNrOfParents; iParent++)
		m_nParents[iParent] = other.m_nParents[iParent];
}

ParentSet::ParentSet(ParentSet * other)
{
	m_nNrOfParents = other->m_nNrOfParents;
	m_nCardinalityOfParents = other->m_nCardinalityOfParents;
	m_nParents.resize(m_nNrOfParents);
	for (int iParent = 0; iParent < m_nNrOfParents; iParent++)
		m_nParents[iParent] = other->m_nParents[iParent];
}

void
ParentSet::addParent(int nParent, Instances * instances)
{
	m_nParents.resize(m_nNrOfParents + 1);
	m_nParents[m_nNrOfParents] = nParent;
	m_nNrOfParents++;
	m_nCardinalityOfParents *= instances->attribute(nParent)->numValues();
}

void
ParentSet::addParent (int nParent, int iParent, Instances * instances)
{
	m_nParents.resize(nParent + 1);
	for (int iParent2 = m_nNrOfParents; iParent2 > iParent; iParent2--)
		m_nParents[iParent2] = m_nParents[iParent2 - 1];		
	m_nParents[iParent] = nParent;
	m_nNrOfParents++;
	m_nCardinalityOfParents *= instances->attribute(nParent)->numValues();
}

int
ParentSet::deleteParent(int nParent, Instances * instances)
{
	int iParent = 0;
	while ((m_nParents[iParent] != nParent) && (iParent < m_nNrOfParents))
		iParent++;
	int iParent2 = -1;
	if (iParent < m_nNrOfParents)
		iParent2 = iParent;
	if (iParent < m_nNrOfParents) {
		while (iParent < m_nNrOfParents - 1) {
			m_nParents[iParent] = m_nParents[iParent + 1];
			iParent++;
		}
		m_nNrOfParents--;
		m_nCardinalityOfParents /= 
			instances->attribute(nParent)->numValues();
	}
	return iParent2;
} 

void
ParentSet::deleteLastParent(Instances * instances)
{
	m_nNrOfParents--;
	m_nCardinalityOfParents = m_nCardinalityOfParents /
		instances->attribute(m_nParents[m_nNrOfParents])->numValues();
}    

} // namespace bayes
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
