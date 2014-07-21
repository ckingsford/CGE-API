/*
 * Derived from:
 *
 * NaiveBayes.java
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
#include <weka/estimators/DiscreteEstimator.h>
#include <weka/estimators/NormalEstimator.h>
#include <weka/classifiers/bayes/NaiveBayes.h>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <vector>

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

static NaiveBayes *
readXML (streambuf * s)
{
	static XMLCh xmlNBIF[5];
	static XMLCh xmlNB[3];
	static XMLCh xmlNAME[5];
	static XMLCh xmlNUMCLASSES[11];
	static XMLCh xmlNUMATTRIBUTES[14];
	static XMLCh xmlDISTRIBS[9];
	static XMLCh xmlCLASSDISTRIB[13];
	static XMLCh xmlNORMALESTIMATOR[16];
	static XMLCh xmlSUMOFWEIGHTS[13];
	static XMLCh xmlSUMOFVALUES[12];
	static XMLCh xmlSUMOFVALUESSQ[14];
	static XMLCh xmlMEAN[5];
	static XMLCh xmlPRECISION[10];
	static XMLCh xmlSTANDARDDEV[12];
	static XMLCh xmlDISCRETEESTIMATOR[18];
	static XMLCh xmlNUMSYMBOLS[11];
	static XMLCh xmlCOUNTS[7];
	static int init;

	if (!init) {
		XMLPlatformUtils::Initialize();
		// set up useful constants
		XMLString::transcode("NBIF", xmlNBIF, 4);
		XMLString::transcode("NB", xmlNB, 2);
		XMLString::transcode("NAME", xmlNAME, 4);
		XMLString::transcode("NUMCLASSES", xmlNUMCLASSES, 10);
		XMLString::transcode("NUMATTRIBUTES", xmlNUMATTRIBUTES, 13);
		XMLString::transcode("DISTRIBS", xmlDISTRIBS, 8);
		XMLString::transcode("CLASSDISTRIB", xmlCLASSDISTRIB, 12);
		XMLString::transcode("NORMALESTIMATOR", xmlNORMALESTIMATOR, 15);
		XMLString::transcode("SUMOFWEIGHTS", xmlSUMOFWEIGHTS, 12);
		XMLString::transcode("SUMOFVALUES", xmlSUMOFVALUES, 11);
		XMLString::transcode("SUMOFVALUESSQ", xmlSUMOFVALUESSQ, 13);
		XMLString::transcode("MEAN", xmlMEAN, 4);
		XMLString::transcode("PRECISION", xmlPRECISION, 9);
		XMLString::transcode("STANDARDDEV", xmlSTANDARDDEV, 11);
		XMLString::transcode("DISCRETEESTIMATOR", xmlDISCRETEESTIMATOR, 17);
		XMLString::transcode("NUMSYMBOLS", xmlNUMSYMBOLS, 10);
		XMLString::transcode("COUNTS", xmlCOUNTS, 6);
		init = 1;
	}

	XercesDOMParser parser;
	parser.setValidationScheme(XercesDOMParser::Val_Never);
	parser.setDoSchema(false) ;
	parser.setLoadExternalDTD(false) ;
	StreamInputSource is(s);
	parser.parse(is);

	DOMDocument * document = parser.getDocument();

	DOMNodeList * nodeList;
	DOMElement * element, * nbElement, * distribsElement;

	// NBIF
	nodeList = document->getElementsByTagName(xmlNBIF);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
		throw new runtime_error("cannot find NBIF");
	// NBIF.NB
	nodeList = element->getElementsByTagName(xmlNB);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
		throw new runtime_error("cannot find NBIF.NB");
	nbElement = element;
	// NBIF.NB.NAME
	nodeList = nbElement->getElementsByTagName(xmlNAME);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
		throw new runtime_error("cannot find NBIF.NB.NAME");
	// text for NBIF.NB.NAME
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
		throw new runtime_error("cannot get text for NBIF.NB.NAME");
   	string nbName(cp);
   	XMLString::release(&cp);

	Debug.dprintf("%s: reading NaiveBayes '%s'\n", __FUNCTION__,
		nbName.c_str());

	// NBIF.NB.NUMCLASSES
	nodeList = nbElement->getElementsByTagName(xmlNUMCLASSES);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
		throw new runtime_error("cannot find NBIF.NB.NUMCLASSES");
	// text for NBIF.NB.NUMCLASSES
	cp = NULL;
	nodeList = element->getChildNodes();
	for (int i = 0; i < (int)nodeList->getLength(); i++) {
		DOMNode * node = nodeList->item(i);
		if (node->getNodeType() == DOMNode::TEXT_NODE) {
			cp = XMLString::transcode(node->getNodeValue());
			break;
		}
	}
	if (cp == NULL)
		throw new runtime_error("cannot get text for NBIF.NB.NUMCLASSES");
	int nClasses = atoi(cp);
   	XMLString::release(&cp);
	if (!nClasses)
		throw new runtime_error("invalid number of classes");

	// NBIF.NB.NUMATTRIBUTES
	nodeList = nbElement->getElementsByTagName(xmlNUMATTRIBUTES);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
		throw new runtime_error("cannot find NBIF.NB.NUMATTRIBUTES");
	// text for NBIF.NB.NUMATTRIBUTES
	cp = NULL;
 	nodeList = element->getChildNodes();
	for (int i = 0; i < (int)nodeList->getLength(); i++) {
		DOMNode * node = nodeList->item(i);
		if (node->getNodeType() == DOMNode::TEXT_NODE) {
			cp = XMLString::transcode(node->getNodeValue());
			break;
		}
	}
	if (cp == NULL)
		throw new runtime_error("cannot get text for NBIF.NB.NUMATTRIBUTES");
	int nAttributes = atoi(cp);
   	XMLString::release(&cp);
	if (!nAttributes)
		throw new runtime_error("invalid number of attributes");

	NaiveBayes * nb = new NaiveBayes(nbName, nClasses, nAttributes + 1);

	// NBIF.NB.DISTRIBS[]
	nodeList = nbElement->getElementsByTagName(xmlDISTRIBS);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
		throw new runtime_error("cannot find NBIF.NB.DISTRIBS");
	distribsElement = element;

	int iAttribute = 0;
	int iClass = 0;

	// NBIF.NB.DISTRIBS[].<ESTIMATOR>
	bool isNormal = true;
	nodeList = distribsElement->getElementsByTagName(xmlNORMALESTIMATOR);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
	{
		nodeList = distribsElement->getElementsByTagName(xmlDISCRETEESTIMATOR);
		if ((nodeList->getLength() < 1) ||
				((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
					== NULL))
			throw new runtime_error("cannot find recognizable estimator in "
				"NBIF.NB.DISTRIBS[]");
		isNormal = false;
	}
	for (int i = 0; i < (int)nodeList->getLength(); i++) {
		Estimator * est;
		DOMElement * estElement = 
			dynamic_cast<DOMElement *>(nodeList->item(i));
		if (!estElement)
			continue;
		if (isNormal) {
			DOMElement * element;
			// ...NORMALESTIMATOR.SUMOFWEIGHTS
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlSUMOFWEIGHTS)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.SUMOFWEIGHTS");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.SUMOFWEIGHTS");
			double sumOfWeights = atof(cp);
			XMLString::release(&cp);
			// ...NORMALESTIMATOR.SUMOFVALUES
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlSUMOFVALUES)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.SUMOFVALUES");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.SUMOFVALUES");
			double sumOfValues = atof(cp);
			XMLString::release(&cp);
			// ...NORMALESTIMATOR.SUMOFVALUESSQ
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlSUMOFVALUESSQ)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.SUMOFVALUESSQ");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.SUMOFVALUESSQ");
			double sumOfValuesSq = atof(cp);
			XMLString::release(&cp);
			// ...NORMALESTIMATOR.MEAN
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlMEAN)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.MEAN");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.MEAN");
			double mean = atof(cp);
			XMLString::release(&cp);
			// ...NORMALESTIMATOR.PRECISION
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlPRECISION)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.PRECISION");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.PRECISION");
			double precision = atof(cp);
			XMLString::release(&cp);
			// ...NORMALESTIMATOR.STANDARDDEV
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlSTANDARDDEV)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.STANDARDDEV");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.DISTRIBS[].NORMALESTIMATOR.STANDARDDEV");
			double standardDev = atof(cp);
			XMLString::release(&cp);
			NormalEstimator * nEst = 
				new NormalEstimator(NaiveBayes::DEFAULT_PRECISION);
			nEst->m_SumOfWeights = sumOfWeights;
			nEst->m_SumOfValues = sumOfValues;
			nEst->m_SumOfValuesSq = sumOfValuesSq;
			nEst->m_Mean = mean;
			nEst->m_Precision = precision;
			nEst->m_StandardDev = standardDev;
			est = (Estimator *) nEst;

			Debug.dprintf("\tnormal estimator %d:%d: "
						  " sumOfWeights %lf"
						  " sumOfValues %lf"
						  " sumOfValuesSq %lf"
						  " mean %lf"
						  " precision %lf"
						  " standardDev %lf\n",
				iAttribute, iClass,
				sumOfWeights,
				sumOfValues,
				sumOfValuesSq,
				mean,
				precision,
				standardDev);
		} else {
			DOMElement * element;
			// ...DISCRETEESTIMATOR.NUMSYMBOLS
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlNUMSYMBOLS)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.DISTRIBS[].DISCRETEESTIMATOR.NUMSYMBOLS");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.DISTRIBS[].DISCRETEESTIMATOR.NUMSYMBOLS");
			int nSymbols = atoi(cp);
			XMLString::release(&cp);
			// ...DISCRETEESTIMATOR.COUNTS
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlCOUNTS)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.DISTRIBS[].DISCRETEESTIMATOR.COUNTS");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.DISTRIBS[].DISCRETEESTIMATOR.COUNTS");
			DiscreteEstimator * dEst = new DiscreteEstimator(nSymbols, 0.0);

			char * table = cp;

			Debug.dprintf("\tdiscrete estimator %d:%d: "
						  " nSymbols %u"
						  " counts %s\n",
				iAttribute, iClass,
				nSymbols,
				table);

			for (int k = 0; k < nSymbols; k++) {
				char buf[40];
				int l;
				double weight;
				// get next weight token from table text
				while (isspace(*cp) && *cp)
					cp++;
				if (!*cp)
					throw new runtime_error("truncated COUNTS for"
						" DISCRETEESTIMATOR");
				for (l = 0; !isspace(*cp) && *cp && l < 40; l++) {
					buf[l] = *cp;
					cp++;
				}
				if (l == 40)
					throw new runtime_error("entry too long in COUNTS"
						" for DISCRETEESTIMATOR");
				buf[l] = 0;
				weight = atof(buf);
				dEst->addValue(k, weight);
			}
			XMLString::release(&table);
			est = (Estimator *) dEst;
		}

		nb->setDistribution(iAttribute, iClass, est);
		iClass++;
		if (iClass >= nClasses) {
			iClass = 0;
			iAttribute++;
			if (iAttribute > nAttributes)
				throw new runtime_error("too many estimators");
		}
	}

	// NBIF.NB.CLASSDISTRIB
	nodeList = nbElement->getElementsByTagName(xmlCLASSDISTRIB);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
		throw new runtime_error("cannot find NBIF.NB.CLASSDISTRIB");
	distribsElement = element;
	nodeList = distribsElement->getElementsByTagName(xmlNORMALESTIMATOR);
	if ((nodeList->getLength() < 1) ||
			((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
				== NULL))
	{
		nodeList = distribsElement->getElementsByTagName(xmlDISCRETEESTIMATOR);
		if ((nodeList->getLength() < 1) ||
				((element = dynamic_cast<DOMElement *>(nodeList->item(0)))
					== NULL))
			throw new runtime_error("cannot find recognizable estimator in "
				"NBIF.NB.CLASSDISTRIB");
		isNormal = false;
	}
	for (int i = 0; i < (int)nodeList->getLength(); i++) {
		DOMElement * estElement = 
			dynamic_cast<DOMElement *>(nodeList->item(i));
		if (!estElement)
			continue;
		if (isNormal) {
			DOMElement * element;
			// ...NORMALESTIMATOR.SUMOFWEIGHTS
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlSUMOFWEIGHTS)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.SUMOFWEIGHTS");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.SUMOFWEIGHTS");
			double sumOfWeights = atof(cp);
			XMLString::release(&cp);
			// ...NORMALESTIMATOR.SUMOFVALUES
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlSUMOFVALUES)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.SUMOFVALUES");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.SUMOFVALUES");
			double sumOfValues = atof(cp);
			XMLString::release(&cp);
			// ...NORMALESTIMATOR.SUMOFVALUESSQ
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlSUMOFVALUESSQ)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.SUMOFVALUESSQ");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.SUMOFVALUESSQ");
			double sumOfValuesSq = atof(cp);
			XMLString::release(&cp);
			// ...NORMALESTIMATOR.MEAN
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlMEAN)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.MEAN");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.MEAN");
			double mean = atof(cp);
			XMLString::release(&cp);
			// ...NORMALESTIMATOR.PRECISION
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlPRECISION)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.PRECISION");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.PRECISION");
			double precision = atof(cp);
			XMLString::release(&cp);
			// ...NORMALESTIMATOR.STANDARDDEV
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlSTANDARDDEV)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.STANDARDDEV");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.CLASSDISTRIB.NORMALESTIMATOR.STANDARDDEV");
			double standardDev = atof(cp);
			XMLString::release(&cp);
			NormalEstimator * nEst =
				new NormalEstimator(NaiveBayes::DEFAULT_PRECISION);
			nEst->m_SumOfWeights = sumOfWeights;
			nEst->m_SumOfValues = sumOfValues;
			nEst->m_SumOfValuesSq = sumOfValuesSq;
			nEst->m_Mean = mean;
			nEst->m_Precision = precision;
			nEst->m_StandardDev = standardDev;

			Debug.dprintf("\tnormal estimator <class>: "
						  " sumOfWeights %lf"
						  " sumOfValues %lf"
						  " sumOfValuesSq %lf"
						  " mean %lf"
						  " precision %lf"
						  " standardDev %lf\n",
				sumOfWeights,
				sumOfValues,
				sumOfValuesSq,
				mean,
				precision,
				standardDev);

			nb->setClassDistribution(nEst);
		} else {
			DOMElement * element;
			// ...DISCRETEESTIMATOR.NUMSYMBOLS
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlNUMSYMBOLS)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.CLASSDISTRIB.DISCRETEESTIMATOR.NUMSYMBOLS");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.CLASSDISTRIB.DISCRETEESTIMATOR.NUMSYMBOLS");
			int nSymbols = atoi(cp);
			XMLString::release(&cp);
			// ...DISCRETEESTIMATOR.COUNTS
			element = dynamic_cast<DOMElement *>
				(estElement->getElementsByTagName(xmlCOUNTS)->item(0));
			if (!element)
				throw new runtime_error("cannot find "
					"NBIF.NB.CLASSDISTRIB.DISCRETEESTIMATOR.COUNTS");
			cp = NULL;
			for (int j = 0; 
				 j < (int)element->getChildNodes()->getLength();
				 j++)
			{
				DOMNode * node = element->getChildNodes()->item(j);
				if (node->getNodeType() == DOMNode::TEXT_NODE) {
					cp = XMLString::transcode(node->getNodeValue());
					break;
				}
			}
			if (cp == NULL)
				throw new runtime_error("cannot find text for "
					"NBIF.NB.CLASSDISTRIB.DISCRETEESTIMATOR.COUNTS");
			DiscreteEstimator * dEst = new DiscreteEstimator(nSymbols, 0.0);
			char * table = cp;

			Debug.dprintf("\tdiscrete estimator <class>: "
						  " nSymbols %u"
						  " counts %s\n",
				nSymbols,
				cp);

			for (int k = 0; k < nSymbols; k++) {
				char buf[40];
				int l;
				double weight;
				// get next weight token from table text
				while (isspace(*cp) && *cp)
					cp++;
				if (!*cp)
					throw new runtime_error("truncated COUNTS for"
						" DISCRETEESTIMATOR");
				for (l = 0; !isspace(*cp) && *cp && l < 40; l++) {
					buf[l] = *cp;
					cp++;
				}
				if (l == 40)
					throw new runtime_error("entry too long in COUNTS"
						" for DISCRETEESTIMATOR");
				buf[l] = 0;
				weight = atof(buf);
				dEst->addValue(k, weight);
			}
			XMLString::release(&table);
			nb->setClassDistribution(dEst);
		}
		break;
	}

	return nb;
}

NaiveBayes *
NaiveBayes::fromXML (streambuf * s)
{
	// test for '<' at start of stream, iterating through available
	// filters, until successful
	s->pubseekoff(0, ios_base::beg);
	istream is(s);
	if (is.get() == (int)'<') {
		is.seekg(0, ios_base::beg);
		return readXML(s);
	}
	// no filters available at this time
	throw new runtime_error("unrecongized format");
	return NULL; // NOTREACHED
}

NaiveBayes *
NaiveBayes::fromXML (streambuf& s)
{
	return fromXML(&s);
}

NaiveBayes *
NaiveBayes::fromXML (string& xml)
{
	stringbuf s(xml);
	return fromXML(&s);
}

#else /* ! HAVE_XERCESC */

