/*
 * Derived from:
 * 
 * BayesNetEstimator.java
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

#include <weka/core/Instance.h>
#include <weka/classifiers/bayes/BayesNet.h>
#include <weka/classifiers/bayes/net/BayesNetEstimator.h>
#include <stdexcept>

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace std;

BayesNetEstimator::~BayesNetEstimator ()
{
}

void
BayesNetEstimator::estimateCPTs (BayesNet * bayesNet)
{
	throw new runtime_error("not implemented");
}

void
BayesNetEstimator::updateClassifier (BayesNet * bayesNet, Instance * instance)
{
	throw new runtime_error("not implemented");
}

vector<double>
BayesNetEstimator::distributionForInstance (BayesNet * bayesNet,
	Instance * instance)
{
	vector<double> v;
	throw new runtime_error("not implemented");
	return v;
}

void
BayesNetEstimator::initCPTs (BayesNet * bayesNet)
{
	throw new runtime_error("not implemented");
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
