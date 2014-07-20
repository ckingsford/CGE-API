#include "PatientSet.h"

void PatientSet::appendPatient(std::shared_ptr<Patient> P)
{
   patient_set.push_back(P);
}

void PatientSet::removePatient(size_t i)
{
   patient_set.erase(patient_set.begin() + i);
}

const std::vector<std::shared_ptr<const Patient>> PatientSet::asVector()
{
   std::vector<std::shared_ptr<const Patient>> const_patient_set;
   for(size_t i = 0; i < patient_set.size(); ++i)
      const_patient_set.push_back(patient_set.at(i));
   return (const std::vector<std::shared_ptr<const Patient>>)const_patient_set;
}

PatientSet PatientSet::select(PatientPredicate f)
{
   PatientSet new_set;
   for(std::shared_ptr<Patient> p : patient_set){
      if (f(*p)) 
         new_set.appendPatient(p);
   }
   return new_set;
}

void PatientSet::restrict(PatientPredicate f)
{
   bool removed_one = false;
   for(size_t i = 0; i < patient_set.size(); ++i){
      if (removed_one){
         --i;
         removed_one = false;
      }
      if(!f(*(patient_set.at(i)))){
         this->removePatient(i);
         removed_one = true;
      }
   }
}

void PatientSet::discard(PatientPredicate f)
{
   bool removed_one = false;
   for(size_t i = 0; i < patient_set.size(); ++i){
      if (removed_one){
         --i;
         removed_one = false;
      }
      if(f(*(patient_set.at(i)))){
         this->removePatient(i);
         removed_one = true;
      }
   }
}

std::shared_ptr<Patient> PatientSet::operator[](size_t i)
{
   if (i >= patient_set.size())
      throw std::out_of_range("Index out of bounds");
   return patient_set.at(i);
}

const std::shared_ptr<Patient> PatientSet::operator[](size_t i) const
{
   if (i >= patient_set.size())
      throw std::out_of_range("Index out of bounds");
   return patient_set.at(i);
}


