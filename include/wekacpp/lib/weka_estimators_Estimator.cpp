/*
 * Derived from:
 *
 * Estimator.java
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
#include <weka/estimators/IncrementalEstimator.h>
#include <cmath>
#include <stdexcept>

namespace weka {
	namespace estimators {

using namespace std;

Estimator::~Estimator()
{
}

int
Estimator::getMinMax (Instances * instances, int attrIndex, double minMax[2]) 
{
	int numNotMissing = 0, i;
	double min = NAN, max = NAN;
	for (i = 0; i < instances->numInstances(); i++) {
		Instance * instance = instances->instance(i);
		if (instance->isMissing(attrIndex))
			continue;
		min = instances->instance(i)->value(attrIndex);
		max = instances->instance(i)->value(attrIndex);
		numNotMissing++;
	}
	for (; i < instances->numInstances(); i++) {
		Instance * instance = instances->instance(i);
		if (!instance->isMissing(attrIndex)) {
			numNotMissing++;
			if (instance->value(attrIndex) < min) {
				min = instance->value(attrIndex);
			} else {
				if (instance->value(attrIndex) > max) {	      
					max = instance->value(attrIndex);
				}
			}
		}
	}
	minMax[0] = min;
	minMax[1] = max;
	return numNotMissing;
}

double
Estimator::getInstancesFromClass(Instances * data, int attrIndex,
	int classIndex, double classValue, Instances * workData)
{
	int num = 0;
	int numClassValue = 0;
	for (int i = 0; i < data->numInstances(); i++) {
		if (!data->instance(i)->isMissing(attrIndex)) {
			num++;
			if (data->instance(i)->value(classIndex) == classValue) {
				workData->add(data->instance(i));
				numClassValue++;
			}
		}
	} 
	return (double)numClassValue / (double)num;
}

void
Estimator::buildEstimator(Estimator * est, Instances * instances,
	int attrIndex, int classIndex, int classValueIndex, bool isIncremental)
{
	// non-incremental estimator add all instances at once
	if (!isIncremental) {
		if (classValueIndex == -1) {
			est->addValues(instances, attrIndex);
		} else {
			est->addValues(instances, attrIndex, classIndex, 
					classValueIndex);
		}
	} else {
		// incremental estimator, read one value at a time
		for (int i = 0; i < instances->numInstances(); i++) {
			Instance * instance = instances->instance(i);
			((IncrementalEstimator *)est)->addValue(instance->value(attrIndex),
				instance->weight());
		}
	}
}

void
Estimator::addValues(Instances * data, int attrIndex, double min, double max,
	double factor)
{
	// no handling of factor, would have to be overridden
	// no handling of min and max, would have to be overridden
	int numInst = data->numInstances();
	for (int i = 1; i < numInst; i++)
		addValue(data->instance(i)->value(attrIndex), 1.0);
}

void
Estimator::addValues(Instances * data, int attrIndex)
{
	double minMax[2];
	getMinMax(data, attrIndex, minMax);
	// factor is 1.0, data set has not been reduced
	addValues(data, attrIndex, minMax[0], minMax[1], 1.0);
}

void
Estimator::addValues (Instances * data, int attrIndex, int classIndex,
	int classValue)
{
	// can estimator handle the data?
	m_noClass = false;    
	double minMax[2];
	getMinMax(data, attrIndex, minMax);
	double min = minMax[0];
	double max = minMax[1];
	// extract the instances with the given class value
	Instances * workData = new Instances(data);
	double factor = 
		getInstancesFromClass(data, attrIndex, classIndex, 
			(double)classValue, workData);
	// if no data return
	if (workData->numInstances() == 0) {
		delete workData;
		return;
	}
	delete workData;
	addValues(data, attrIndex, min, max, factor);
}

void
Estimator::addValues(Instances * data, int attrIndex, int classIndex,
	int classValue, double min, double max)
{
	// extract the instances with the given class value
	Instances * workData = new Instances(data);
	double factor =
		getInstancesFromClass(data, attrIndex, classIndex, 
			(double)classValue, workData);
	// if no data return
	if (workData->numInstances() == 0) {
		delete workData;
		return;
	}
	delete workData;
	addValues(data, attrIndex, min, max, factor);
}

double
Estimator::logScore(int nType)
{
	throw new runtime_error("not implemented");
	return NAN;
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

