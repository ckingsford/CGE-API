#ifndef CLINICALRECORD_H
#define CLINICALRECORD_H

#include "ClinicalSchema.h"
#include "ClinicalValue.h"

class ClinicalRecord
{
private: 
   std::shared_ptr<ClinicalSchema> clinical_schema;
   std::vector<std::shared_ptr<ClinicalValue>> values;
public:
   ClinicalRecord()
   {
      values.reserve(1);
   }
   std::shared_ptr<ClinicalSchema> schema() const;
   void setSchema(std::shared_ptr<ClinicalSchema> S);
   void setClinicalValue(size_t pos, ClinicalValue* v);
   void setClinicalValue(const std::string& name, ClinicalValue* v);
   const ClinicalValue* value(size_t pos) const;
   const ClinicalValue* value(const std::string & name) const;
   std::shared_ptr<ClinicalValue>& operator[](size_t i);
   const std::shared_ptr<ClinicalValue> operator[](size_t i) const;
   std::shared_ptr<ClinicalValue>& operator[](const std::string& name);
   const std::shared_ptr<ClinicalValue> operator[](const std::string& name) const;
   const std::vector<std::shared_ptr<const ClinicalValue>> 
      valuesAsVector() const;
};
#endif
