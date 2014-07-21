/*
 * Derived from:
 *
 * TAN.java
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

#ifndef __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_TAN__
#define __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_TAN__

#include <weka/core/Instances.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <weka/classifiers/bayes/net/local/LocalScoreSearchAlgorithm.h>
#include <vector>

namespace weka {
	namespace classifiers {
		namespace bayes {
			namespace local {

using namespace std;

class TAN : public LocalScoreSearchAlgorithm 
{
 public:
	TAN();
	TAN(BayesNet * bayesNet, Instances * instances);

	virtual ~TAN();

	void buildStructure (BayesNet * bayesNet, Instances * instances);
};

} // namespace local
} // namespace bayes
} // namespace classifiers
} // namespace weka

#endif /* __WEKACPP_CLASSIFIERS_BAYES_NET_LOCAL_TAN__ */

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
