#ifndef CLINICALSCHEMA_H
#define CLINICALSCHEMA_H

#include "FieldSchema.h"
#include "PatientField.h"

class ClinicalSchema : public FieldSchema<ClinicalField>
{
public:
   bool isRequired(const std::string & name) const
   {
      return field(name)->isRequired();
   }
};
#endif