NaiveBayes *
NaiveBayes::fromXML (streambuf * s)
{
	throw new runtime_error("not implemented");
	return NULL; // NOTREACHED
}

NaiveBayes *
NaiveBayes::fromXML (streambuf& s)
{
	throw new runtime_error("not implemented");
	return NULL; // NOTREACHED
}

NaiveBayes *
NaiveBayes::fromXML (string& xml)
{
	throw new runtime_error("not implemented");
	return NULL; // NOTREACHED
}

#endif /* HAVE_XERCESC */

NaiveBayes::NaiveBayes (string& name, int nClasses, int nAttributes) :
	Classifier(name),
	m_Distributions(NULL),
	m_ClassDistribution(NULL),
	m_nClasses(nClasses),
	m_nAttributes(nAttributes)
{
	// initialize distribution estimators if we've been dimensioned already
	if (nClasses != 0 && nAttributes != 0) {
		m_Distributions = new Estimator**[m_nAttributes - 1];
		if (m_Distributions == NULL)
			throw new runtime_error("out of memory");
		for (int i = 0; i < m_nAttributes - 1; i++) {
			m_Distributions[i] = new Estimator*[m_nClasses];
			if (m_Distributions[i] == NULL)
				throw new runtime_error("out of memory");
			for (int j = 0; j < m_nClasses; j++)
				m_Distributions[i][j] = NULL;
		}
	}
}

