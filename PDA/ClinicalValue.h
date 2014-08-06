#ifndef CLINICALVALUE_H
#define CLINICALVALUE_H

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include "Date.h"

namespace cge{
   namespace patients{
  
class ClinicalValue
{
public: 
   virtual ~ClinicalValue() = 0;   //Destructor
   virtual const std::string type() const = 0;
   virtual const std::string toString() const = 0;
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
   const std::string type() const {return "Missing";}
   const std::string toString() const {return "?";}
};

class StringValue : public ClinicalValue
{
private:
   std::string value;
public:
   StringValue(std::string n) : value(n) {}
   operator std::string() {return value;}
   const std::string type() const {return "String";}
   const std::string toString() const {return value;}
};

class BoolValue : public ClinicalValue
{
private:
   bool value;
public:
   BoolValue(bool n) : value(n) {}
   operator bool() {return value;}
   const std::string type() const {return "Bool";}
   const std::string toString() const
   {
   if (value)
      return "true";
   else
      return "false";
   }
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
      if (!date::isValidDate(day, month, year))
            throw std::invalid_argument("Invalid Date");
   }
   const std::string toString() const
   {
      std::string s_year = std::to_string(year);
      std::string s_month = std::to_string(month);
      std::string s_day = std::to_string(day);

      return s_year + "-" + s_month + "-" + s_day;
   }
   const std::string type() const {return "Date";}
};

class DoubleValue : public ClinicalValue
{
private:
   double value;
public:
   DoubleValue(double n) : value(n) {}
   operator double() {return value;}
   const std::string type() const {return "Double";}
   const std::string toString() const {return (std::to_string(value));}
};

class IntValue : public ClinicalValue
{
private:
   int value;
public:
   IntValue(int n) : value(n) {}
   operator int() {return value;}
   const std::string type() const {return "Int";}
   const std::string toString() const {return (std::to_string(value));}
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
   
   const std::string type() const {return "History";}
   const std::string toString() const
   {
      std::string hist_line;
      const std::vector<ClinicalValue*> hist_vals = valuesAsVector();
      const std::vector<DateValue> hist_dates = datesAsVector();
      ClinicalValue* temp;
         for (size_t i = 0; i < hist_vals.size(); ++i){
            temp = hist_vals.at(i);
            hist_line.append(temp->type() + '\t');
            hist_line.append(temp->toString() + '\t');
            hist_line.append(hist_dates.at(i).toString());
         
            if (i + 1 < hist_vals.size())
               hist_line.append("&");
         }
      return hist_line;
   }

};

}//namespace patients
}//namespace cge
#endif
