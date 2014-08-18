#ifndef MODELTRAINER_H
#define MODELTRAINER_H

#include "Model.h"

class ModelTrainer
{
public:
   virtual std::shared_ptr<Model> 
      train(const PatientSet& S, const std::string& V) = 0;
   virtual std::shared_ptr<Model> 
      train(const PatientSet& S, const std::vector<int>& V) = 0;
   virtual std::shared_ptr<Model> 
      train(const PatientSet& S, const std::vector<double>& V) = 0;
   bool isClassification() {return false;}
   bool isRegression() {return false;}
};
#endif
