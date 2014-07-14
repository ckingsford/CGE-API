#include "ClinicalRecord.h"
#include <iostream>
std::shared_ptr<ClinicalSchema> ClinicalRecord::schema() const
{
   return clinical_schema;
}

void ClinicalRecord::setSchema(std::shared_ptr<ClinicalSchema> S)
{
   clinical_schema = S;
}

void ClinicalRecord::setClinicalValue(size_t pos, ClinicalValue* v)
{
   //checks if pos represents a valid position on the schema
   if (pos >= (this->schema()->size()))
      throw std::out_of_range("This position doesn't exist in the schema");
   if (pos >= values.size())
      values.resize(this->schema()->size());
   std::shared_ptr<ClinicalValue> share_v(v);
   values.at(pos) = share_v;
}

void ClinicalRecord::setClinicalValue(const std::string& name, ClinicalValue* v)
{
   this->setClinicalValue(this->schema()->indexOfField(name), v); 
}

const ClinicalValue* ClinicalRecord::value(size_t pos) const
{
   if(pos >= values.size())
      throw std::out_of_range("This is nota valid position.");
   return (values.at(pos).get());
}

const ClinicalValue* ClinicalRecord::value(const std::string & name) const
{
   return this->value(this->schema()->indexOfField(name));
}

std::shared_ptr<ClinicalValue>& ClinicalRecord::operator[](size_t i)
{
   if(i >= values.size())
      throw std::out_of_range("Index out of bounds");
   return (this->values.at(i));
}

const std::shared_ptr<ClinicalValue> ClinicalRecord::operator[](size_t i) const
{
   if(i >= values.size())
      throw std::out_of_range("Index out of bounds");
   return (this->values.at(i));
}

std::shared_ptr<ClinicalValue>& ClinicalRecord::operator[]
   (const std::string& name)
{
   return (this->values.at(this->schema()->indexOfField(name)));
}

const std::shared_ptr<ClinicalValue> ClinicalRecord::operator[]
   (const std::string& name) const
{
   return (this->values.at(this->schema()->indexOfField(name)));
}

const std::vector<std::shared_ptr<const ClinicalValue>> 
      ClinicalRecord::valuesAsVector() const
{
   std::vector<std::shared_ptr<const ClinicalValue>> value_vector;
   for (size_t i = 0; i < values.size(); ++i)
      value_vector.push_back(values.at(i)); //makes each CV constant
   return value_vector;
}