NaiveBayes::~NaiveBayes()
{
	if (m_Distributions != NULL) {
		for (int i = 0; i < m_nAttributes - 1; i++)
			if (m_Distributions[i] != NULL)
				delete [] m_Distributions[i];
		delete [] m_Distributions;
	}
}

void
NaiveBayes::buildClassifier (Instances * instances)
{
	// just assume classifiers built for training are not dimensioned at
	// instantiation time

	Debug.dprintf("%s: building classifier\n", __FUNCTION__);

    m_nClasses = instances->numClasses();
	m_nAttributes = instances->numAttributes();

    m_Distributions = new Estimator**[m_nAttributes - 1];
	if (m_Distributions == NULL)
		throw new runtime_error("out of memory");
	for (int i = 0; i < m_nAttributes - 1; i++) {
		m_Distributions[i] = new Estimator*[m_nClasses];
		if (m_Distributions[i] == NULL)
			throw new runtime_error("out of memory");
		for (int j = 0; j < m_nClasses; j++)
			m_Distributions[i][j] = NULL;
	}
    m_ClassDistribution = new DiscreteEstimator(m_nClasses, true);

	Instances trainInstances(instances);

	int attIndex = 0;
	for (int i = 0; i < m_nAttributes; i++) {
		Attribute * attr = trainInstances.attribute(i);
		double numPrecision = DEFAULT_PRECISION;
		if (attr->type() == Attribute::NUMERIC) {
			trainInstances.sort(attr);
			if ((trainInstances.numInstances() > 0) &&
					!trainInstances.instance(0)->isMissing(attr))
			{
				double lastVal = trainInstances.instance(0)->value(attr);
				double currentVal, deltaSum = 0;
				int distinct = 0;
				for (int j = 1; j < trainInstances.numInstances(); j++) {
					Instance * currentInst = trainInstances.instance(j);
					if (currentInst->isMissing(attr))
						break;
					currentVal = currentInst->value(attr);
					if (currentVal != lastVal) {
						deltaSum += currentVal - lastVal;
						lastVal = currentVal;
						distinct++;
					}
				}
				if (distinct > 0)
					numPrecision = deltaSum / distinct;
			}
		}
		for (int j = 0; j < m_nClasses; j++) {
			switch (attr->type()) {
			case Attribute::NUMERIC: 
				m_Distributions[attIndex][j] = 
					new NormalEstimator(numPrecision);
				break;
			case Attribute::NOMINAL:
				m_Distributions[attIndex][j] = 
					new DiscreteEstimator(attr->numValues(), true);
				break;
			default:
				throw new runtime_error("unknown attribute type");
			}
		}
		attIndex++;
	}

	for (int i = 0; i < trainInstances.numInstances(); i++)
		updateClassifier(trainInstances.instance(i));
}

