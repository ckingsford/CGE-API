#ifndef GENOTYPESCHEMA_H
#define GENOTYPESCHEMA_H

#include "FieldSchema.h"
#include "VariantField.h"

namespace cge{
   namespace patients{
   
class GenotypeSchema : public FieldSchema<VariantField>
{
public: 
   //Theres are rewritten since GenotypeScema declares a new field function.
   const VariantField* field(size_t i) const
   {
      return FieldSchema<VariantField>::field(i);
   }
   const VariantField* field(const std::string& name) const
   {
      return FieldSchema<VariantField>::field(name);
   }
   
   size_t indexOfLocation(const GenomicLocation& p) const
   {
      for(size_t i = 0; i < size(); ++i){
      if ((field(i) != NULL) && ((field(i)->location()) == p))
         return i;
      }
      return NonExistantField;
   }
   const VariantField* field(const GenomicLocation& p) const
   {
      return field(indexOfLocation(p));
   }
 
};

}//namespace patients
}//namespace cge
#endif
