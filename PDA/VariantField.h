#ifndef VARIANTFIELD_H
#define VARIANTFIELD_H

#include<string>
#include<vector>
#include"GenomicLocation.h"

namespace cge{
   namespace patients{
   
class VariantField
{
private: 
   std::string variant_name;
   GenomicLocation variant_location;
   std::vector<std::string> seq_variants;
public:
   VariantField()
   {
      seq_variants.resize(256);
   }

   VariantField(std::string n, GenomicLocation l) : 
      variant_name(n), variant_location(l)
   {
      seq_variants.resize(256);
   }

   const std::string& name() const {return variant_name;}
   
   void setName(const std::string& name) {variant_name = name;}
   
   GenomicLocation location() const {return variant_location;}
   
   void setLocation(GenomicLocation p) {variant_location = p;}

   const std::vector<std::string> variantList() {return seq_variants;}

   void setVariantList(std::vector<std::string> s){seq_variants = s;}

   std::string variant(char i) const {return seq_variants.at(i);}

   void setVariant(char i, std::string s) {seq_variants.at(i) = s;}
};

}//namespace patients
}//namespace cge
#endif
