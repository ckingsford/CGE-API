#ifndef VALUE_H
#define VALUE_H

#include "ClinicalValue.h"

namespace cge{
   namespace patients{
   
class Value
{
private:
   std::shared_ptr<const ClinicalValue> clin_value;
   std::shared_ptr<char> variant;
public: 
   Value(const ClinicalValue* c) : clin_value((std::shared_ptr<const ClinicalValue>)c)
   { 
      variant = std::shared_ptr<char> (nullptr);
   }
   Value(char v) : variant(std::make_shared<char>(v))
   { 
      clin_value = std::shared_ptr<ClinicalValue> (nullptr);
   }
   bool isClinical()
   {
      return (clin_value.get() != nullptr);
   }
   bool isVariant()
   {
      return (variant.get() != nullptr);
   }
   char getVariant()
   {
      return *variant;
   }
   const ClinicalValue* getClinical()
   {
      return clin_value.get();
   }

};

}//namespace patients
}//namespace cge
#endif
