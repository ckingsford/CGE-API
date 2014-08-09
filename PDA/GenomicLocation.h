#ifndef GENOMICLOCATION_H
#define GENOMICLOCATION_H

#include <string>
#include <fstream>
#include <sstream>
#include "ReferenceGenome.h"
#include "StringFunctions.h"

namespace cge{
   namespace patients{

class GenomicLocation
{
private:
   int chrom_num;
   uint64_t pos_num;
   ReferenceGenome* ref_genome;
   std::string rs_num;
public:
   GenomicLocation(int chrom, uint64_t pos, ReferenceGenome ref) : 
      chrom_num(chrom), pos_num(pos) 
   { 
      ref_genome = &ref;
   }
   
   GenomicLocation(int chrom, uint64_t pos) : 
      chrom_num(chrom), pos_num(pos) 
   { 
      ref_genome = &(GENOME_HG19::Instance());
   }
   
   GenomicLocation() : chrom_num(-1), pos_num(0)
   {
      ref_genome = &(GENOME_HG19::Instance());
   }

   void setRSNumber(const std::string rs) {rs_num=rs;}

   const int chromosome() const {return chrom_num;}

   const int position() const {return pos_num;}

   const std::string rsNumber() const {return rs_num;}
   
   const ReferenceGenome* refGenome() const {return ref_genome;}

   void setRefGenome(ReferenceGenome ref) {ref_genome = &ref;}

   std::string lookupRS() const
   {
      std::ifstream snps("snp138.txt");
      if(!snps)
         return "rs";
      std::string chrom = "chr" + chromosome();
      std::string pos = "" + position();
      bool foundChromNum = false;
      std::string line;
      std::vector<std::string> info;
      //skips over header line
      std::getline(snps, line);
      //begins searching
      while(snps.good()){
         std::getline(snps, line);
         info = utility::split(line, '\t');
         if (info[0].compare(chrom) == 0){
            foundChromNum = true;
            if (info[1].compare(pos) == 0)
               return info[2];
         }
         //stops searching once past chromosome number
         else if (foundChromNum)
            return "rs";
      }
      return "rs";
   }
};


inline bool operator==(const GenomicLocation& lhs, const GenomicLocation& rhs)
{
   if (lhs.refGenome() != nullptr && rhs.refGenome() != nullptr){
      if(((lhs.refGenome()->name()).compare(rhs.refGenome()->name())) == 0){
         if (lhs.chromosome() == rhs.chromosome() && 
               lhs.position() == rhs.position())
            return true;
      }
   }
   
   //look up rsNumber of lhs and compare
   if (lhs.rsNumber().size() < 3 && rhs.rsNumber().size() > 2){
      if(lhs.lookupRS().compare(rhs.rsNumber()) == 0)
         return true;
   }
   //look up rsNumber of rhs and compare
   else if (rhs.rsNumber().size() < 3 && lhs.rsNumber().size() > 2){
      if(rhs.lookupRS().compare(lhs.rsNumber()) == 0)
         return true;
   }
   return false;
}

}//namespace patients
}//namespace cge
#endif
