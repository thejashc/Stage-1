#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "defs.h"

#include "write_config.H"

using namespace std;

int write_config( const vector<ParticleData>&  particle,
                  const BoxData&               box,
                  const int                    step,
                  const int                    NrParticles,
                  const int                    NrSubs,
                  const string                 name,
                  const bool                   echo
                )
{
  int i,j,k;
  int i4;

  if ( sizeof(i4) != 4 ) { cout << "*** sizeof(i4) = " << sizeof(i4) << endl; abort(); }

  cout << "--- Writing config ---" << endl << endl;

  ofstream File ( name.c_str(), ios::binary);

  i4 = sizeof(NrParticles) + sizeof(step) + sizeof(box.file) + sizeof(box.type) + sizeof(box.dimens);
  File.write( (const char*) &i4         , sizeof(i4         ) ); // header for fortran compatibility
  File.write( (const char*) &NrParticles, sizeof(NrParticles) );
  File.write( (const char*) &step       , sizeof(step       ) );
  File.write( (const char*) &box.file   , sizeof(box.file   ) );
  File.write( (const char*) &box.type   , sizeof(box.type   ) );
  File.write( (const char*) &box.dimens , sizeof(box.type   ) );
  File.write( (const char*) &i4         , sizeof(i4         ) ); // footer for fortran

  i4 = sizeof(box.edge) + sizeof(box.wall);
  File.write( (const char*) &i4         , sizeof(i4         ) ); // header
//cout << box.edge[x] << "  " << box.edge[y] << "  " << box.edge[z] << endl;
  File.write( (const char*) &box.edge   , sizeof(box.edge   ) );
  File.write( (const char*) &box.wall   , sizeof(box.wall   ) );
  File.write( (const char*) &i4         , sizeof(i4         ) ); // footer

// positions

  k  = sizeof(particle[0].pos.comp[x]);
  i4 = NrParticles * box.dimens * k;
  File.write( (const char*) &i4, sizeof(i4) ); // header
  for ( i = 0 ; i < NrParticles ; i++ )
  {
    for ( j = 0 ; j < box.dimens ; j ++ )
    {
      File.write( (const char*) &particle[i].pos.comp[j], k );
    } // j
  } // i
  File.write( (const char*) &i4, sizeof(i4) ); // footer

// velocities

  k  = sizeof(particle[0].vel.comp[x]);
  i4 = NrParticles * box.dimens * k;
  File.write( (const char*) &i4, sizeof(i4) ); // header
  for ( i = 0 ; i < NrParticles ; i++ )
  {
    for ( j = 0 ; j < box.dimens ; j ++ )
    {
      File.write( (const char*) &particle[i].vel.comp[j], k );
    } // j
  } // i
  File.write( (const char*) &i4, sizeof(i4) ); // footer

  if ( NrSubs > 1 )
  {

// rotations

    k  = sizeof(particle[0].quat.comp[0]);
    i4 = NrParticles * ( box.angle_stop - box.angle_start + 1 ) * k;
    File.write( (const char*) &i4, sizeof(i4) ); // header
    for ( i = 0 ; i < NrParticles ; i++ )
    {
      for ( j = box.angle_start ; j <= box.angle_stop ; j ++ )
      {
        File.write( (const char*) &particle[i].quat.comp[j], k );
      } // j
    } // i
    File.write( (const char*) &i4, sizeof(i4) ); // footer

// angular velocities

    k  = sizeof(particle[0].omega.comp[0]);
    i4 = NrParticles * k * ( box.omega_stop - box.omega_start + 1 );
    File.write( (const char*) &i4, sizeof(i4) ); // header
    for ( i = 0 ; i < NrParticles ; i++ )
    {
      for ( j = box.omega_start ; j <= box.omega_stop ; j ++ )
      {
        File.write( (const char*) &particle[i].omega.comp[j], k );
      } // j
    } // i
    File.write( (const char*) &i4, sizeof(i4) ); // footer

  } // NrSubs > 1

// done

  File.close();

  if ( echo )
  {
    ParticleData TmpPart(0);
    for ( i = 0 ; i < min(NrParticles,10) ; i ++ )
    {
      cout << i;
      TmpPart = particle[i];
      TmpPart.echo(box);
    }
  }
/*{
    cout << i << "  "
         << particle[i].pos[x] << "  "
         << particle[i].pos[y] << "  "
         << particle[i].pos[z] << "  "
         << particle[i].vel[x] << "  "
         << particle[i].vel[y] << "  "
         << particle[i].vel[z] << endl;
  } */
  cout << endl;

  return 0;
}
