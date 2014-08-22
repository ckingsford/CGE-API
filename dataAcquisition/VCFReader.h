#ifndef VCFREADER_H
#define VECREADER_H

#include "Genotype.h"
#include "StringFunctions.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace cge::patients;

namespace cge{
   namespace dataaquisition{

std::shared_ptr<GenotypeSchema> readVCFtoGenotype(std::istream& input_stream)
{
   std::vector<GenomicLocation> location_list;
   std::vector<std::vector<std::string>> variant_list_list;
   std::string line;
   const int start_column = 9;
   
   while(input_stream.good()){
      std::getline(input_stream, line);
      line.shrink_to_fit();
      if (line.compare("") == 0 || line[0] == '#') //header
         continue;
      //setting SNP info
      else{
         std::vector<std::string> rec = utility::split(line, '\t');
         //records location
         GenomicLocation loc = 
            GenomicLocation(std::stoi(rec[0]), std::stoi(rec[1]));
         loc.setRSNumber(rec[2]);
         location_list.push_back(loc);
         
         //records variants(indicies)
         std::vector<std::string> variant_list;
         std::vector<std::string> alleles;
         alleles.push_back(rec[3]);
         std::vector<std::string> alt_alleles = utility::split(rec[4], ',');
         for (auto it = alt_alleles.begin(); it != alt_alleles.end(); ++it)
            alleles.push_back(*it);
         for(size_t i = start_column; i < rec.size(); ++i){
            std::string gt_data = (utility::split(rec[i], ':'))[0];
            int gt_1 = std::stoi((utility::split(gt_data,'|'))[0]);
            int gt_2 = std::stoi((utility::split(gt_data,'|'))[1]);
            variant_list.push_back(alleles[gt_1] + "|" + alleles[gt_2]);
         }
         variant_list_list.push_back(variant_list);
      }
   }
   //returns schema
   std::shared_ptr<GenotypeSchema> geno(new GenotypeSchema());
   for(size_t i = 0; i < location_list.size(); ++i){
      std::string field_name;
      if ((location_list[i].rsNumber().compare("rs") == 0)
            || (location_list[i].rsNumber().compare("") == 0))
         field_name = std::to_string(location_list[i].chromosome()) 
            + "." + std::to_string(location_list[i].position());
      VariantField* f = new VariantField();
      f->setName(field_name);
      f->setLocation(location_list[i]);
      f->setVariantList(variant_list_list[i]);
      geno->appendField(f);
   }
   return geno;

}


}//dataaquisition
}//cge
#endif
