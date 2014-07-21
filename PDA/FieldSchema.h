#ifndef FIELDSCHEMA_H
#define FIELDSCHEMA_H

#include<string>
#include<vector>
#include<stdexcept>
#include<limits>
#include<algorithm>

namespace cge{
   namespace patients{

template <typename T>
class FieldSchema
{
private: 
   std::vector<T*> field_list;
   std::vector<bool> visible_list;
   void swap(int i, int j);
public: 
   const size_t NonExistantField = std::numeric_limits<size_t>::max(); 
   FieldSchema()
   {
      field_list.reserve(1);
      visible_list.reserve(1);
   }
   size_t appendField (T* f);
   void setField(size_t i, T* f);
   void removeField(size_t i);
   bool restrictToFields(const std::vector<std::string> & names);
   void clearFieldRestriction();
   void removeField(const std::string & name);
   size_t size() const;
   size_t indexOfField(const std::string & name) const;
   const T* field(size_t i) const;
   const T* field(const std::string & name) const;
   bool hasField(const std::string & name) const;
   std::string name(size_t i);
   const std::vector<std::string> fieldNames() const;
   void setFieldOrder(const std::vector<std::string> & order);
   typename std::vector<T*>::iterator begin()
   {
      return field_list.begin();
   }
   typename std::vector<T*>::iterator end()
   {
      return field_list.end();
   }
};


//implementation---------------------------------------------------------------
template <typename T>
size_t FieldSchema<T>::appendField (T* f)
{
   //check if field is already in schema
   for(auto it = begin();it!=end();++it){
      if ((*it) != NULL &&(((*it)->name()).compare(f->name()) == 0))
         throw std::invalid_argument ("This field already exists.");
   }
   //inserts field
   field_list.push_back(f);
   visible_list.push_back(true);
   return ((this->size()) - 1);
}

template <typename T>
void FieldSchema<T>::setField(size_t i, T* f)
{
   //check if field is already in schema
   for(auto it = begin();it!=end();++it){
      if ((*it) != NULL && (((*it)->name()).compare(f->name()) == 0))
         throw std::invalid_argument ("This field already exists.");
   }
   //check if size of field_list is big enough
   if (size() <= i){
      field_list.resize(i + 1);
      visible_list.resize(i + 1, true);
   }
   //adds f
   field_list.at(i) = f;
   visible_list.at(i) = true;
}

template <typename T>
void FieldSchema<T>::removeField(size_t i)
{
   if (size() <= i || (visible_list.at(i)) == false)
      throw std::out_of_range("This field does not exist in the schema.");
   else{
      field_list.erase(begin() + i);
      visible_list.erase(visible_list.begin() + i);
   }
}

template <typename T>
bool FieldSchema<T>::restrictToFields(const std::vector<std::string> & names)
{
   //remove old restrictions
   clearFieldRestriction();
   //check if everything in names exists
   bool all_exist = true;
   for(auto i = names.begin(); i != names.end(); ++i)
      if(!hasField(*i))
         all_exist = false;
   //sets new restrictions
   for(int i = 0; i < size(); ++i){
      if(std::find(names.begin(), names.end(), name(i)) == names.end())
         visible_list.at(i) = false;
   }
   return all_exist;
}

template <typename T>
void FieldSchema<T>::clearFieldRestriction()
{
   for(int i = 0; i < visible_list.size(); ++i)
      visible_list.at(i) = true;
}

template <typename T>
void FieldSchema<T>::removeField(const std::string & name)
{
   this->removeField(indexOfField(name));
}

template <typename T>
size_t FieldSchema<T>::size() const
{
   return field_list.size();
}

template <typename T>
size_t FieldSchema<T>::indexOfField(const std::string & name) const 
{
   for (size_t i = 0; i < size(); ++i){
      if ((field(i) != NULL) && ((field(i)->name()).compare(name) == 0))
         return i;
   }
   return NonExistantField;
}

template <typename T>
const T* FieldSchema<T>::field(size_t i) const
{
   if (visible_list.at(i) == false || i >= size() || i == NonExistantField)
      throw std::invalid_argument("There is no field at this index");
   if (field_list.at(i) == NULL)
      throw std::invalid_argument("This field is NULL");
   return field_list.at(i);
}

template <typename T>
const T* FieldSchema<T>::field(const std::string & name) const
{
   return this->field(this->indexOfField(name));
}

template <typename T>
bool FieldSchema<T>::hasField(const std::string & name) const
{
   size_t i = this->indexOfField(name);
   return (i != NonExistantField);
}

template <typename T>
std::string FieldSchema<T>::name(size_t i)
{
   return this->field(i)->name();
}

template <typename T>
const std::vector<std::string> FieldSchema<T>::fieldNames() const
{
   std::vector<std::string> field_names;
   for (size_t i = 0; i<(this->size()); ++i){
      if(visible_list.at(i) == true && field_list.at(i) != NULL)
         field_names.push_back(this->field(i)->name());
   }
   const std::vector<std::string> final_names(field_names);
   return final_names;
}

template <typename T>
void FieldSchema<T>::swap(int i, int j)
{
   T* temp =  this->field(i);
   field_list.at(i) = this->field(j);
   field_list.at(j) = temp;
}

template <typename T>
void FieldSchema<T>::setFieldOrder(const std::vector<std::string> & order)
{
   //check if order is a valid permutation
   if(!std::is_permutation(order.begin(),order.end(),(fieldNames().begin())))
      throw std::invalid_argument("Not a valid permuation of the field names.");
   int i = 0;
   for(int j = 0; j < order.size(); j++){
      while (visible_list.at(i) != true)
         i++;
      this->swap(i,(this->indexOfField(order.at(j)))); 
      i++;
   }
} 

}//namespace patients
}//namespace cge
#endif