void
NaiveBayes::updateClassifier (Instance * instance)
{
	if (instance->numAttributes() != m_nAttributes)
		throw new runtime_error("incorrect number of attributes in instance");
	if (instance->numClasses() != m_nClasses)
		throw new runtime_error("incorrect number of classes in instance");
    if (instance->classIsMissing())
		throw new runtime_error("missing class in instance");
	int attIndex = 0;
	for (int i = 0; i < m_nAttributes; i++) {
		Attribute * attr = instance->attribute(i);
		if (!instance->isMissing(attr)) {
			m_Distributions[attIndex][(int)instance->classValue()]
				-> addValue(instance->value(attr), instance->weight());
		}
		attIndex++;
	}
	m_ClassDistribution->addValue(instance->classValue(),
		instance->weight());
}

vector<double>
NaiveBayes::distributionForInstance (Instance * instance)
{
	if (instance->numAttributes() != m_nAttributes)
		throw new runtime_error("incorrect number of attributes in instance");
	vector<double> v(m_nClasses);
    for (int i = 0; i < m_nClasses; i++)
		v[i] = m_ClassDistribution->getProbability(i);
	int attIndex = 0;
	for (int i = 0; i < m_nAttributes; i++) {
		Attribute * attr = instance->attribute(i);
		if (!instance->isMissing(attr)) {
			double temp, max = 0.0;
			for (int j = 0; j < m_nClasses; j++) {
				temp = m_Distributions[attIndex][j]
							-> getProbability(instance->value(attr));
				if (temp < 1e-75)
					temp = 1e-75;
				v[j] *= temp;
				if (v[j] > max)
					max = v[j];
				if (isnan(v[j]))
					throw new runtime_error("NaN returned from estimator for"
						" attribute");
			}
			if ((max > 0.0) && (max < 1e-75))
				for (int j = 0; j < m_nClasses; j++)
					v[j] *= 1e75;
		}
		attIndex++;
	}

	// normalize
	double sum = 0.0;
	for (int i = 0; i < m_nClasses; i++)
		sum += v[i];
	for (int i = 0; i < m_nClasses; i++)
		v[i] = v[i] / sum;

	return v;
}

