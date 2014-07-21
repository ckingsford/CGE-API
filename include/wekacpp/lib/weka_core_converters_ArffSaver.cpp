/*
 * Derived from:
 *
 * ArffSaver.java
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

#include <weka/core/Attribute.h>
#include <weka/core/Instance.h>
#include <weka/core/Instances.h>
#include <weka/core/converters/ArffSaver.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <streambuf>

namespace weka {
	namespace converters {

using namespace std;

void
ArffSaver::writeHeader (Instances * header)
{
	m_Stream->sputn("@relation ", 10);
	string relation = header->relationName();
	m_Stream->sputn(relation.c_str(), relation.size());
	m_Stream->sputn("\n\n", 2);
	for (int i = 0; i < header->numAttributes(); i++) {
		m_Stream->sputn("@attribute ", 11); 

		Attribute * attr = header->attribute(i);

		switch (attr->type()) {

		case Attribute::NUMERIC: {
			string name = attr->name();
			m_Stream->sputn(name.c_str(), name.size());
			m_Stream->sputn(" numeric", 8);
			if ((((NumericAttribute *)attr)->getLowerBound() != NAN &&
				 ((NumericAttribute *)attr)->getLowerBound() != -INFINITY) &&
				(((NumericAttribute *)attr)->getUpperBound() != NAN &&
				 ((NumericAttribute *)attr)->getUpperBound() != INFINITY))
			{
				char buf[40];
				m_Stream->sputc(' ');
				if (((NumericAttribute *)attr)->lowerBoundIsOpen())
					m_Stream->sputc('(');
				else
					m_Stream->sputc('[');
				sprintf(buf, "%lf", 
					((NumericAttribute *)attr)->getLowerBound());
				m_Stream->sputn(buf, strlen(buf));
				m_Stream->sputc(',');
				sprintf(buf, "%lf", 
					((NumericAttribute *)attr)->getUpperBound());
				m_Stream->sputn(buf, strlen(buf));
				if (((NumericAttribute *)attr)->upperBoundIsOpen())
					m_Stream->sputc(')');
				else
					m_Stream->sputc(']');
			}
		}	break;

		case Attribute::NOMINAL: {
			string name = attr->name();
			bool first = true;
			m_Stream->sputn(name.c_str(), name.size());
			m_Stream->sputn(" {", 2);
			for (int j = 0; j < attr->numValues(); j++) {
				if (!first) {
					m_Stream->sputc(',');
				} else {
					first = false;
				}
				string val = attr->value(j);
				m_Stream->sputn(val.c_str(), val.size());
			}
			m_Stream->sputn("}", 1);
		}	break;

		case Attribute::STRING: {
			string name = attr->name();
			m_Stream->sputn(name.c_str(), name.size());
			m_Stream->sputn(" string", 7);
		}	break;

		default:
			cerr << __FUNCTION__ << ": unhandled attribute (" << i << ")\n";
			return;
		}

		m_Stream->sputc('\n');
	}
	m_Stream->sputn("\n@data\n\n", 8);
	m_bHeaderWritten = true;
}

ArffSaver::ArffSaver (streambuf * s)
{
	m_Stream = s;
}

ArffSaver::~ArffSaver()
{
}

void
ArffSaver::writeInstance (Instance * instance)
{
	for (int i = 0; i < instance->numAttributes(); i++) {
		Attribute * attr = instance->attribute(i);
		if (i != 0)
			m_Stream->sputc(',');

		switch (attr->type()) {

		case Attribute::NUMERIC: {
			char buf[40];
			sprintf(buf, "%lf", instance->value(i));
			m_Stream->sputn(buf, strlen(buf));
		}	break;

		case Attribute::NOMINAL: {
			string val = attr->value((int)instance->value(i));
			m_Stream->sputn(val.c_str(), val.size());
		}	break;

		case Attribute::STRING: {
			string val = attr->value((int)instance->value(i));
			m_Stream->sputc('"');
			m_Stream->sputn(val.c_str(), val.size());
			m_Stream->sputc('"');
		}	break;

		}
	}

	m_Stream->sputc('\n');
}

void
ArffSaver::writeIncremental (Instance * instance)
{
	if (!m_bHeaderWritten)
		writeHeader(instance->dataset());
	writeInstance(instance);
}

void
ArffSaver::writeBatch (Instances * instances)
{
	if (!m_bHeaderWritten)
		writeHeader(instances);
	for (int i = 0; i < instances->numInstances(); i++)
		writeInstance(instances->instance(i));
}

} // namespace converters
} // namespace weka

// Local variables:
//  mode: C++
//  c-indent-level: 4
//  c-basic-offset: 4
//  tab-width: 4
// End:
//
// vim:tabstop=4:shiftwidth=4:smarttab:
