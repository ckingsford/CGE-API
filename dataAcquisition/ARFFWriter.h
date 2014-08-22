#ifndef ARFFWRITER_H
#define ARFFWRITER_H

#include "Genotype.h"
#include "StringFunctions.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace cge::patients;

namespace cge{
   namespace dataaquisition{

void writeARFFfromClinical(std::ostream& clin_file, Patient& P)
{
   std::shared_ptr<ClinicalRecord> clin_rec = P.clinicalRecord();
   
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
   
   //data writing
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
      clin_file << names.at(i) + ',' + req + '\n';
   }
}

void writeARFFfromGenomic(std::ostream& geno_file, Patient& P)
{
   std::shared_ptr<Genotype> geno = P.genotype();
   
   //header
   geno_file << "% 1. Title: CGE Genotype\n%\n";
   geno_file << "@RELATION genotype\n\n";
   //attributes
   geno_file << "@ATTRIBUTE name STRING\n"; 
   geno_file << "@ATTRIBUTE location STRING\n";//as chrom.pos.rs.ref
   geno_file << "@ATTRIBUTE variant_list STRING\n";
   geno_file << "@ATTRIBUTE variant STRING\n";

   //get info
   const std::vector<std::string> names = geno->schema()->fieldNames();
   std::vector<std::string> locations;
   for (auto i = geno->schema()->begin(); i != geno->schema()->end(); ++i){
      GenomicLocation loc = (*i)->location();
      std::string loc_string = std::to_string(loc.chromosome()) + ".";
      loc_string += std::to_string(loc.position()) + ".";
      loc_string += loc.rsNumber() + ".";
      loc_string += loc.refGenome()->name();
      locations.push_back(loc_string);
   }
   std::vector<std::string> variant_lists;
   for (auto i = geno->schema()->begin(); i != geno->schema()->end(); ++i){
      std::vector<std::string> v_list = (*i)->variantList();
      std::string result = "";
      for (auto const& s : v_list) 
         result += s + "\u001f";
      result.pop_back();
      variant_lists.push_back(result);   
   }
   std::vector<char> variants = geno->variantsAsVector();
   
   //data writing
   geno_file << "@data\n";
   for (size_t i = 0; i < names.size(); ++i){
      geno_file << names[i] + "," + locations[i] + "," + variant_lists[i] + ",";
      geno_file << variants[i] + "\n";
   }
}

}//dataaquisition
}//cge
#endif
