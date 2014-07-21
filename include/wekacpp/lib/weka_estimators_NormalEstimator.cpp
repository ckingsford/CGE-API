/*
 * Derived from:
 *
 * NormalEstimator.java
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

#include <weka/estimators/Estimator.h>
#include <weka/estimators/NormalEstimator.h>
#include <cmath>
#include <stdexcept>

#include "utils.h"

namespace weka {
	namespace estimators {

using namespace std;

NormalEstimator::~NormalEstimator()
{
}

void
NormalEstimator::addValue (double data, double weight)
{
    if (weight == 0)
		return;
    data = round(data);
    m_SumOfWeights += weight;
    m_SumOfValues += data * weight;
    m_SumOfValuesSq += data * data * weight;
    if (m_SumOfWeights > 0) {
		m_Mean = m_SumOfValues / m_SumOfWeights;
		double stdDev =
			sqrt(abs(m_SumOfValuesSq - m_Mean * m_SumOfValues) /
						m_SumOfWeights);
		// If the stdDev ~= 0, we really have no idea of scale yet, 
		// so stick with the default. Otherwise...
		if (stdDev > 1e-10) {
			if (m_Precision / (2.0 * 3.0) > stdDev) {
				m_StandardDev = m_Precision / (2.0 * 3.0);
			} else {
				m_StandardDev = stdDev;
			}
		}
	}
}

double
NormalEstimator::getProbability (double data)
{
    data = round(data);
    double zLower = (data - m_Mean - (m_Precision / 2.0)) / m_StandardDev;
    double zUpper = (data - m_Mean + (m_Precision / 2.0)) / m_StandardDev;
    double pLower = normalProbability(zLower);
    double pUpper = normalProbability(zUpper);
    return pUpper - pLower;
}

} // namespace estimators
} // namespace weka

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:

