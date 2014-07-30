#ifndef VCFREADER_H
#define VECREADER_H

#include "Genotype.h"
#include <iostream>
#include <fstream>

using namespace cge::patients;

class VCFReader
{
private: 
   std::vector<std::string> variant_list;
   std::string filename;
public: 
   VCFReader(std::string name)
   {
      bool isValidExt = false;
      if (name.length() > 4)
         isValidExt = (0 == name.compare(name.length() - 4, 4, ".vcf"));
      if (!isValidExt && name.length() > 8)
         isValidExt = 
            (0 == name.compare(name.length() - 4, 4, ".vcf.txt"));
      if(!isValidExt)
         throw std::invalid_argument("Invalid file type");

      filename = name;
   }
   void read();
};

void VCFReader::read()
{
   //counts number of lines
   int number_of_lines = 0;
   std::ifstream input_stream;
   std::string line;
   input_stream.open(filename);
   while (input_stream.good()){
      std::getline(input_stream, line);
      ++number_of_lines;
   }
   input_stream.close();

   int header_lines = 0;
   int current_snp_index = 0;
   int line_index = 0;
   const int patient_start_column = 9;
   input_stream.open(filename);
   
   while(input_stream.good()){
      std::getline(input_stream, line);
      line.shrink_to_fit();
      if (line.compare("") == 0 || line.compare(0,2,"##"))
         header_lines++;
      //Headerline
      else if (line.compare(0,1,"#")){
      //TODO
      }
      //setting SNP info
      else{
      //TODO
      }
   }
}

#endif
