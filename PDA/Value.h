#ifndef VALUE_H
#define VALUE_H

#include "ClinicalValue.h"

class Value
{
private:
   std::shared_ptr<ClinicalValue> clin_value;
   std::shared_ptr<char> variant;
public: 
   Value(std::shared_ptr<ClinicalValue> c) : clin_value(c)
   { }
   Value(char v) : variant(std::make_shared<char>(v))
   { }
   bool isClinical()
   {
      return (clin_value != nullptr);
   }
   bool isVariant()
   {
      return (variant != nullptr);
   }
   char getVariant()
   {
      return *variant;
   }
   ClinicalValue* getClinical()
   {
      return clin_value.get();
   }

};

#endif
