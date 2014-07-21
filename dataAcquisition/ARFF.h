#ifndef ARFF_H
#define ARFF_H

#include <iostream>
#include "PatientSet.h"

namespace ARFF
{
   bool readARFFtoClinical(std::istream& in_stream, PatientSet& P);
   bool writeARFFfromClinical(std::ostream& out_stream, PatientSet& P);
   bool readARFFtoGenotype(std::istream& in_stream, PatientSet& P);
   bool writeARFFfromGenotype(std::ostream& out_stream, PatientSet& P);

}

#endif
