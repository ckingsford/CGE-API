/* This file exists for the sake of writing temporary tests
 * and checking if code compiles.
 */

#include <iostream>
#include "PatientSet.h"
#include "VCFReader.h"
#include "ARFFReader.h"
#include "ARFFWriter.h"
#include "ModelTrainer.h"
int main()
{
   using namespace std;
   using namespace cge::patients;
   using namespace cge::dataaquisition;
   /*filebuf fb;
   fb.open("chr2.vcf", ios::in);
   istream testVCF(&fb);
   shared_ptr<GenotypeSchema> foo = readVCFtoGenotype(testVCF);
   shared_ptr<Genotype> geno(new Genotype());
   geno->setSchema(foo);
   for (size_t i = 0; i < foo->size(); ++i)
      geno->setVariant(i, 0);
   Patient* John = new Patient();
   John->setGenotype(geno);
   John->setName("George");
   fb.close();
   fb.open("test.ARFF", ios::out);
   ostream testARFFout(&fb);
   writeARFFfromGenomic(testARFFout, *John);
   */
   cout<<"code compiled\n";
   return 0;
}
