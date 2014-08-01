#include "StringFunctions.h"

namespace utility{

std::vector<std::string> split(const std::string &s, char delim) 
{
   std::vector<std::string> items;
   std::stringstream s_stream(s);
   std::string item;
   while (std::getline(s_stream, item, delim)) {
      items.push_back(item);
   }
   return items;
}

}

