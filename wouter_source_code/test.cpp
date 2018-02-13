#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "vctr3D.h"

double rnd(void)
 {
   return (double) rand() / RAND_MAX;
 }


int main (void)
{
  int    i,j;
  int    iseed;
  mtrx3D a,b,c;

  iseed = 135;
  srand(iseed);

  for ( i = 0 ; i < 3 ; i ++ )
  {
    for ( j = 0 ; j < 3 ; j ++ )
    {
      a.comp[i][j] = rnd();
    }
  }

  a.echo();
  cout << endl;

  b = a.inverse();

  a.echo();
  cout << endl;
  b.echo();
  cout << endl;

  c = a * b;

  a.echo();
  cout << endl;
  b.echo();
  cout << endl;
  c.echo();
  cout << endl;

  c = b * a;

  a.echo();
  cout << endl;
  b.echo();
  cout << endl;
  c.echo();
  cout << endl;
}
