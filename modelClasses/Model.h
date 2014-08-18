#ifndef MODEL_H
#define MODEL_H

#include "Prediction.h"
#include "ModelTrainer.h"
#include "PatientSet.h"
#include <iostream>

class ModelTrainer;

class Model
{
private:
   std::shared_ptr<ModelTrainer> m_trainer;
public:
   virtual Prediction apply(const Patient & P) = 0;
   virtual Prediction apply(const PatientSet & P) = 0;
   const std::shared_ptr<ModelTrainer> trainer() const {return m_trainer;}
   bool isClassification() const;
   bool isRegression() const;
   virtual const std::vector<std::string> featureNames() const = 0;
   virtual const std::vector<double> featureWeights() const = 0;
   virtual const std::vector<std::string> requiredClinicalFields() = 0;
   virtual const std::vector<std::string> requiredGenomicFields() = 0;
   virtual bool save(std::ostream& o) const = 0;
   virtual bool read(std::istream& i) = 0; 
};
#endif
