#ifndef GENOTYPESCHEMA_H
#define GENOTYPESCHEMA_H

#include "FieldSchema.h"
#include "VariantField.h"

class GenotypeSchema : public FieldSchema<VariantField>
{
public: 
   size_t indexOfLocation(const GenomicLocation& p) const
   {
      for(size_t i = 0; i < FieldSchema<VariantField>::size(); ++i){
      if ((FieldSchema<VariantField>::field(i) != NULL) && 
            ((FieldSchema<VariantField>::field(i)->location()) == p))
         return i;
      }
      return NonExistantField;
   }
   const VariantField* field(const GenomicLocation& p) const
   {
      return FieldSchema<VariantField>::field(indexOfLocation(p));
   }
};

#endif
