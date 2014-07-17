#ifndef GENOTYPE_H
#define GENOTYPE_H

#include <stdexcept>
#include <memory>
#include "Population.h"
#include "GenotypeSchema.h"
class Genotype
{
private: 
   std::shared_ptr<GenotypeSchema> genotype_schema;
   std::vector<char> variants;
   Population* pop;
public:
   Genotype()
   {
      variants.reserve(1);
   }
   std::shared_ptr<GenotypeSchema> schema();
   void setSchema(std::shared_ptr<GenotypeSchema> S);
   size_t size();
   void setVariant(size_t i, char v);
   void setVariant(GenomicLocation p, char v);
   char variant(const GenomicLocation & p);
   char variant(size_t i);
   const std::vector<char> variantsAsVector();
   Population* population();
   void setPopulation(Population* p);
};

#endif
