#ifndef VCFREADER_H
#define VECREADER_H

#include "Genotype.h"
#include "StringFunctions.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace cge::patients;

class VCFReader
{
private: 
   std::vector<GenomicLocation> location_list;
   std::vector<std::vector<std::string>> variant_list_list;
   std::string filename;

public: 
   VCFReader(std::string name)
   {
      bool isValidExt = false;
      if (name.length() > 4)
         isValidExt = (0 == name.compare(name.length() - 4, 4, ".vcf"));
      if (!isValidExt && name.length() > 8)
         isValidExt = 
            (0 == name.compare(name.length() - 8, 8, ".vcf.txt"));
      if(!isValidExt)
         throw std::invalid_argument("Invalid file type");

      filename = name;
   }
   void read();
   std::shared_ptr<GenotypeSchema> get_info();
};

void VCFReader::read()
{
   //counts number of lines
   std::ifstream input_stream;
   std::string line;
   const int start_column = 9;
   input_stream.open(filename);
   
   while(input_stream.good()){
      std::getline(input_stream, line);
      line.shrink_to_fit();
      if (line.compare("") == 0 || line.compare(0,2,"##") //metadata
            || line.compare(0,1,"#")) //header
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
}

std::shared_ptr<GenotypeSchema> VCFReader::get_info()
{
   std::shared_ptr<GenotypeSchema> geno(new GenotypeSchema());
   for(size_t i = 0; i < location_list.size(); ++i){
      VariantField* f = new VariantField();
      f->setName((location_list[i]).rsNumber());
      f->setLocation(location_list[i]);
      f->setVariantList(variant_list_list[i]);
      geno->appendField(f);
   }
   return geno;
}

#endif
