#ifndef CLINICALVALUE_H
#define CLINICALVALUE_H

#include <string>
#include "boost/date_time/gregorian/gregorian.hpp"
#include <vector>
#include <utility>
#include <memory>
//#include <iostream>
class ClinicalValue
{
public: 
   virtual ~ClinicalValue() = 0;   //Destructor
};
inline ClinicalValue::~ClinicalValue() { }

class MissingValue : public ClinicalValue
{
private: 
   MissingValue(){}                                
   ~MissingValue(){}                               
   MissingValue(const MissingValue&);              //Prevent copy-construction
   MissingValue& operator=(const MissingValue&);   //Prevent assignment
public: 
   static MissingValue& Instance()
   {
      static MissingValue singleton;
      return singleton;
   }
};

class StringValue : public ClinicalValue
{
private:
   std::string value;
public:
   StringValue(std::string n) : value(n) {}
   operator std::string() {return value;}
};

class BoolValue : public ClinicalValue
{
private:
   bool value;
public:
   BoolValue(bool n) : value(n) {}
   operator bool() {return value;}
};

class DateValue : public ClinicalValue
{
public:
   int day;
   int month; 
   int year; 

   DateValue(std::initializer_list<int> args)
   {
      day = *(args.begin());
      month = *(args.begin() + 1);
      year = *(args.begin() + 2);
      boost::gregorian::date temp(year, month, day);  //checks for valid date
   }
};

class DoubleValue : public ClinicalValue
{
private:
   double value;
public:
   DoubleValue(double n) : value(n) {}
   operator double() {return value;}
};

class IntValue : public ClinicalValue
{
private:
   int value;
public:
   IntValue(int n) : value(n) {}
   operator int() {return value;}
};

class HistoryValue : public ClinicalValue
{
private: 
   std::vector<std::pair<DateValue,std::shared_ptr<ClinicalValue>>> 
      history_vector;
public: 
   typedef std::pair<DateValue,std::shared_ptr<ClinicalValue>> history_pair;
   typedef std::vector<history_pair>::iterator iterator; 
   typedef std::vector<history_pair>::const_iterator const_iterator; 
   
   HistoryValue()
   {
      history_vector.reserve(1);
   }
   bool addDataPoint(const DateValue & d, ClinicalValue* v);
   bool removeDataPoint(const DateValue & d);
   ClinicalValue* valueAtDate(const DateValue & d) const; 
   const std::vector<ClinicalValue*> valuesAsVector() const;
   const std::vector<DateValue> datesAsVector() const;
   ClinicalValue* value(size_t i) const;
   size_t size() const;

   iterator begin() {return history_vector.begin();}
   const_iterator begin() const {return history_vector.begin();}
   iterator end() {return history_vector.end();}
   const_iterator end() const {return history_vector.end();}

};

#endif
