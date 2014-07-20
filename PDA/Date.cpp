namespace date
{
   bool isLeapYear(int y)
   {
      return(y % 100 != 0 && y % 4 == 0) || (y % 400 == 0);
   }
   bool isValidDate(int day, int month, int year)
   {
      bool is_valid = true;
      if((year <= 1600) | (day < 1))
         is_valid = false;    
      switch(month)
      {
         case 2:
            if(isLeapYear(year)){ 
               if(day > 29)
                  is_valid = false;
            }
            else{
               if(day > 28)
                  is_valid = false;
            }
            break;
         case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            if(day > 31)
               is_valid = false;
            break;
         case 4: case 6: case 9: case 11:
            if(day > 30)
               is_valid = false;
            break;
         default: 
            is_valid = false;
            break;
      }
      return is_valid;
      }
}
