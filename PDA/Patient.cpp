#include "Patient.h"

const std::string Patient::name()
{
   return patient_name;
}

void Patient::setName(const std::string& n)
{
   patient_name = n;
}

std::shared_ptr<ClinicalRecord> Patient::clinicalRecord()
{
   return patient_clin_record;
}

void Patient::setClinicalRecord(std::shared_ptr<ClinicalRecord> r)
{
   patient_clin_record = r;
}

std::shared_ptr<Genotype> Patient::genotype()
{
   return patient_genotype;
}

void Patient::setGenotype(std::shared_ptr<Genotype> g)
{
   patient_genotype = g;
}

size_t Patient::size()
{
   size_t clin_size = patient_clin_record->schema()->size();
   size_t genotype_size = patient_genotype->schema()->size();
   return (clin_size + genotype_size);
}