void
NaiveBayes::XMLDTD (streambuf * s)
{
	static string s1(
		"<?xml version=\"1.0\"?>\n"
		"<!DOCTYPE NBIF [\n"
		"\t<!ELEMENT NBIF ( NB )*>\n"
		"\t\t<!ATTLIST NBIF VERSION CDATA #REQUIRED>\n"
		"\t<!ELEMENT NB (NAME, NUMCLASSES, NUMATTRIBUTES, DISTRIBS, "
				"CLASSDISTRIB)>\n"
		"\t<!ELEMENT NAME (#PCDATA)>\n"
		"\t<!ELEMENT NUMCLASSES (#PCDATA)>\n"
		"\t<!ELEMENT NUMATTRIBUTES (#PCDATA)>\n"
		"\t<!ELEMENT DISTRIBS ( NORMALESTIMATOR | DISCRETEESTIMATOR )*>\n"
		"\t<!ELEMENT CLASSDISTRIB ( NORMALESTIMATOR | DISCRETEESTIMATOR )>\n"
		"\t<!ELEMENT NORMALESTIMATOR ( SUMOFWEIGHTS | SUMOFVALUES | "
				"SUMOFVALUESSQ | MEAN | PRECISION | STANDARDDEV )*>\n"
		"\t<!ELEMENT SUMOFWEIGHTS (#PCDATA)>\n"
		"\t<!ELEMENT SUMOFVALUES (#PCDATA)>\n"
		"\t<!ELEMENT SUMOFVALUESSQ (#PCDATA)>\n"
		"\t<!ELEMENT MEAN (#PCDATA)>\n"
		"\t<!ELEMENT PRECISION (#PCDATA)>\n"
		"\t<!ELEMENT STANDARDDEV (#PCDATA)>\n"
		"\t<!ELEMENT DISCRETEESTIMATOR ( NUMSYMBOLS, COUNTS )>\n"
		"\t<!ELEMENT NUMSYMBOLS (#PCDATA)>\n"
		"\t<!ELEMENT COUNTS (#PCDATA)>\n"
		"]>\n\n");
	s->sputn(s1.c_str(), s1.length());
}

