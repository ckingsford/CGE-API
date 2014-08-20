#ifndef PREDICTION_H
#define PREDICTION_H

#include "Patient.h"
#include "Model.h"

class Model;

class Prediction
{
private:
   std::weak_ptr<Model> mod;
   std::weak_ptr<Patient> pat;
   double val;
   std::vector<double> conf_int;
   std::vector<double> pred_fea_weights;
public:
   Prediction(std::shared_ptr<Model> m, std::shared_ptr<Patient> p)
   {
      mod = m;
      pat = p;
   }
   void setValue(double v)
   {
      val = v;
   }
   const double value() const
   {
      return val;
   }
   void setConfidenceInterval(std::vector<double> ci)
   {
      conf_int = ci;
   }
   const std::vector<double> confidenceInterval() const
   {
      return conf_int;
   }
   const std::weak_ptr<Model> model() const
   {
      return mod;
   }
   const std::weak_ptr<Patient> patient() const
   {
      return pat;
   }
   void setPredictionFeatureWeights(std::vector<double> pfw)
   {
      pred_fea_weights = pfw;
   }
   std::vector<double> predictionFeatureWeights() const
   {
      return pred_fea_weights;
   }
};





#endif
