#ifndef CLINICALSCHEMA_H
#define CLINICALSCHEMA_H

#include "FieldSchema.h"
#include "PatientField.h"
   
namespace cge{
   namespace patients{

class ClinicalSchema : public FieldSchema<ClinicalField>
{
public:
   bool isRequired(const std::string & name) const
   {
      return field(name)->isRequired();
   }
};

}//namespace patients
}//namespace cge
#endif
