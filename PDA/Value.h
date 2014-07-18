#ifndef VALUE_H
#define VALUE_H

#include "ClinicalValue.h"

class Value
{
private:
   ClinicalValue* clin_value;
   char* variant;
public: 
   Value(ClinicalValue* c) : clin_value(c) 
   { 
      variant = nullptr;
   }
   Value(char* v) : variant(v)
   {
      clin_value = nullptr;
   }
   bool isClinical()
   {
      return (clin_value != nullptr);
   }
   bool isVariant()
   {
      return (variant != nullptr);
   }
   char* getVariant()
   {
      return variant;
   }
   ClinicalValue* getClinical()
   {
      return clin_value;
   }

};

#endif
