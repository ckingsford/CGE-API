#ifndef REFERENCEGENOME_H
#define REFERENCEGENOME_H

#include<string>

namespace cge{
   namespace patients{
   
class ReferenceGenome
{
protected:
   ReferenceGenome(){}
   ~ReferenceGenome(){}
   ReferenceGenome(const ReferenceGenome&);        //Prevent copy-construction
   ReferenceGenome& operator=(const ReferenceGenome&);    //Prevent assignment 
   std::string ref_name;
public:
   static ReferenceGenome& Instance()
   {
      static ReferenceGenome singleton;
      return singleton;
   }

   const std::string& name() const 
   {
      return ref_name;
   }
};
inline bool operator==(const ReferenceGenome& lhs, const ReferenceGenome& rhs)
{
   return (((lhs.name()).compare(rhs.name())) == 0);
}

class GENOME_HG19 : public  ReferenceGenome
{
private:
   GENOME_HG19(){}
   ~GENOME_HG19(){}
   GENOME_HG19(const GENOME_HG19&);
   GENOME_HG19& operator=(const GENOME_HG19&);
public:
   static GENOME_HG19& Instance()
   {
      static GENOME_HG19 singleton;
      return singleton;  
   }
};

}//namespace patients
}//namespace cge
#endif
