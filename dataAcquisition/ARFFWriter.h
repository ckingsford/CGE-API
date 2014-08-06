#ifndef ARFFWRITER_H
#define ARFFWRITER_H

#include "Genotype.h"
#include "StringFunctions.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace cge::patients;

class ARFFWriter
{
private:
   std::string filename;
public:
   ARFFWriter(std::string name)
   {
      bool isValidExt = false;
      if (name.length() > 5)
         isValidExt = (0 == name.compare(name.length() - 5, 5, ".arff"));
      if (!isValidExt && name.length() > 9)
         isValidExt = 
            (0 == name.compare(name.length() - 9, 9, ".arff.txt"));
      if(!isValidExt)
         throw std::invalid_argument("Invalid file type");

      filename = name;
   }
   
   void writeClinical(std::shared_ptr<ClinicalRecord> clin_rec);
   void writeGenomic(Genotype geno);
};

void ARFFWriter::writeClinical(std::shared_ptr<ClinicalRecord> clin_rec)
{
   std::ofstream clin_file;
   clin_file.open(filename);
   
   //header
   clin_file << "% 1. Title: CGE Clinical Record\n%\n";
   clin_file << "@RELATION clinical_record\n\n";
   //attributes
   clin_file << "@ATTRIBUTE string_value STRING\n"; 
   clin_file << "@ATTRIBUTE bool_value {true, false}\n";
   clin_file << "@ATTRIBUTE date_value date [yyyy-MM-dd]\n";
   clin_file << "@ATTRIBUTE int_value NUMERIC\n";
   clin_file << "@ATTRIBUTE double_value NUMERIC\n";
   clin_file << "@ATTRIBUTE history_value STRING\n";
   clin_file << "@ATTRIBUTE name STRING\n";
   clin_file << "@ATTRIBUTE required {true,false}\n";
   
   //get all info
   const std::vector<std::shared_ptr<const ClinicalValue>> values = 
      clin_rec->valuesAsVector();
   const std::vector<std::string> names= clin_rec->schema()->fieldNames();
   std::vector<bool> requires;
   for(auto it = names.begin(); it != names.end(); ++it)
     requires.push_back(clin_rec->schema()->isRequired(*it));
   
   //data
   clin_file << "@data\n";
   for (size_t i = 0; i < values.size(); ++i){
      std::string val = values.at(i)->toString();
      if(values.at(i)->type().compare("String") == 0){
         clin_file << 
            "" + val + ",?,?,?,?,?,";
      }
      else if (values.at(i)->type().compare("Bool") == 0){
         clin_file << 
            "?," + val + ",?,?,?,?,";
      }
      else if (values.at(i)->type().compare("Double") == 0){
         clin_file << 
            "?,?," + val + ",?,?,?,";
      }
      else if (values.at(i)->type().compare("Int") == 0){
         clin_file << 
            "?,?,?," + val + ",?,?,";

      }
      else if (values.at(i)->type().compare("Date") == 0){
         clin_file << 
            "?,?,?,?," + val + ",?,";
      }
      else if (values.at(i)->type().compare("History") == 0){
         clin_file << 
            "?,?,?,?,?," + val + ",";
      }
      else{
         clin_file << "?,?,?,?,?,?,";
      }
      std::string req;
      if (requires.at(i))
         req = "true";
      else
         req = "false";
      clin_file << names.at(i) + ',' + req;
   }
   clin_file.close(); 
}

#endif
