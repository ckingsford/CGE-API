#include "ClinicalValue.h"

int compareDate(DateValue a_date, DateValue b_date) 
//0 if dates are equal, negative if a comes first, positive if b comes first
{
   if (a_date.year == b_date.year){
      if (a_date.month == b_date.month){
         return (a_date.day - b_date.day);
      }
      else{
         return (a_date.month - b_date.month);
      }
   }
   else
      return (a_date.year - b_date.year);
}

bool HistoryValue::addDataPoint(const DateValue & d, ClinicalValue * v)
{  
   std::shared_ptr<ClinicalValue> share_v(v);
   history_pair new_point = std::make_pair(d, share_v);
   iterator i = begin();
   while (i != end() && compareDate((*i).first, new_point.first) < 0){
      ++i;
   }
   if (compareDate((*i).first, new_point.first) == 0)
      return false;
   history_vector.insert (i, new_point);  
   return true;
}

bool HistoryValue::removeDataPoint(const DateValue & d)
{
   iterator i = begin();
   while (i != end()){
      if (compareDate(d, (*i).first) == 0){
         history_vector.erase(i);
         return true;
      }
      ++i;
   }
   return false;
}

ClinicalValue* HistoryValue::valueAtDate(const DateValue & d) const 
{
   const_iterator i = begin();
   ClinicalValue* result = &(MissingValue::Instance());
   while (i != end()){
      if (compareDate(d, (*i).first) >= 0){
         result = ((*i).second).get();  
      }
      ++i;
   } 
   return result;
}

const std::vector<ClinicalValue *> HistoryValue::valuesAsVector() const
{
   std::vector<ClinicalValue *> temp;
   for (const_iterator i = begin(); i != end(); ++i)
      temp.push_back (((*i).second).get());
   const std::vector<ClinicalValue *> val_vector = temp;
   return val_vector;
}

const std::vector<DateValue> HistoryValue::datesAsVector() const
{
   std::vector<DateValue> temp;
   for (const_iterator i = begin(); i != end(); ++i)
      temp.push_back ((*i).first);
   const std::vector<DateValue> date_vector = temp;
   return date_vector;
}

ClinicalValue* HistoryValue::value(size_t i) const
{
   if (i >= history_vector.size())
      return &(MissingValue::Instance());
   return ((history_vector.at(1)).second).get();
}

size_t HistoryValue::size() const 
{
   return history_vector.size();
}
