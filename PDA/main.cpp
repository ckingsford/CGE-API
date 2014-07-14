#include <iostream>
#include "ClinicalValue.h"
#include "ClinicalSchema.h"
#include "ClinicalRecord.h"
int main()
{
   using std::cout;
   using std::vector;
   using std::string;
   using std::shared_ptr;
   cout<<"code compiled\n";
/*
   shared_ptr<ClinicalSchema> test_schema(new ClinicalSchema());
   ClinicalField* test0 = new ClinicalField("A");
   ClinicalField* test1 = new ClinicalField("B");
   ClinicalField* test2 = new ClinicalField("C");
   ClinicalField* test3 = new ClinicalField("D");
   ClinicalField* test4 = new ClinicalField("E");
   ClinicalField* test5 = new ClinicalField("F");

   test_schema->appendField(test0);
   test_schema->appendField(test1);
   test_schema->appendField(test2);
   test_schema->appendField(test3);
   test_schema->appendField(test4);
   test_schema->appendField(test5);
   
   ClinicalRecord* test_record = new ClinicalRecord();
   test_record->setSchema(test_schema);

   vector<string> order = test_record->schema()->fieldNames();
   for(auto it = order.begin(); it!=order.end();++it)
      cout << *it << "\n";
   cout<<"\n";
   
   IntValue* zero = new IntValue(0);
   IntValue* one = new IntValue(1);
   IntValue* two = new IntValue(2);
   IntValue* three = new IntValue(3);
   IntValue* four = new IntValue(4);
   IntValue* five = new IntValue(5);

   test_record->setClinicalValue(0,zero);
   test_record->setClinicalValue("B",one);
   test_record->setClinicalValue(2,two);
   test_record->setClinicalValue("D",three);
   test_record->setClinicalValue(4,four);
   test_record->setClinicalValue("F",five);

   std::shared_ptr<ClinicalValue> ten_ptr(new IntValue(10));

   (*test_record)["A"] = ten_ptr;
   int x = int(*((IntValue*) ((*test_record)[0]).get()));
   if(x == 10)
      cout<<"true \n";
   else
      cout<<"false, x = "<<x<<"\n";

   
   IntValue* temp = (IntValue*) test_record->value("A");
   cout<<int(*temp)<<"\n";
   temp = (IntValue*) test_record->value(1);
   cout<<int(*temp)<<"\n";
   temp = (IntValue*) test_record->value("C");
   cout<<int(*temp)<<"\n";  
   temp = (IntValue*) test_record->value(3);
   cout<<int(*temp)<<"\n"; 
   temp = (IntValue*) test_record->value("E");
   cout<<int(*temp)<<"\n";
   temp = (IntValue*) test_record->value(5);
   cout<<int(*temp)<<"\n\n";
*/
   return 0;
}
