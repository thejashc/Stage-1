#include <cstdlib>

using namespace std;

#include "rnd.H"

double rnd(void)
{
  return (double) rand() / RAND_MAX;
}

