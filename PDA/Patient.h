#ifndef PATIENT_H
#define PATIENT_H

#include "Genotype.h"
#include "ClinicalRecord.h"
#include "Value.h"
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
   Value value(size_t i);
   //TODO iterator
};
#endif
