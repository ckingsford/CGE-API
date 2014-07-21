#ifndef PATIENTSET_H
#define PATIENTSET_H

#include "Patient.h"

namespace cge{
   namespace patients{
   
using PatientPredicate = std::function<bool(const Patient& P)>;
class PatientSet
{
private: 
   std::vector<std::shared_ptr<Patient>> patient_set;
public:
   typedef std::vector<std::shared_ptr<Patient>>::iterator iterator;
   typedef std::vector<std::shared_ptr<Patient>>::const_iterator const_iterator;
   
   PatientSet()
   {
      patient_set.reserve(1);
   }
   void appendPatient(std::shared_ptr<Patient> P);
   void removePatient(size_t i);
   const std::vector<std::shared_ptr<const Patient>> asVector();
   PatientSet select(PatientPredicate f);
   void restrict(PatientPredicate f);
   void discard(PatientPredicate f);

   std::shared_ptr<Patient> operator[](size_t i);
   const std::shared_ptr<Patient> operator[](size_t i) const;

   iterator begin() {return patient_set.begin();}
   const_iterator begin() const {return patient_set.begin();}
   iterator end() {return patient_set.end();}
   const_iterator end() const {return patient_set.end();}
};

}//namespace patients
}//namespace cge
#endif
