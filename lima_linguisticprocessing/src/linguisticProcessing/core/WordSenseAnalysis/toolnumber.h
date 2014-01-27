/*
    Copyright 2002-2013 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef LIMA_WORDSENSEDISAMBIGUATION_TOOLNUMBER_H
#define LIMA_WORDSENSEDISAMBIGUATION_TOOLNUMBER_H

namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{
  
        
/** 
        @author Guillaume Pitel <guillaume.pitel@gmail.com>
        @author Claire Mouton <Claire.Mouton@cea.fr>
*/  

class ToolNumber
{   
public:
  ToolNumber();
  
  ~ToolNumber();

  
  static std::vector<int> & makePermutation(int size)
  {
    std::vector<int> * permut = new std::vector<int>(size);
    std::vector<int> avail(size);
    // Generate a list {0,1,2,3,...,size-1}
    for (int i = 0; i < size; i++)
      avail[i] = i;
    int index;
    for (int i = 0; i < size; i++)
    {   
      // Pick a random element from the list
      index = rand() % (size-i);
      (*permut)[i] =  avail[index];
      // And replace it with the last element in the list (so that it is no longer available)
      avail[index] = avail.back();
      // Remove the last element
      avail.pop_back();
    }
    return *permut;
  } 

  
  
  static float magiccosine ( char *a, float *b, int dim , int nPairs)
  {
    double sum = 0.;
    for (int p = 0; p < nPairs ; p++) {
      uint64_t pos;
      memcpy (&pos, a + p*(sizeof(uint)+sizeof(float)), sizeof(uint));
      if (pos < (uint) dim) {
  float value;
  memcpy (&value, a + p*(sizeof(uint)+sizeof(float))+ sizeof(uint), sizeof(float));
  /*cerr << "Value : " << (double)value << endl;
  cerr << "b[pos] : " << (double)b[pos]<< endl;*/  
  sum+= (double)value * (double)b[pos];
      }
    }
    return (float)sum;
  }
  
  
  
};



}
}
}


#endif // LIMA_WORDSENSEDISAMBIGUATION_TOOLNUMBER_H
