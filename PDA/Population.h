#ifndef POPULATION_H
#define POPULATION_H

#include<string>

namespace cge{
   namespace patients{
   
class Population
{
private:
   std::string pop_name;
public:
   Population(std::string n) : pop_name(n) { }

   const std::string& name() const {return pop_name;}

   void setName(const std::string& s) {pop_name = s;}
};

}//namespace patients
}//namespace cge
#endif
