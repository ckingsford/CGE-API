#ifndef ARFFREADER_H
#define ARFFREADER_H

#include "PatientSet.h"
#include "StringFunctions.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace cge::patients;

class ARFFReader
{
private:
   std::string filename;
   std::shared_ptr<ClinicalRecord> clin_rec;
   std::shared_ptr<Genotype> geno;
public:
   ARFFReader(std::string name)
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
   void readClinical();
   void readGenomic();
};

void ARFFReader::readClinical()
{
   std::ifstream clin_file;
   clin_file.open(filename);
   std::string line;
   std::vector<std::string> line_info;
   size_t pos = 0;
   while(clin_file.good()){
      std::getline(clin_file, line);
      if(line[0] == '%')
         continue;
      line_info = utility::split(line, ',');
      bool is_req = (line_info[7].compare("true") == 0);
      clin_rec->schema()->appendField(new ClinicalField(line_info[6], is_req));
      if(((line_info[0]).size() > 1) || ((line_info[0])[0] != '?')){//string
         clin_rec->setClinicalValue(pos, new StringValue(line_info[0]));
      }
      else if(((line_info[1]).size() > 1) || ((line_info[1])[0] != '?')){//bool
         clin_rec->setClinicalValue(pos, new BoolValue(line_info[1]));
      }
      else if(((line_info[2]).size() > 1) || ((line_info[2])[0] != '?')){//doub
         clin_rec->setClinicalValue(pos, new DoubleValue(line_info[2]));
      }
      else if(((line_info[3]).size() > 1) || ((line_info[3])[0] != '?')){//int
         clin_rec->setClinicalValue(pos, new IntValue(line_info[3]));
      }
      else if(((line_info[4]).size() > 1) || ((line_info[4])[0] != '?')){//date
         clin_rec->setClinicalValue(pos, new DateValue(line_info[4]));
      }
      else if(((line_info[5]).size() > 1) || ((line_info[5])[0] != '?')){//hist
         clin_rec->setClinicalValue(pos, new HistoryValue(line_info[5]));
      }
      else{ //missing
         clin_rec->setClinicalValue(pos, &MissingValue::Instance());
      } 
      ++pos;
   }
   
   clin_file.close();
}

void ARFFReader::readGenomic()
{
   std::ifstream geno_file;
   geno_file.open(filename);
   std::string line;
   std::vector<std::string> line_info;
   std::vector<std::string> loc_info;
   std::vector<std::string> variant_list;
   while(geno_file.good()){
      std::getline(geno_file, line);
      if(line[0] == '%')
         continue;
      line_info = utility::split(line, ',');
      loc_info = utility::split(line_info[1], '.');
      variant_list = utility::split(line_info[2],'\u001f');
      VariantField* temp_field = new VariantField();
      temp_field->setName(line_info[0]);
      temp_field->setVariantList(variant_list);
      GenomicLocation loc; 
      //When more references are added this will be expanded
      if (loc_info[3].compare("GENOME_HG19"))
         loc = GenomicLocation(std::stoi(loc_info[0]), std::stoi(loc_info[1]), 
               GENOME_HG19::Instance());
      else//default case
         loc = GenomicLocation(std::stoi(loc_info[0]), std::stoi(loc_info[1]));
      loc.setRSNumber(loc_info[2]);
      temp_field->setLocation(loc);

      geno->schema()->appendField(temp_field);
      char v = (line_info[3])[0];
      geno->setVariant(loc, v);
   }
   geno_file.close();
}
#endif
