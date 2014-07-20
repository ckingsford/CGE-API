#ifndef PATIENT_H
#define PATIENT_H

#include "Genotype.h"
#include "ClinicalRecord.h"
#include "Value.h"
#include "Field.h"
#include <utility>

class PatientIterator;

class Patient
{
private:
   std::string patient_name;
   std::shared_ptr<ClinicalRecord> patient_clin_record;
   std::shared_ptr<Genotype> patient_genotype; 
public: 
   const std::string name();
   void setName(const std::string& n);
   std::shared_ptr<ClinicalRecord> clinicalRecord();
   void setClinicalRecord(std::shared_ptr<ClinicalRecord> r);
   std::shared_ptr<Genotype> genotype();
   void setGenotype(std::shared_ptr<Genotype> g);
   size_t size();
   Value value(size_t i) const;
   Field field(size_t i) const;
   PatientIterator begin();
   PatientIterator end();
};

class PatientIterator
{
private: 
   const Patient* pat;
   size_t pos;
public:
   PatientIterator (const Patient* p, size_t i) : pat(p), pos(i)
   { }

   bool operator!= (const PatientIterator& other) const
   {
      return pos != other.pos;
   }

   std::pair<std::shared_ptr<const Field>, std::shared_ptr<const Value>> 
      operator* () const
   {
      std::shared_ptr<const Field> f = 
         std::make_shared<const Field>(pat->field(pos));
      std::shared_ptr<const Value> v = 
         std::make_shared<const Value>(pat->value(pos));
      return std::make_pair(f,v);
   }

   const PatientIterator& operator++ ()
   {
      ++pos;
      return *this;
   }
   
   const PatientIterator& operator-- ()
   {
      --pos;
      return *this;
   }
};

#endif
