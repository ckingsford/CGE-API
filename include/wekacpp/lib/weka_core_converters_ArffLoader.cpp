/*
 * Derived from:
 *
 * ArffLoader.java
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
#include <weka/core/converters/ArffLoader.h>
#include <cctype>
#include <iostream>
#include <string>
#include <stdexcept>
#include <streambuf>

namespace weka {
	namespace converters {

using namespace std;

string
ArffLoader::Token::toString ()
{
	string s;
	switch (kind) {
	case ERR:		s.append("<ERROR>");							break;
	case COMMA:		s.append("','");								break;
	case LBRACE:	s.append("'{'");								break;
	case RBRACE:	s.append("'}'");								break;
	case LBRACK:	s.append("'['");								break;
	case RBRACK:	s.append("']'");								break;
	case LPAREN:	s.append("'('");								break;
	case RPAREN:	s.append("')'");								break;
	case IDENT:		s.append("'"); s.append(text); s.append("'");	break;
	case NUMBER:	s.append(text);									break;
	case STRING:	s.append("\""); s.append(text); s.append("\"");	break;
	case ATTRIBUTE:	s.append("'@attribute'");						break;
	case DATA:		s.append("'@data'");							break;
	case RELATION:	s.append("'@relation'");						break;
	}
	return s;
}

ArffLoader::Token
ArffLoader::getNextToken ()
{
	ArffLoader::Token token;

	// skip whitespace

  again:
	int c = m_Stream->sgetc();

	while (isspace(c)) {
		if (c == '\n')
			m_Line++;
		c = m_Stream->snextc();
	}

	// skip comments

	if (c == '%') {
		while ((c != '\n') && (c != EOF))
			c = m_Stream->snextc();
		m_Line++;
		goto again;
	}

	// process next token

	char buf[2];
	buf[1] = '\0';

	switch (c) {

	case '{':
		token.kind = ArffLoader::Token::LBRACE;
		m_Stream->sbumpc();
		goto out;

	case '}':
		token.kind = ArffLoader::Token::RBRACE;
		m_Stream->sbumpc();
		goto out;

	case '[':
		token.kind = ArffLoader::Token::LBRACK;
		m_Stream->sbumpc();
		goto out;

	case ']':
		token.kind = ArffLoader::Token::RBRACK;
		m_Stream->sbumpc();
		goto out;

	case '(':
		token.kind = ArffLoader::Token::LPAREN;
		m_Stream->sbumpc();
		goto out;

	case ')':
		token.kind = ArffLoader::Token::RPAREN;
		m_Stream->sbumpc();
		goto out;

	case ',':
		token.kind = ArffLoader::Token::COMMA;
		m_Stream->sbumpc();
		goto out;

	case '"':
		while (true) {
			c = m_Stream->snextc();
			if (c == '"') {
				m_Stream->sbumpc();
				token.kind = ArffLoader::Token::STRING;
				goto out;
			}
			if (c == EOF)
				goto error;
			buf[0] = c;
			token.text.append(buf);
		}
		break;

	case '\'':
		while (true) {
			c = m_Stream->snextc();
			if (c == '\'') {
				m_Stream->sbumpc();
				token.kind = ArffLoader::Token::STRING;
				goto out;
			}
			if (c == EOF)
				goto error;
			buf[0] = c;
			token.text.append(buf);
		}
		break;

	case '@':
		switch (tolower(m_Stream->snextc())) {
		case 'a':
			if ((tolower(m_Stream->snextc()) == 't') &&
				(tolower(m_Stream->snextc()) == 't') &&
				(tolower(m_Stream->snextc()) == 'r') &&
				(tolower(m_Stream->snextc()) == 'i') &&
				(tolower(m_Stream->snextc()) == 'b') &&
				(tolower(m_Stream->snextc()) == 'u') &&
				(tolower(m_Stream->snextc()) == 't') &&
				(tolower(m_Stream->snextc()) == 'e') &&
				(isspace(m_Stream->snextc())))
			{
				token.kind = ArffLoader::Token::ATTRIBUTE;
				goto out;
			}
			break;
		case 'd':
			if ((tolower(m_Stream->snextc()) == 'a') &&
				(tolower(m_Stream->snextc()) == 't') &&
				(tolower(m_Stream->snextc()) == 'a') &&
				(isspace(m_Stream->snextc())))
			{
				token.kind = ArffLoader::Token::DATA;
				goto out;
			}
			break;
		case 'r':
			if ((tolower(m_Stream->snextc()) == 'e') &&
				(tolower(m_Stream->snextc()) == 'l') &&
				(tolower(m_Stream->snextc()) == 'a') &&
				(tolower(m_Stream->snextc()) == 't') &&
				(tolower(m_Stream->snextc()) == 'i') &&
				(tolower(m_Stream->snextc()) == 'o') &&
				(tolower(m_Stream->snextc()) == 'n') &&
				(isspace(m_Stream->snextc())))
			{
				token.kind = ArffLoader::Token::RELATION;
				goto out;
			}
			break;
		default:
			goto error;
		}
		break;

	case 'i': {
		int next = m_Stream->snextc();
		if ((tolower(next) == 'n') && (tolower(m_Stream->snextc()) == 'f')) {
			token.text.append("inf");
			m_Stream->sbumpc();
			token.kind = ArffLoader::Token::NUMBER;
			goto out;
		} else {
			m_Stream->sputbackc(next);
			goto ident;
		}
	}	break;

	case '+':
	case '-': {
		int next = m_Stream->snextc();
		if (tolower(next) == 'i') {
			if ((tolower(m_Stream->snextc()) == 'n') &&
				(tolower(m_Stream->snextc()) == 'f'))
			{
				buf[0] = c;
				token.text.append(buf);
				token.text.append("inf");
				m_Stream->sbumpc();
				token.kind = ArffLoader::Token::NUMBER;
				goto out;
			} else {
				goto error;
			}
		} else {
			m_Stream->sputbackc(next);
		}
	}
	// fallthrough
	case '.':
	case '0' ... '9':
		buf[0] = c;
		token.text.append(buf);
		while (true) {
			c = m_Stream->snextc();
			switch (c) {
			case '+': 
			case '-': 
			case '.':
			case '0' ... '9':
			case 'e': case 'E': 
				buf[0] = c;
				token.text.append(buf);
				break;
			default:
				token.kind = ArffLoader::Token::NUMBER;
				goto out;
			}
		}
		break;

	case '_': 
	case 'A' ... 'Z': 
	case 'a' ... 'h': case 'j' ... 'z':
	  ident:
		buf[0] = c;
		token.text.append(buf);
		while (true) {
			c = m_Stream->snextc();
			switch (c) {
			case '_': 
			case '-': 
			case '0' ... '9': 
			case 'A' ... 'Z': 
			case 'a' ... 'z':
				buf[0] = c;
				token.text.append(buf);
				break;
			default:
				token.kind = ArffLoader::Token::IDENT;
				goto out;
			}
		}
		break;

	default:
		goto error;
		
	}

  out:
	return token;

  error:
	token.kind = ArffLoader::Token::ERR;
	return token;
}

void
ArffLoader::readHeader ()
{
	if (m_Phase != IN_HEADER)
		return;

	Attribute * attr = NULL;

	Token token = getNextToken();

	while (token.kind != Token::DATA) {

		switch (token.kind) {

		case Token::RELATION: {
			token = getNextToken();
			if (token.kind != Token::IDENT) {
				cerr << __FUNCTION__ << "; unexpected " << token.toString() 
					 << " after @RELATION\n";
				m_Phase = IN_ERR;
				return;
			}
			if (m_RelationName.size() > 0) {
				cerr << __FUNCTION__ << ": @RELATION already seen\n";
				m_Phase = IN_ERR;
				return;
			}
			m_RelationName = token.text;
		}	break;

		case Token::ATTRIBUTE: {
			string name;

			attr = NULL;

			token = getNextToken();
			if (token.kind != Token::IDENT) 
				goto attr_unexpected;
			name = token.text;
			token = getNextToken();

			switch (token.kind) {

			case Token::IDENT: {
				for (int i = 0; i < (int)token.text.size(); i++) 
					token.text[i] = tolower(token.text[i]);
				if (token.text == "numeric" || token.text == "real" || 
					token.text == "integer")
				{
					attr = (Attribute *)
						new NumericAttribute(name, m_Attributes.size());
					m_Attributes.push_back(attr);
				} else if (token.text == "string") {
					attr = (Attribute *)
						new StringAttribute(name, m_Attributes.size());
					m_Attributes.push_back(attr);
				} else {
					cerr << __FUNCTION__ << ": unknown data type '" 
						 << token.text << "' in @ATTRIBUTE\n";
					m_Phase = IN_ERR;
					return;
				}

				token = getNextToken();
				if (token.kind == Token::LBRACK ||
					token.kind == Token::LPAREN)
				{
					((NumericAttribute *)attr)->m_LowerBoundIsOpen = 
						(token.kind == Token::LPAREN);
					token = getNextToken();
					if (token.kind != Token::NUMBER) {
					bad_range:
						cerr << __FUNCTION__ 
							 << ": bad range for numeric attribute\n";
						m_Phase = IN_ERR;
						return;
					}
					if (token.text == "-inf") {
						((NumericAttribute *)attr)->m_LowerBound = -INFINITY;
					} else {
						((NumericAttribute *)attr)->m_LowerBound = 
							strtod(token.text.c_str(), NULL);
					}
					token = getNextToken();
					if (token.kind != Token::COMMA)
						goto bad_range;
					token = getNextToken();
					if (token.kind != Token::NUMBER)
						goto bad_range;
					if (token.text == "inf") {
						((NumericAttribute *)attr)->m_UpperBound = INFINITY;
					} else {
						((NumericAttribute *)attr)->m_UpperBound = 
							strtod(token.text.c_str(), NULL);
					}
					token = getNextToken();
					if ((token.kind != Token::RBRACK) && 
						(token.kind != Token::RPAREN))
						goto bad_range;
					((NumericAttribute *)attr)->m_UpperBoundIsOpen =
						(token.kind == Token::RPAREN);
				} else {
					continue;
				}
			}	break;

			case Token::LBRACE: {
				attr = (Attribute *)
					new NominalAttribute(name, m_Attributes.size());
				m_Attributes.push_back(attr);
				token = getNextToken();
				while (token.kind != Token::RBRACE) {
					switch (token.kind) {
					case Token::IDENT:
					case Token::NUMBER:
					case Token::STRING:
						attr->addValue(new string(token.text));
						break;
					case Token::COMMA:
						break;
					default:
						goto attr_unexpected;
					}
					token = getNextToken();
				}
			}	break;

			default:
			  attr_unexpected:
				cerr << __FUNCTION__ << ": unexpected " << token.toString()
					 << " after @ATTRIBUTE\n";
				m_Phase = IN_ERR;
				return;
			}
		}	break;

		case Token::DATA:
			break;

		default:
			cerr << __FUNCTION__ << ": unexpected " << token.toString()
				 << "\n";
			m_Phase = IN_ERR;
			return;
		}

		token = getNextToken();
	}

	m_Phase = IN_DATA;
	m_bHeaderRead = true;

	m_Header = new Instances(m_RelationName, m_Attributes);
	m_Header->setClassIndex(m_Attributes.size() - 1);
}

ArffLoader::ArffLoader (streambuf * s)
{
	m_Stream = s;
	m_bHeaderRead = false;
	m_Phase = IN_HEADER;
	m_Header = NULL;
	m_Line = 1;
}

ArffLoader::~ArffLoader ()
{
	// delete header, if any

	if (m_Header != NULL)
		delete m_Header;

	// clean attribute vector

	vector<Attribute *>::iterator i = m_Attributes.begin();
	while (i != m_Attributes.end()) {
		delete *i;
		i++;
	}
}

Instances *
ArffLoader::getStructure ()
{
	if (!m_bHeaderRead)
		readHeader();
	return m_Header;
}

Instances *
ArffLoader::getDataSet ()
{
	if (!m_bHeaderRead)
		readHeader();
	Instances * instances = new Instances(m_Header);
	Instance * instance;
	while ((instance = getNextInstance()) != NULL)
		instances->add(instance);
	return instances;
}

Instance *
ArffLoader::getNextInstance ()
{
	if (!m_bHeaderRead)
		readHeader();

	if (m_Phase != IN_DATA)
		return NULL;

	vector<double> values(m_Header->numAttributes());
	int i = 0;
	while (i < m_Header->numAttributes()) {
		Token token = getNextToken();

		if (token.kind == Token::ERR)
			return NULL;

		if (token.kind == Token::COMMA)
			continue;

		Attribute * attr = m_Header->attribute(i);

		switch (attr->type()) {

		case Attribute::NUMERIC: {
			switch (token.kind) {
			case Token::NUMBER: {
				double v;
				sscanf(token.text.c_str(), "%lf", &v);
				values[i] = v;
			}	break;
			default:
				cerr << __FUNCTION__ << ": attribute at index " << i 
					 << " is not numeric\n";
				m_Phase = IN_ERR;
				return NULL;
			}
		} break;

		case Attribute::NOMINAL: {
			switch (token.kind) {
			case Token::IDENT:
			case Token::STRING:
				values[i] = attr->indexOfValue(token.text);
				break;
			default:
				cerr << __FUNCTION__ << ": attribute at index " << i 
					 << " is not nominal\n";
				m_Phase = IN_ERR;
				return NULL;
			}
		} break;

		case Attribute::STRING: {
			switch (token.kind) {
			case Token::STRING:
				values[i] = attr->indexOfValue(token.text);
				break;
			default:
				cerr << __FUNCTION__ << ": attribute at index " << i 
					 << " is not a string\n";
				m_Phase = IN_ERR;
				return NULL;
			}
		} break;

		}

		i++;
	}

	Instance * instance = new Instance(1.0, values);
	instance->setDataset(m_Header);

	return instance;
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
