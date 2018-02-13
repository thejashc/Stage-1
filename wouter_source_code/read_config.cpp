#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "defs.h"

#include "read_config.H"

using namespace std;

int read_config (       vector<ParticleData>&  particle,
                        BoxData&               box,
                        int&                   steps,
                  const int                    NrParticles,
                  const int                    NrSubs
                )
{

  int i, j, k;
  int i4,j4;

// read

  cout << "--- Reading start configuration ---" << endl << endl;

  ifstream File ( in_conf.c_str(), ios::binary );
  if ( ! File ) { cout << "*** input file is empty" << endl; abort(); }

  i4 = sizeof(i) + sizeof(steps) + sizeof(box.file) + sizeof(box.type) + sizeof(box.dimens);
  File.read( (char*) &j4        , sizeof(j4        ) ); // header for fortran compatibility
  if ( j4 != i4 ) { cout << "*** Mismatch in size of header 1 : " << i4 << " vs " << j4 << endl; abort(); }
  File.read( (char*) &i         , sizeof(i         ) ); // NrParticles
  File.read( (char*) &steps     , sizeof(steps     ) );
  File.read( (char*) &box.file  , sizeof(box.file  ) );
  File.read( (char*) &box.type  , sizeof(box.type  ) );
  File.read( (char*) &box.dimens, sizeof(box.dimens) );
  File.read( (char*) &j4        , sizeof(j4        ) ); // footer for fortran compatibility
  if ( j4 != i4 ) { cout << "*** Mismatch in size of footer 1 : " << i4 << " vs " << j4 << endl; abort(); }

  cout << "Number of particles : " << i          << endl;
  cout << "Previous steps      : " << steps      << endl;
  cout << "File type           : " << box.file   << endl;
  cout << "Box type            : " << box.type   << endl;
  cout << "Box dimensions      : " << box.dimens << endl;

  box.SetAngRange();

  i4 = sizeof(box.edge) + sizeof(box.wall);
  File.read( (char*) &j4       , sizeof(j4       ) ); // header
  if ( j4 != i4 ) { cout << "*** Mismatch in size of header 2 : " << i4 << " vs " << j4 << endl; abort(); }
  File.read( (char*) &box.edge , sizeof(box.edge ) );
  File.read( (char*) &box.wall , sizeof(box.wall ) );
  File.read( (char*) &j4       , sizeof(j4       ) ); // footer for fortran compatibility
  if ( j4 != i4 ) { cout << "*** Mismatch in size of footer 1 : " << i4 << " vs " << j4 << endl; abort(); }

  cout << "Box edge            : " << box.edge.comp[x] << "  "
                                   << box.edge.comp[y] << "  "
                                   << box.edge.comp[z] << "  "  << endl;
  cout << "Wall mean           : " << box.wall.mean    << endl;
  cout << "     amplitude      : " << box.wall.ampl    << endl;
  cout << "     freq           : " << box.wall.freq    << endl;
  cout << "     phase          : " << box.wall.phase   << endl;
  cout << "     pos            : " << box.wall.pos     << endl;
  cout << endl;

  if ( i != NrParticles )
  {
    cout << "*** Inconsistent numbers of particles ***" << endl << endl;
    cout << i << " versus " << NrParticles << endl;
    abort();
  }

// positions

  k  = sizeof(particle[0].pos.comp[x]);
  i4 = NrParticles * box.dimens * k;
  File.read( (char*) &j4, sizeof(j4) ); // header
  if ( j4 != i4 ) { cout << "*** Mismatch in size of header pos : " << i4 << " vs " << j4 << endl; abort(); }
  for ( i = 0 ; i < NrParticles ; i++ )
  {
    for ( j = 0 ; j < box.dimens ; j ++ )
    {
      File.read( (char*) &particle[i].pos.comp[j], k );
    } // j
  } // i
  File.read( (char*) &j4, sizeof(j4) ); // footer
  if ( j4 != i4 ) { cout << "*** Mismatch in size of footer pos : " << i4 << " vs " << j4 << endl; abort(); }

// velocities

  k  = sizeof(particle[0].vel.comp[x]);
  i4 = NrParticles * box.dimens * k;
  File.read( (char*) &j4, sizeof(j4) ); // footer
  if ( j4 != i4 ) { cout << "*** Mismatch in size of header vel : " << i4 << " vs " << j4 << endl; abort(); }
  for ( i = 0 ; i < NrParticles ; i++ )
  {
    for ( j = 0 ; j < box.dimens ; j++ )
    {
      File.read( (char*) &particle[i].vel.comp[j], k );
    } // j
  } // i
  File.read( (char*) &j4, sizeof(j4) ); // footer
  if ( j4 != i4 ) { cout << "*** Mismatch in size of footer vel" << endl; abort(); }

// rotations

  if ( NrSubs > 1 )
  {
    k  = sizeof(particle[0].quat.comp[0]);
    i4 = NrParticles * ( box.angle_stop - box.angle_start + 1 ) * k;
    File.read( (char*) &j4, sizeof(j4) ); // footer
    if ( j4 != i4 ) { cout << "*** Mismatch in size of header quat : " << i4 << " vs " << j4 << endl; abort(); }
    for ( i = 0 ; i < NrParticles ; i++ )
    {
      for ( j = box.angle_start ; j <= box.angle_stop ; j++ )
      {
        File.read( (char*) &particle[i].quat.comp[j], k );
      } // j
    } // i
    File.read( (char*) &j4, sizeof(j4) ); // footer
    if ( j4 != i4 ) { cout << "*** Mismatch in size of footer quat" << endl; abort(); }

// angular velocities

    k  = sizeof(particle[0].omega.comp[0]);
    i4 = NrParticles * ( box.omega_stop - box.omega_start + 1 ) * k;
    File.read( (char*) &j4, sizeof(j4) ); // footer
    if ( j4 != i4 ) { cout << "*** Mismatch in size of header qdot : " << i4 << " vs " << j4 << endl; abort(); }
    for ( i = 0 ; i < NrParticles ; i++ )
    {
      for ( j = box.omega_start ; j <= box.omega_stop ; j++ )
      {
        File.read( (char*) &particle[i].omega.comp[j], k );
      } // j
    } // i
    File.read( (char*) &j4, sizeof(j4) ); // footer
    if ( j4 != i4 ) { cout << "*** Mismatch in size of footer qdot" << endl; abort(); }
  } // NrSubs > 1

// done reading

  File.close();

  for ( i = 0 ; i < min(NrParticles,10) ; i ++ )
  {
    cout << i;
    particle[i].echo(box);
  }
  cout << endl;

  return 0;
}