void
NaiveBayes::XMLDTD (streambuf& s)
{
	XMLDTD(&s);
}

void
NaiveBayes::XMLDTD (string& xml)
{
	stringbuf s(xml);
	XMLDTD(s);
}

void
NaiveBayes::toXML (streambuf * s)
{
	char buf[128];

	Debug.dprintf("%s: writing NaiveBayes '%s'\n", __FUNCTION__,
		m_Name.c_str());

	s->sputn(
		"<NBIF VERSION=\"0.1\">\n"
		"<NB>\n"
		"<NAME>",
		32);
	string s1 = xmlnorm(m_Name);
	s->sputn(s1.c_str(), s1.length());
	s->sputn("</NAME>\n", 8);
	s->sputn("<NUMCLASSES>", 12);
	s->sputn(buf, sprintf(buf, "%u", m_nClasses));
	s->sputn("</NUMCLASSES>\n", 14);
	s->sputn("<NUMATTRIBUTES>", 15);
	s->sputn(buf, sprintf(buf, "%u", m_nAttributes - 1));
	s->sputn("</NUMATTRIBUTES>\n", 17);
	s->sputn("<DISTRIBS>\n", 11);
	for (int i = 0; i < m_nAttributes - 1; i++) {
		for (int j = 0; j < m_nClasses; j++) {
			NormalEstimator * nEst = 
				dynamic_cast<NormalEstimator *>(m_Distributions[i][j]);
			if (nEst != NULL) {
				s->sputn("\t<NORMALESTIMATOR>\n", 19);
				s->sputn("\t\t<SUMOFWEIGHTS>", 16);
				s->sputn(buf, sprintf(buf, "%0.16lf", nEst->m_SumOfWeights));
				s->sputn("</SUMOFWEIGHTS>\n", 16);
				s->sputn("\t\t<SUMOFVALUES>", 15);
				s->sputn(buf, sprintf(buf, "%0.16lf", nEst->m_SumOfValues));
				s->sputn("</SUMOFVALUES>\n", 15);
				s->sputn("\t\t<SUMOFVALUESSQ>", 17);
				s->sputn(buf, sprintf(buf, "%0.16lf", nEst->m_SumOfValuesSq));
				s->sputn("</SUMOFVALUESSQ>\n", 17);
				s->sputn("\t\t<MEAN>", 8);
				s->sputn(buf, sprintf(buf, "%0.16lf", nEst->m_Mean));
				s->sputn("</MEAN>\n", 8);
				s->sputn("\t\t<PRECISION>", 13);
				s->sputn(buf, sprintf(buf, "%0.16lf", nEst->m_Precision));
				s->sputn("</PRECISION>\n", 13);
				s->sputn("\t\t<STANDARDDEV>", 15);
				s->sputn(buf, sprintf(buf, "%0.16lf", nEst->m_StandardDev));
				s->sputn("</STANDARDDEV>\n", 15);
				s->sputn("\t</NORMALESTIMATOR>\n", 20);

				Debug.dprintf("\tnormal estimator %d:%d: "
							  " sumOfWeights %lf"
							  " sumOfValues %lf"
							  " sumOfValuesSq %lf"
							  " mean %lf"
							  " precision %lf"
							  " standardDev %lf\n",
					i, j,
					nEst->m_SumOfWeights,
					nEst->m_SumOfValues,
					nEst->m_SumOfValuesSq,
					nEst->m_Mean,
					nEst->m_Precision,
					nEst->m_StandardDev);

				continue;
			} else {
				DiscreteEstimator * dEst =
					dynamic_cast<DiscreteEstimator *>(m_Distributions[i][j]);
				if (dEst != NULL) {
					s->sputn("\t<DISCRETEESTIMATOR>\n", 21);
					s->sputn("\t\t<NUMSYMBOLS>", 14);
					s->sputn(buf, sprintf(buf, "%u", dEst->m_nSymbols));
					s->sputn("</NUMSYMBOLS>\n", 14);
					s->sputn("\t\t<COUNTS>", 10);
					for (int k = 0; k < dEst->m_nSymbols; k++)
						s->sputn(buf,
							sprintf(buf, " %0.16lf", dEst->m_Counts[k]));
					s->sputn(" </COUNTS>\n", 11);
					s->sputn("\t</DISCRETEESTIMATOR>\n", 22);

					Debug.dprintf("\tdiscrete estimator %d:%d: "
								  " numSymbols %u counts ",
						i, j,
						dEst->m_nSymbols);
					for (int k = 0; k < dEst->m_nSymbols; k++)
						Debug.dprintf(" %lf", dEst->m_Counts[k]);
					Debug.dprintf("\n");

					continue;
				} else {
					throw new runtime_error("unsupported Estimator");
				}
			}
		}
	}
	s->sputn("<CLASSDISTRIB>\n", 15);
	DiscreteEstimator * dEst =
		dynamic_cast<DiscreteEstimator *>(m_ClassDistribution);
	if (dEst == NULL)
		throw new runtime_error("class estimator is not a DiscreteEstimator");
	s->sputn("\t<DISCRETEESTIMATOR>\n", 21);
	s->sputn("\t\t<NUMSYMBOLS>", 14);
	s->sputn(buf, sprintf(buf, "%u", dEst->m_nSymbols));
	s->sputn("</NUMSYMBOLS>\n", 14);
	s->sputn("\t\t<COUNTS>", 10);
	s->sputn(buf, sprintf(buf, "%lu", (unsigned long)dEst->m_Counts[0]));
	for (int i = 1; i < dEst->m_nSymbols; i++)
		s->sputn(buf, sprintf(buf, " %lu", (unsigned long)dEst->m_Counts[i]));
	s->sputn("</COUNTS>\n", 10);
	s->sputn("\t</DISCRETEESTIMATOR>\n", 22);
	s->sputn("</CLASSDISTRIB>\n", 16);

	Debug.dprintf("\tdiscrete estimator <class>: numSymbols %u counts ",
		dEst->m_nSymbols);
	for (int i = 0; i < dEst->m_nSymbols; i++)
		Debug.dprintf(" %lu", (unsigned long)dEst->m_Counts[i]);
	Debug.dprintf("\n");

	s->sputn(
		"</NB>\n"
		"</NBIF>\n",
		14);
}

void
NaiveBayes::toXML (streambuf& s)
{
	toXML(&s);
}

void
NaiveBayes::toXML (string& xml)
{
	stringbuf s(xml);
	toXML(s);
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
