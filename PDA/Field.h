#ifndef FIELD_H
#define FIELD_H

#include "VariantField.h"
#include "PatientField.h"

namespace cge{
   namespace patients{
   
class Field
{
private: 
   std::shared_ptr<const ClinicalField> clin_field;
   std::shared_ptr<const VariantField> var_field;
public:
   Field(const ClinicalField* c) : 
      clin_field((std::shared_ptr<const ClinicalField>) c),
      var_field(std::shared_ptr<const VariantField>(nullptr))
   { }
   Field(const VariantField* v) : 
      clin_field (std::shared_ptr<const ClinicalField>(nullptr)),
      var_field((std::shared_ptr<const VariantField>) v)  
   { }
   bool isClinical()
   {
      return (clin_field.get() != nullptr);
   }
   bool isVariant()
   {
      return (var_field.get() != nullptr);
   }
   std::shared_ptr<const VariantField> getVariant()
   {
      return var_field;
   }
   std::shared_ptr<const ClinicalField> getClinical()
   {
      return clin_field;
   }
};

}//namespace patients
}//namespace cge
#endif
