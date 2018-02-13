#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "defs.h"

#include "read_field.H"

using namespace std;

int read_field( FieldData& field )
{

  int    i, j;
  double srij2, srij6;
  vctr3D com;
  double MaxDist, MaxDist2, MaxDistZ;
  double MinInert,MaxInert;

  cout << "--- Reading field file ---" << endl << endl;

  ifstream File ( in_fld.c_str() );
  if ( ! File ) { cout << "*** input file is empty ***" << endl; abort(); }

  File   >> field.NrParticles;         File.ignore(100,'\n');
  File   >> field.NrSubs;              File.ignore(100,'\n');

  if ( field.NrParticles < 1 ) { cout << "*** Too few particles ***" << endl; abort(); }
  if ( field.NrSubs      < 1 ) { cout << "*** Too few subs ***"      << endl; abort(); }

  field.sub.resize(field.NrSubs);
  for ( i = 0 ; i < field.NrSubs ; i ++ )
  {
    File >> field.sub[i].pos.comp[x]
         >> field.sub[i].pos.comp[y]
         >> field.sub[i].pos.comp[z]
         >> field.sub[i].mass;         File.ignore(100,'\n');
  } // i
  File   >> field.eps;                 File.ignore(100,'\n');
  File   >> field.sigma;               File.ignore(100,'\n');
  File   >> field.rcut;                File.ignore(100,'\n');
    
#include "read_check.h"

  File.close();

// finished reading, echo input - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  cout   << "NrParticles : "  << field.NrParticles   << endl;
  cout   << "NrSubs      : "  << field.NrSubs        << endl;
  cout   << setiosflags(ios::fixed) << setprecision(4);
  for ( i = 0 ; i < field.NrSubs ; i ++ )
  {
    cout << "  sub       : "  << setw(10) << field.sub[i].pos.comp[x] << "  "
                              << setw(10) << field.sub[i].pos.comp[y] << "  "
                              << setw(10) << field.sub[i].pos.comp[z] << "    &  "
                              << setw(10) << field.sub[i].mass        << endl;
  } // i
  cout   << "epsilon     : "  << setw(10) << field.eps                << endl;
  cout   << "sigma       : "  << setw(10) << field.sigma              << endl;
  cout   << "rcut        : "  << setw(10) << field.rcut  << " sigma"  << endl;
  cout   << endl;

// useful constants - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  field.sigma2  = field.sigma * field.sigma;
  field.eps4    = field.eps * 4.;
  field.rcut   *= field.sigma;
  field.rcut2   = field.rcut * field.rcut;

  srij2         = field.sigma2 / field.rcut2;
  srij6         = srij2 * srij2 * srij2;
  field.Ecut    = field.eps4 * srij6 * ( srij6 - 1. );

// zero center of mass - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  com        = null3D;
  field.mass = 0.;
  for ( i = 0 ; i < field.NrSubs ; i ++ )
  {
    com        += field.sub[i].pos * field.sub[i].mass;
    field.mass += field.sub[i].mass;
  }
  field.minv = 1. / field.mass;
  com *= field.minv;
  cout << "c.o.m.      : ";
  com.echo();
  cout << "zero-ed" << endl;
  for ( i = 0 ; i < field.NrSubs ; i ++ )
  {
    field.sub[i].pos -= com;
    cout << "  sub       : "  << setw(10) << field.sub[i].pos.comp[x] << "  "
                              << setw(10) << field.sub[i].pos.comp[y] << "  "
                              << setw(10) << field.sub[i].pos.comp[z] << "    &  "
                              << setw(10) << field.sub[i].mass        << endl;
  } // i
  cout << endl;

// establish safe cut-off distance - - - - - - - - - - - - - - - - - - - - - - - - -

  MaxDist2 = 0.0;
  MaxDistZ = 0.0;
  for ( i = 0 ; i < field.NrSubs ; i++ )
  {
    MaxDist2 = max ( MaxDist2,      field.sub[i].pos * field.sub[i].pos );
    MaxDistZ = max ( MaxDistZ, abs( field.sub[i].pos.comp[z] ) );
  }
  MaxDist      = sqrt(MaxDist2);            // furthest sub
// #ifdef ComList
  field.mxcut  = 2. * MaxDist + field.rcut; // furthest c.o.m. to c.o.m. distance plus cut-off
// #else
//   field.mxcut  = field.rcut;
// #endif
  field.mxcut2 = pow( field.mxcut, 2 );
  cout << "MaxDist     : "  << setw(10) << MaxDist     << endl
       << "mxcut       : "  << setw(10) << field.mxcut << endl << endl;

// Is the particle abs flat in z plane? - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  field.flat = false;
  if ( MaxDistZ == 0. )
  {
    cout << "+++ Particle is flat in z-direction +++" << endl << endl;
    field.flat = true;
  }

// inertia tensor - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  if ( field.NrSubs == 1 )
  {
     cout << "+++ NrSub = 1, requires faked inertia tensor +++" << endl;
     field.inert  = mtrx3D( 1., 1., 1. ); // prevents execution problems
     field.inert *= ( 2. / 5. ) * field.sub[0].mass * pow( field.sigma, 2 );
     // rough approach to 2/5 m r^2, i.e. massive sphere
  }
  else
  {
    field.inert = null33D; // ignoring mass distribution within subs
    for ( j = 0 ; j < field.NrSubs ; j ++ )
      { field.inert -= mtrx3D( field.sub[j].pos, field.sub[j].pos ) * field.sub[j].mass ; }
    field.inert -= mtrx3D( 1., 1., 1. ) * field.inert.trace(); // minus to compensate negative trace
  }

  cout << "inertia : " << endl;
  field.inert.echo();
  cout << endl;

// checks on inertia tensor to prevent problems in dynamics - - - - - - - - - - - - - - - - - - - -

  MaxInert = 0.;  // also used as estimate of typical element value
  for ( i = 0 ; i < 3 ; i ++ )
  {
    for ( j = 0 ; j < 3 ; j ++ )
      { MaxInert = max( MaxInert, abs( field.inert.comp[i][j] ) ) ; }
  }

  if ( abs(field.inert.det()) < 1.e-6 * pow(MaxInert,3) )
  {
    cout << "+++ Inertia tensor has vanishing determinant +++" << endl;
    for ( i = 0 ; i < 3 ; i ++ )
    {
      if ( abs( field.inert.comp[i][i] ) < 1.e-6 )
      {
        cout << "+++ inserted fake diagonal element: " << i << " +++" << endl;
        field.inert.comp[i][i] = MaxInert;
      }
    } // i
    cout << "inertia     :" << endl;
    field.inert.echo();
    cout << endl;
    if ( abs(field.inert.det()) < 1.e-6 * pow(MaxInert,3) )
      { cout << "*** Inertia tensor still has vanishing determinant ***" << endl; abort(); };
  } // det

  MinInert = min( min( field.inert.comp[x][x],     // diagonal
                       field.inert.comp[y][y] ),
                       field.inert.comp[z][z] );

  if ( MinInert < 0 )
      { cout << "*** Inertia tensor still has negative diagonal element ***" << endl; abort(); };
  if ( max( max( abs( field.inert.comp[x][y] ),   // off-diagonal
                 abs( field.inert.comp[x][z] ) ),
                 abs( field.inert.comp[y][z] ) )
       > 1.e-6 * MinInert
     )
    { cout << "+++ Inertia tensor is not diagonal +++" << endl; } // abort(); } // Is this neccessary?

// inverse inertia tensor - - - - - - - - - - - - - - - - - - - - - - - - -

  field.InrtInv = field.inert.inverse();

  cout << "inertia inv :" << endl;
  field.InrtInv.echo();
  cout << endl;

  cout << endl;

  return 0;
}
