/*
 * Derived from:
 *
 * BayesNet.java
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

#include <weka/core/Debug.h>
#include <weka/core/Instance.h>
#include <weka/core/Instances.h>
#include <weka/classifiers/Classifier.h>
#include <weka/classifiers/bayes/net/BayesNetEstimator.h>
#include <weka/classifiers/bayes/net/SimpleEstimator.h>
#include <weka/classifiers/bayes/net/DiscreteEstimatorBayes.h>
#include <weka/classifiers/bayes/net/SearchAlgorithm.h>
#include <weka/classifiers/bayes/net/local/K2.h>
#include <weka/classifiers/bayes/BayesNet.h>
#include <weka/filters/Filter.h>
#include <weka/filters/supervised/attribute/Discretize.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

#ifdef HAVE_XERCESC
# include <xercesc/dom/DOM.hpp>
# include <xercesc/parsers/XercesDOMParser.hpp>
# include <xercesc/sax/InputSource.hpp>
# include <xercesc/util/BinInputStream.hpp>
# include <xercesc/util/XMLString.hpp>
# include <xercesc/util/PlatformUtils.hpp>
#endif

namespace {

using namespace std;

/* xmlnorm converts the five standard XML entities in a string
   e.g. the string V&D's is returned as V&amp;D&apos;s */
string
xmlnorm (string s1) {
    string s2;
    for (int i = 0; i < (int)s1.length(); i++) {
		char c = s1[i];
		switch (c) {
		case '&':	s2.append("&amp;");		break;
		case '\'':	s2.append("&apos;");	break;
		case '\"':	s2.append("&quot;");	break;
		case '<':	s2.append("&lt;");		break;
		case '>':	s2.append("&gt;");		break;
		default:
			s2 += c;
			break;
		}
	}
	return s2;
}

}

namespace weka {
	namespace classifiers {
		namespace bayes {

using namespace std;
using namespace weka::estimators;
using namespace weka::filters;

#ifdef HAVE_XERCESC

XERCES_CPP_NAMESPACE_USE;

class StreamInputStream : public BinInputStream
{
	streambuf * s;
	streamsize pos;

 public:
	StreamInputStream(streambuf * s)
	{
		this->s = s;
	}

    bool
	getIsOpen() const
	{
		return true;
	}

    unsigned int
	getSize() const
	{
		// NOTE: returning 0 won't work if someone actually calls getSize
		return 0;
	}

    void
	reset()
	{
	}

	unsigned int
	curPos() const
	{
		return pos;
	}

	unsigned int
	readBytes (XMLByte* const toFill, const unsigned int maxToRead)
	{
		streamsize nrRead = s->sgetn((char *)toFill, maxToRead);
		pos += nrRead;
		return nrRead;
	}

};

class StreamInputSource : public InputSource
{
	streambuf * s;

 public:
    StreamInputSource (streambuf * s,
        MemoryManager* const manager = XMLPlatformUtils::fgMemoryManager)
	{
		this->s = s;
		XMLCh * name = (XMLCh*) manager->allocate(40);
		char buf[40];
		sprintf(buf, "XMLBIF03@%p", s);
		XMLString::transcode(buf, name, 39);
        setSystemId(name);
	}

