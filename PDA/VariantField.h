#ifndef VARIANTFIELD_H
#define VARIANTFIELD_H

#include<string>
#include"GenomicLocation.h"

class VariantField
{
private: 
   std::string variant_name;
   GenomicLocation variant_location;
public:
   VariantField(std::string n, GenomicLocation l) : 
      variant_name(n), variant_location(l){ }

   const std::string& name() {return variant_name;}
   
   void setName(const std::string& name) {variant_name = name;}
   
   GenomicLocation location() {return variant_location;}
   
   void setLocation(GenomicLocation p) {variant_location = p;}

   //variantList and variant?
};

#endif
