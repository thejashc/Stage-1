#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "defs.h"

#include "read_control.H"

using namespace std;

int read_control ( CtrlData&  ctrl )
{

  cout << "--- Reading control file ---" << endl << endl;

  ifstream File ( in_ctrl.c_str() );
  if ( ! File ) { cout << "*** input file is empty" << endl; abort(); }

  File >> ctrl.NrSteps;      File.ignore(100,'\n');
  File >> ctrl.dcdintrvl;    File.ignore(100,'\n');
  File >> ctrl.logintrvl;    File.ignore(100,'\n');
  File >> ctrl.dt;           File.ignore(100,'\n');
  File >> ctrl.wall.mean;    File.ignore(100,'\n');
  File >> ctrl.wall.ampl;    File.ignore(100,'\n');
  File >> ctrl.wall.freq;    File.ignore(100,'\n');
  File >> ctrl.grav;         File.ignore(100,'\n');
  File >> ctrl.BckgrndFric;  File.ignore(100,'\n');

  cout << "NrSteps     : " << ctrl.NrSteps     << endl;
  cout << "dcdintrvl   : " << ctrl.dcdintrvl   << endl;
  cout << "logintrvl   : " << ctrl.logintrvl   << endl;
  cout << "dt          : " << ctrl.dt          << endl;
  cout << "wall mean   : "  << ctrl.wall.mean   << endl;
  cout << "     ampl   : "  << ctrl.wall.ampl   << endl;
  cout << "     freq   : "  << ctrl.wall.freq   << endl;
  cout << "gravity     : "  << ctrl.grav        << endl;
  cout << "BckgrndFric : "  << ctrl.BckgrndFric << endl;

  cout << endl;

#include "read_check.h"

  File.close();

// manipulations and checks

  ctrl.wall.on = ( ctrl.wall.mean > 0. );

  if ( ctrl.wall.on )
  {
    if ( ctrl.grav < 0. )
      { cout << "*** negative gravity ***" << endl; abort(); }
    if ( ctrl.wall.ampl < 0. )
      { cout << "*** negative wall amplitude ***" << endl; abort(); }
  }
  else // not wall.on
  {
    if ( ctrl.grav != 0. )
      { cout << "*** gravity but no limiting wall ***" << endl; abort(); }
    if ( ctrl.wall.ampl != 0 || ctrl.wall.freq != 0 )
      { cout << "+++ non-zero input on non-existing wall +++" << endl;}
  }

  if ( ctrl.dt > 0. ) 
    { ctrl.rdt = 1. / ctrl.dt; }
  else
    {
      cout << "+++  reciproce time step set to zero +++" << endl;
      ctrl.rdt = 0.;
    }

  return 0;
}