	BinInputStream *
	makeStream() const
	{
		return new StreamInputStream(s);
	}
};

static BayesNet *
readXMLBIF03 (streambuf * s)
{
	static XMLCh xmlBIF[4];
	static XMLCh xmlDEFINITION[11];
	static XMLCh xmlFOR[4];
	static XMLCh xmlGIVEN[6];
	static XMLCh xmlNAME[5];
	static XMLCh xmlNETWORK[8];
	static XMLCh xmlOUTCOME[8];
	static XMLCh xmlTABLE[6];
	static XMLCh xmlVARIABLE[9];
	static int init;

	if (!init) {
		XMLPlatformUtils::Initialize();
		// set up useful constants
		XMLString::transcode("BIF", xmlBIF, 3);
		XMLString::transcode("DEFINITION", xmlDEFINITION, 10);
		XMLString::transcode("FOR", xmlFOR, 3);
		XMLString::transcode("GIVEN", xmlGIVEN, 5);
		XMLString::transcode("NAME", xmlNAME, 4);
		XMLString::transcode("NETWORK", xmlNETWORK, 7);
		XMLString::transcode("OUTCOME", xmlOUTCOME, 7);
		XMLString::transcode("TABLE", xmlTABLE, 5);
		XMLString::transcode("VARIABLE", xmlVARIABLE, 8);
		init = 1;
	}

	// build DOM model of XMLBIF input

	XercesDOMParser parser;
	parser.setValidationScheme(XercesDOMParser::Val_Never);
	parser.setDoSchema(false) ;
	parser.setLoadExternalDTD(false) ;
	StreamInputSource is(s);
	parser.parse(is);

	DOMDocument * document = parser.getDocument();

	DOMNodeList * nodeList;
	DOMElement * element, * networkElement;

	// create network

	// BIF
	nodeList = document->getElementsByTagName(xmlBIF);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
		throw new runtime_error("cannot find BIF");
	// BIF.NETWORK
	nodeList = element->getElementsByTagName(xmlNETWORK);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
		throw new runtime_error("cannot find BIF.NETWORK");
	networkElement = element;
	// BIF.NETWORK.NAME
	nodeList = networkElement->getElementsByTagName(xmlNAME);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
		throw new runtime_error("cannot find BIF.NETWORK.NAME");
	// text for BIF.NETWORK.NAME
	char * cp = NULL;
	nodeList = element->getChildNodes();
	for (int i = 0; i < (int)nodeList->getLength(); i++) {
		DOMNode * node = nodeList->item(i);
		if (node->getNodeType() == DOMNode::TEXT_NODE) {
			cp = XMLString::transcode(node->getNodeValue());
			break;
		}
	}
	if (cp == NULL)
		throw new runtime_error("cannot get text for BIF.NETWORK.NAME");

   	string networkName(cp);
   	XMLString::release(&cp);

	BayesNet * net = new BayesNet(networkName);

	Debug.dprintf("%s: reading BayesNet '%s'\n", "BayesNet::readXMLBIF03",
		networkName.c_str());

	// build attributes

	// BIF.NETWORK.VARIABLE[]
 	vector<Attribute *> attrs;
 	Attribute * attr;
	nodeList = networkElement->getElementsByTagName(xmlVARIABLE);
 	for (int i = 0; i < (int)nodeList->getLength(); i++) {
		// BIF.NETWORK.VARIABLE[].NAME
		DOMElement * variableElement = 
			dynamic_cast<DOMElement *>(nodeList->item(i));
		if (!variableElement)
			continue;
		DOMElement * nameElement =
			dynamic_cast<DOMElement *>
				(variableElement->getElementsByTagName(xmlNAME)->item(0));
		// text for BIF.NETWORK.VARIABLE[].NAME
		cp = NULL;
		for (int j = 0; 
				j < (int)nameElement->getChildNodes()->getLength(); 
				j++)
		{
			DOMNode * node = nameElement->getChildNodes()->item(j);
			if (node->getNodeType() == DOMNode::TEXT_NODE) {
				cp = XMLString::transcode(node->getNodeValue());
				break;
			}
		}
		if (cp == NULL)
			throw new runtime_error("cannot find text for "
				"BIF.NETWORK.VARIABLE[].NAME");
		string name(cp);
		XMLString::release(&cp);

		attr = new NominalAttribute(name);

		Debug.dprintf("\tadded attribute '%s'\n", name.c_str());

		// BIF.NETWORK.VARIABLE[].OUTCOME[]
		DOMNodeList * outcomeNodeList =
			variableElement->getElementsByTagName(xmlOUTCOME);
		for (int j = 0; j < (int)outcomeNodeList->getLength(); j++) {
			DOMElement * outcomeElement = 
				dynamic_cast<DOMElement *>(outcomeNodeList->item(j));
			if (outcomeElement == NULL)
				continue;
			// value for BIF.NETWORK.VARIABLE[].OUTCOME[]
			cp = NULL;
			for (int k = 0; 
					k < (int)outcomeElement->getChildNodes()->getLength();
					k++) 
			{
				DOMNode * node = outcomeElement->getChildNodes()->item(k);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"BIF.NETWORK.VARIABLE[].OUTCOME[]");
			string * value = new string(cp);
			XMLString::release(&cp);

			attr->addValue(value);

			Debug.dprintf("\t\tadded outcome '%s'\n", value->c_str());
		}

		attrs.push_back(attr);
	}

	// create instances set
		// (we need this for its incorporated attribute definitions)

	Instances * instances = new Instances(networkName, attrs);
	// set last attribute as class attribute
	instances->setClassIndex(instances->numAttributes() - 1);
	net->setInstances(instances);
	net->initStructure();

	// create parent sets

	// BIF.NETWORK.DEFINITION[]
	nodeList = networkElement->getElementsByTagName(xmlDEFINITION);
 	for (int i = 0; i < (int)nodeList->getLength(); i++) {
		DOMElement * defElement = 
			dynamic_cast<DOMElement *>(nodeList->item(i));
		if (!defElement)
			continue;
		// BIF.NETWORK.DEFINITION[].FOR
		DOMElement * forElement =
			dynamic_cast<DOMElement *>
				(defElement->getElementsByTagName(xmlFOR)->item(0));
		if (forElement == NULL)
			throw new runtime_error("cannot find "
				"BIF.NETWORK.DEFINITION[].FOR");
		// text for BIF.NETWORK.DEFINITION[].FOR
		cp = NULL;
		for (int j = 0; 
				j < (int)forElement->getChildNodes()->getLength(); 
				j++)
		{
			DOMNode * node = forElement->getChildNodes()->item(j);
			if (node->getNodeType() == DOMNode::TEXT_NODE) {
				cp = XMLString::transcode(node->getNodeValue());
				break;
			}
		}
		if (cp == NULL)
			throw new runtime_error("cannot find text for "
				"BIF.NETWORK.DEFINITION[].FOR");
		string name(cp);
		XMLString::release(&cp);
		int nNode = -1;
		for (int j = 0; j < instances->numAttributes(); j++) {
			if (instances->attribute(j)->name() == name) {
				nNode = j;
				break;
			}
		}
		if (nNode == -1)
			throw new runtime_error("cannot resolve attribute for "
 				"DEFINITION");

		// resolve parent identities
		// BIF.NETWORK.DEFINITION[].GIVEN[]
		DOMNodeList * givenNodeList = 
			defElement->getElementsByTagName(xmlGIVEN);
		for (int j = 0; j < (int)givenNodeList->getLength(); j++) {
			DOMElement * givenElement = 
				dynamic_cast<DOMElement *>(givenNodeList->item(j));
			if (givenElement == NULL)
				continue;
			// text for BIF.NETWORK.DEFINITION[].GIVEN[]
			for (int k = 0; 
					k < (int)givenElement->getChildNodes()->getLength();
					k++)
			{
				DOMNode * node = givenElement->getChildNodes()->item(k);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot get text for "
					"BIF.NETWORK.DEFINITION[].GIVEN[]");
			string name(cp);
			XMLString::release(&cp);
			int nParent = -1;
			for (int k = 0; k < instances->numAttributes(); k++) {
				if (instances->attribute(k)->name() == name) {
					nParent = k;
					break;
				}
			}
			if (nParent == -1)
				throw new runtime_error("cannot identify attribute for "
					"DEFINITION.GIVEN");

			net->getParentSets()->at(nNode)
				-> addParent(nParent, net->getInstances());

			Debug.dprintf("\tadded parent relation %d -> %d\n", nNode,
				nParent);
		}

		// build conditional probability table

		int nCardinality = net->getParentSets()->at(nNode)
				-> getCardinalityOfParents();
		int nValues = instances->attribute(nNode)->numValues();
		if ((int)net->getDistributions()->size() <= nNode)
			net->getDistributions()->resize(nNode + 1);
		if ((int)net->getDistributions()->at(nNode).size() <= nCardinality)
			net->getDistributions()->at(nNode).resize(nCardinality);
		for (int j = 0; j < nCardinality; j++)
			net->getDistributions()->at(nNode).at(j) = 
				new DiscreteEstimatorBayes(nValues, 0.0);

		// BIF.NETWORK.DEFINITION[].TABLE

		DOMElement * tableElement = 
			dynamic_cast<DOMElement *>
				(defElement->getElementsByTagName(xmlTABLE)->item(0));
		if (tableElement == NULL)
			throw new runtime_error("cannot get TABLE for DEFINITION");
		for (int j = 0; 
				j < (int)tableElement->getChildNodes()->getLength(); 
				j++)
		{
			DOMNode * node = tableElement->getChildNodes()->item(j);
			if (node->getNodeType() == DOMNode::TEXT_NODE) {
				cp = XMLString::transcode(node->getNodeValue());
				break;
			}
		}
		if (cp == NULL)
			throw new runtime_error("cannot find text for "
				"BIF.NETWORK.DEFINITION[].FOR");
		char * table = cp;
		for (int j = 0; j < nCardinality; j++) {
			DiscreteEstimatorBayes * est = (DiscreteEstimatorBayes *)
				net->getDistributions()->at(nNode).at(j);
			if (est == NULL)
				throw new runtime_error("missing estimator");
			for (int k = 0; k < nValues; k++) {
				char buf[40];
				int l;
				double weight;
				// get next weight token from table text
				while (isspace(*cp) && *cp)
					cp++;
				if (!*cp)
					throw new runtime_error("truncated TABLE for DEFINITION");
				for (l = 0; !isspace(*cp) && *cp && l < 40; l++) {
					buf[l] = *cp;
					cp++;
				}
				if (l == 40)
					throw new runtime_error("entry too long in TABLE"
						" for DEFINITION");
				buf[l] = 0;
				weight = strtod(buf, NULL);

				est->addValue(k, weight);
			}
		}
		XMLString::release(&table);
	}

	return net;
}

BayesNet *
BayesNet::fromXMLBIF03 (streambuf * s)
{
	// test for '<' at start of stream, iterating through available
	// filters, until successful
	s->pubseekoff(0, ios_base::beg);
	istream is(s);
	if (is.get() == (int)'<') {
		is.seekg(0, ios_base::beg);
		return readXMLBIF03(s);
	}
	// no filters available at this time
	throw new runtime_error("unrecongized format");
	return NULL; // NOTREACHED
}

BayesNet *
BayesNet::fromXMLBIF03 (streambuf& s)
{
	return fromXMLBIF03(&s);
}

BayesNet *
BayesNet::fromXMLBIF03 (string& xml)
{
	stringbuf s(xml);
	return fromXMLBIF03(&s);
}

#else /* ! HAVE_XERCESC */

BayesNet *
BayesNet::fromXMLBIF03 (streambuf * s)
{
	throw new runtime_error("not implemented");
	return NULL; // NOTREACHED
}

BayesNet *
BayesNet::fromXMLBIF03 (streambuf& s)
{
	throw new runtime_error("not implemented");
	return NULL; // NOTREACHED
}

BayesNet *
BayesNet::fromXMLBIF03 (string& xml)
{
	throw new runtime_error("not implemented");
	return NULL; // NOTREACHED
}

#endif /* HAVE_XERCESC */

BayesNet::BayesNet (string& name) :
	Classifier(name),
	m_SearchAlgorithm(NULL),
	m_BayesNetEstimator(NULL),
	m_Instances(NULL)
{
}

BayesNet::BayesNet (string * name) :
	Classifier(*name),
	m_SearchAlgorithm(NULL),
	m_BayesNetEstimator(NULL),
	m_Instances(NULL)
{
}

BayesNet::~BayesNet ()
{
	if (m_SearchAlgorithm != NULL)
		delete m_SearchAlgorithm;
	if (m_BayesNetEstimator != NULL)
		delete m_BayesNetEstimator;
}

Instances *
BayesNet::normalizeDataSet (Instances * instances)
{
	bool bHasNonNominal = false;
	bool bHasMissingValues = false;

	for (int i = 0; i < instances->numAttributes(); i++) {
		Attribute * attr = instances->attribute(i);
		if (attr->type() != Attribute::NOMINAL)
			bHasNonNominal = true;
		for (int i = 0; i < instances->numInstances(); i++) {
			Instance * instance = instances->instance(i);
			if (instance->isMissing(attr->index()))
				bHasMissingValues = true;
		}
	}

	if (bHasMissingValues) {
		// fill in missing values in data set
		// See weka/filters/unsupervised/attribute/ReplaceMissingValues.java
		// for implementation advice. Perhaps a port of this class and 
		// supporting classes would be worth the effort.
		// "Replaces all missing values for nominal and numeric attributes
		//  in a dataset with the modes and means from the training data."
		throw new runtime_error("data set has missing value");
	}

	if (bHasNonNominal) {
		Debug.dprintf("%s: relation '%s' contains numeric attributes\n",
			"BayesNet::normalizeDataSet", instances->relationName().c_str());

		// create output format by replacing numeric attributes with
		// nominal ones
		Instances * outFmt = new Instances(instances);
		if (outFmt == NULL)
			throw new runtime_error("could not create output format");
		for (int i = 0; i < outFmt->numAttributes(); i++) {
			Attribute * attr = outFmt->attribute(i);
			if (attr->type() != Attribute::NUMERIC)
				continue;
			string name(attr->name());
			outFmt->setAttribute(i, new NominalAttribute(name));
		}

		// discretize data set
		Discretize * filter = new Discretize();
		if (filter == NULL)
			throw new runtime_error("could not create Discretize filter");
		filter->setOutputFormat(outFmt);
		instances = filter->useFilter(instances);
		delete filter;

		// delete output format
		delete outFmt;
	}

	return instances;
}

Instance *
BayesNet::normalizeInstance (Instance * instance)
{
	Instance * newInstance;
	for (int i = 0; i < instance->numAttributes(); i++) {
		Attribute * attr = instance->attribute(i);
		if (attr->type() == Attribute::NUMERIC) {
			// convert attribute to nominal type, using bins either
			// previously established by normalizeInstances() or learned
			// from parse of XMLBIF source
			NominalAttribute * nomAttr = 
				dynamic_cast<NominalAttribute *>(m_Instances->attribute(i));
			if (nomAttr == NULL)
				throw new runtime_error("internal attribute set contains "
					"unexpected non-nominal attribute");
			bool ok = false;
			newInstance = new Instance(instance);
			for (int j = 0; j < nomAttr->numValues(); j++) {
				if (nomAttr->isInRange(j, instance->value(i))) {
					newInstance->setValue(i, (double)j);
					ok = true;
					break;
				}
			}
			if (!ok) {
				delete newInstance;
				throw new runtime_error("unable to find range for non-"
					"nominal attribute");
			}
			instance = newInstance;
		} else if (attr->type() != Attribute::NOMINAL) {
			throw new runtime_error("instance has non-nominal attribute");
		}
		attr = instance->attribute(i);
		if (instance->isMissing(attr->index())) {
			// See weka/filters/unsupervised/attribute/
			// ReplaceMissingValues.java for implementation advice. Perhaps
			// a port of this class and supporting classes would be worth
			// the effort.
			// "Replaces all missing values for nominal and numeric
			// attributes in a dataset with the modes and means from the
			// training data."
			throw new runtime_error("instance has missing value");
		}
	}

	return instance;
}

void
BayesNet::initStructure()
{
	m_ParentSets.resize(m_Instances->numAttributes());
	for (int i = 0; i < m_Instances->numAttributes(); i++)
		m_ParentSets[i] = new ParentSet(m_Instances->numAttributes());
}

void
BayesNet::buildStructure()
{
	Debug.dprintf("%s: finding best structure for relation '%s'\n",
		"BayesNet::buildStructure", m_Instances->relationName().c_str());
	m_SearchAlgorithm->buildStructure(this, m_Instances);
}

void
BayesNet::buildClassifier (Instances * instances)
{
	Instances * newInstances;

	// copy and process instance set

	instances = new Instances(instances);
	instances->deleteWithMissingClass();
	newInstances = normalizeDataSet(instances);
	if (newInstances != instances) {
		delete instances;
		instances = newInstances;
	}
	m_Instances = instances;
	m_NumClasses = instances->numClasses();

	// build the network structure
	initStructure();
	buildStructure();

	// build the set of CPTs
	estimateCPTs();
}

void
BayesNet::updateClassifier (Instance * instance)
{
	Instance * newInstance;
	newInstance = normalizeInstance(instance);
	if (newInstance != instance) {
		delete instance;
		instance = newInstance;
	}
	m_BayesNetEstimator->updateClassifier(this, instance);
} 

void
BayesNet::initCPTs()
{
	m_BayesNetEstimator->initCPTs(this);
}

void
BayesNet::estimateCPTs()
{
	Debug.dprintf("%s: estimating CPTs for relation '%s'\n", 
		"BayesNet::estimateCPTs", m_Instances->relationName().c_str());
	m_BayesNetEstimator->estimateCPTs(this);
}

vector<double>
BayesNet::distributionForInstance (Instance * instance)
{
	Instance * newInstance = normalizeInstance(instance);
	vector<double> v =
		m_BayesNetEstimator->distributionForInstance(this, newInstance);
	if (newInstance != instance)
		delete newInstance;
	return v;
}

vector<double>
BayesNet::countsForInstance (Instance * instance)
{
	vector<double> fCounts(m_NumClasses);
	for (int iClass = 0; iClass < m_NumClasses; iClass++)
		fCounts[iClass] = 0.0;
	for (int iClass = 0; iClass < m_NumClasses; iClass++) {
		double fCount = 0;
		for (int iAttribute = 0; 
			 iAttribute < m_Instances->numAttributes();
			 iAttribute++)
		{
			double iCPT = 0;
			for (int iParent = 0;
				 iParent < m_ParentSets[iAttribute]->getNrOfParents();
				 iParent++)
			{
				int nParent = m_ParentSets[iAttribute]->getParent(iParent);
                    if (nParent == m_Instances->classIndex()) {
                        iCPT = iCPT * m_NumClasses + iClass;
                    } else {
                        iCPT = iCPT *
							m_Instances->attribute(nParent)->numValues() +
								instance->value(nParent);
                    }
                }
			DiscreteEstimatorBayes * est = (DiscreteEstimatorBayes *)
				m_Distributions[iAttribute][(int)iCPT];
			if (iAttribute == m_Instances->classIndex()) {
				fCount += est->getCount(iClass);
			} else {
				fCount += est->getCount(instance->value(iAttribute));
			}
		}
		
		fCounts[iClass] += fCount;
	}
	return fCounts;
}

void
BayesNet::toDOT(streambuf * s)
{
	const char * name, * attr, * parent;
	name = m_Instances->relationName().c_str();
	s->sputn("digraph ", 8);
	s->sputn(name, strlen(name));
	s->sputn(" {\n", 3);
	for (int i = 0; i < m_Instances->numAttributes(); i++) {
		attr = m_Instances->attribute(i)->name().c_str();
		for (int j = 0; j < m_ParentSets[i]->getNrOfParents(); j++) {
			parent = m_Instances->attribute(m_ParentSets[i]->getParent(j))
				-> name().c_str();
			s->sputn(attr, strlen(attr));
			s->sputn(" -> ", 4);
			s->sputn(parent, strlen(parent));
		}
	}
	s->sputn("}\n", 3);
}

void
BayesNet::toDOT(streambuf& s)
{
	toDOT(&s);
}

void
BayesNet::toDOT(string& xml)
{
	stringbuf s(xml);
	toDOT(s);
}

void
BayesNet::XMLBIF03DTD(streambuf * s)
{
	static string s1(
		"<?xml version=\"1.0\"?>\n"
		"<!DOCTYPE BIF [\n"
		"\t<!ELEMENT BIF ( NETWORK )*>\n"
		"\t\t<!ATTLIST BIF VERSION CDATA #REQUIRED>\n"
		"\t<!ELEMENT NETWORK (NAME, ( PROPERTY | VARIABLE | DEFINITION )*)>\n"
		"\t<!ELEMENT NAME (#PCDATA)>\n"
		"\t<!ELEMENT VARIABLE ( NAME, ( OUTCOME |  PROPERTY )* ) >\n"
		"\t\t<!ATTLIST VARIABLE TYPE (nature|decision|utility) \"nature\">\n"
		"\t<!ELEMENT OUTCOME (#PCDATA)>\n"
		"\t<!ELEMENT DEFINITION ( FOR | GIVEN | TABLE | PROPERTY )* >\n"
		"\t<!ELEMENT FOR (#PCDATA)>\n"
		"\t<!ELEMENT GIVEN (#PCDATA)>\n"
		"\t<!ELEMENT TABLE (#PCDATA)>\n"
		"\t<!ELEMENT PROPERTY (#PCDATA)>\n"
		"]>\n\n");
	s->sputn(s1.c_str(), s1.length());
}

void
BayesNet::XMLBIF03DTD(streambuf& s)
{
	XMLBIF03DTD(&s);
}

void
BayesNet::XMLBIF03DTD(string& xml)
{
	stringbuf s(xml);
	XMLBIF03DTD(s);
}

void
BayesNet::toXMLBIF03(streambuf * s)
{
	char buf[128];

	Debug.dprintf("%s: writing BayesNet '%s'\n", "BayesNet::toXMLBIF03",
		m_Instances->relationName().c_str());

	s->sputn(
		"<BIF VERSION=\"0.3\">\n"
		"<NETWORK>\n"
		"<NAME>",
		36);
	string s1 = xmlnorm(m_Instances->relationName());
	s->sputn(s1.c_str(), s1.length());
	s->sputn("</NAME>\n", 8);

	for (int i = 0; i < m_Instances->numAttributes(); i++) {
		Attribute * attr = m_Instances->attribute(i);
		s->sputn(
			"<VARIABLE TYPE=\"nature\">\n"
			"<NAME>",
			31);
		string s2 = xmlnorm(attr->name());
		s->sputn(s2.c_str(), s2.length());
		s->sputn("</NAME>\n", 8);
		Debug.dprintf("\tattribute '%s'\n", attr->name().c_str());
		for (int j = 0; j < m_Instances->attribute(i)->numValues(); j++) {
			s->sputn("<OUTCOME>", 9);
			string s3 = xmlnorm(attr->value(j));
			s->sputn(s3.c_str(), s3.length());
			s->sputn("</OUTCOME>\n", 11);
		Debug.dprintf("\t\toutcome '%s'\n", attr->name().c_str());
		}
		s->sputn("</VARIABLE>\n", 12);
	}

	for (int i = 0; i < m_Instances->numAttributes(); i++) {
		s->sputn(
			"<DEFINITION>\n"
			"<FOR>",
			18);
		string s3 = xmlnorm(m_Instances->attribute(i)->name());
		s->sputn(s3.c_str(), s3.length());
		s->sputn("</FOR>\n", 7);
		for (int j = 0; j < m_ParentSets[i]->getNrOfParents(); j++) {
			s->sputn("<GIVEN>", 7);
			string s4 = 
				xmlnorm(m_Instances->attribute(m_ParentSets[i]->getParent(j))
					-> name());
			s->sputn(s4.c_str(), s4.length());
			s->sputn("</GIVEN>\n", 9);

			Debug.dprintf("\tparent relation %d -> %d\n", i,
				m_ParentSets[i]->getParent(j));
		}
		s->sputn("<TABLE>\n", 8);
		for (int j = 0; j < m_ParentSets[i]->getCardinalityOfParents(); j++) {
			for (int k = 0; k < m_Instances->attribute(i)->numValues(); k++) {
				s->sputn(buf, 
					sprintf(buf, "%0.16lf ", 
						m_Distributions[i][j]->getProbability(k)));
			}
			s->sputn("\n", 1);
		}
		s->sputn(
			"</TABLE>\n"
			"</DEFINITION>\n",
			23);
	}
	s->sputn(
		"</NETWORK>\n"
		"</BIF>\n",
		18);
}

void
BayesNet::toXMLBIF03(streambuf& s)
{
	toXMLBIF03(&s);
}

void
BayesNet::toXMLBIF03(string& xml)
{
	stringbuf s(xml);
	toXMLBIF03(s);
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
