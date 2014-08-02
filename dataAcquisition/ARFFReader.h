#ifndef ARFFREADER_H
#define ARFFREADER_H

#include "Genotype.h"
#include "StringFunctions.h"
#include <iostream>
#include <fstream>
#include <sstream>

class ARFFReader
{
private:
   std::string filename;
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
};
#endif
