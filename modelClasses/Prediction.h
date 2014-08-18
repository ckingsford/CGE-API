#ifndef PREDICTION_H
#define PREDICTION_H

#include "Patient.h"
#include "Model.h"

class Model;

class Prediction
{
public:
   Prediction(std::shared_ptr<Model>, std::shared_ptr<Patient>);
   const double value() const;
   const std::vector<double> confidenceInterval() const;
   const std::weak_ptr<Model> model() const;
   const std::weak_ptr<Patient> patient() const;
   std::vector<double> predictionFeatureWeights() const;
};
#endif
