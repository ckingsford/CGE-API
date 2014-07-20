#include "Genotype.h"

std::shared_ptr<GenotypeSchema> Genotype::schema()
{
  return genotype_schema;
}

void Genotype::setSchema(std::shared_ptr<GenotypeSchema> S)
{
   genotype_schema = S;
}

size_t Genotype::size()
{
   return this->schema()->size();
}

void Genotype::setVariant(size_t i, char v)
{
   if (i >= (this->size()))
      throw std::out_of_range("This position doesn't exist in the schema");
   if (i >= variants.size())
      variants.resize(this->size());
   if (this->schema()->field(i)->variant(v) == "")
      throw std::out_of_range("No variant exists at this index.");
   variants.at(i) = v;
   
}

void Genotype::setVariant(GenomicLocation p, char v)
{
   this->setVariant((this->schema()->indexOfLocation(p)), v);
}

char Genotype::variant(const GenomicLocation & p)
{
   return this->variant(this->schema()->indexOfLocation(p));
}

char Genotype::variant(size_t i)
{
   if(i >= variants.size())
      throw std::out_of_range("This is not a valid position.");
   return (variants.at(i));
}

const std::vector<char> Genotype::variantsAsVector()
{
   return variants;
}

Population* Genotype::population()
{
   return pop;
}

void Genotype::setPopulation(Population* p)
{
   pop = p;
}
