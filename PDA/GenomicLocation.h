#ifndef GENOMICLOCATION_H
#define GENOMICLOCATION_H

#include <string>
#include "ReferenceGenome.h"

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
   
   GenomicLocation() : chrom_num(0), pos_num(0)
   {
      ref_genome = &(GENOME_HG19::Instance());
   }

   void setRSNumber(const std::string rs) {rs_num=rs;}

   const int chromosome() const {return chrom_num;}

   const int position() const {return pos_num;}

   const std::string rsNumber() const {return rs_num;}
};
inline bool operator==(const GenomicLocation& lhs, const GenomicLocation& rhs)
{
   if (lhs.chromosome() == rhs.chromosome() && lhs.position() == rhs.position())
      return true;
   if ((lhs.rsNumber()).compare(rhs.rsNumber()) == 0)
      return true;
   if (/*TODO*/ false)
      return true;
   return false;
}

}//namespace patients
}//namespace cge
#endif
