#ifndef PATIENTFIELD_H
#define PATIENTFIELD_H

#include <string>
#include <utility>
class PatientField
{
private:
   std::string field_name;
   bool required;
public: 
   PatientField(const std::string & n, bool r) : field_name(n), required(r) { }
   PatientField(const std::string & n) : field_name(n), required(true) { }
   //PatientField(std::initializer_list<T> il) : 
   //name(*(args.begin())), required((args.begin()+1)*)
   void setName(const std::string & n) {field_name = n;}
   const std::string name() {return field_name;}
   void setRequired(bool req) {required = req;}
   bool isRequired() {return required;}
};

class ClinicalField : public PatientField
{
public:
   ClinicalField(const std::string & n, bool r) : PatientField(n,r) { }
   ClinicalField(const std::string & n) : PatientField(n) { }

};

#endif 
