#ifndef ARFFREADER_H
#define ARFFREADER_H

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
   
   void writeClinical(ClinicalRecord clin_rec);
   void writeGenomic(Genotype geno);
};

ARFFWriter::writeClinical(ClinicalRecord clin_rec)
{
   ofstream clin_file;
   clin_file.open(filename);
   //header
   clin_file << "% 1. Title: CGE Clinical Record\n%\n";
   clin_file << "@RELATION clinical_record\n\n";

}

#endif
