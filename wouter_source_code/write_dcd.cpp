#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "defs.h"

#include "write_dcd.H"

using namespace std;

int write_dcd ( const vector<ParticleData>&  particle,
                const BoxData&               box,
                const int                    step,
                const int                    NrParticles,
                const int                    NrSubs
              )
{

//string const name      = "output.dcd";
  bool   const WrtBox  = true;
  bool   const WrtWall = true;

  char     c1; // ,i1;
  int      i4;
  float    r4;
  double   r8;

  int      i,j,k;
  int      NrOut;
  float    corners[8][3];
  float    wall[4][3];

  NrOut = NrParticles * NrSubs;
  if ( WrtBox )                 { NrOut += 8; }
  if ( WrtWall && box.wall.on ) { NrOut += 4; }

// on first write: create proper header

  if ( step == 0 )
  {
// check sized of variable, to guarantee compatibility
    if ( sizeof(c1) != 1 ) { cout << "*** sizeof(c1) = " << sizeof(c1) << endl; abort(); }
    if ( sizeof(r4) != 4 ) { cout << "*** sizeof(r4) = " << sizeof(r4) << endl; abort(); }
    if ( sizeof(r8) != 8 ) { cout << "*** sizeof(r8) = " << sizeof(r8) << endl; abort(); }
    if ( sizeof(i4) != 4 ) { cout << "*** sizeof(i4) = " << sizeof(i4) << endl; abort(); }

    ofstream File ( out_dcd.c_str(), ios::binary );

    i4 =   84 ; File.write( (const char*) &i4, sizeof(i4) );  // block header
    c1 =   'C'; File.write( (const char*) &c1, sizeof(c1) );
    c1 =   'O'; File.write( (const char*) &c1, sizeof(c1) );
    c1 =   'R'; File.write( (const char*) &c1, sizeof(c1) );
    c1 =   'D'; File.write( (const char*) &c1, sizeof(c1) );
    i4 = 9999 ; File.write( (const char*) &i4, sizeof(i4) );  // number of frames -- don't care
    i4 =    0 ; File.write( (const char*) &i4, sizeof(i4) );
    i4 =    1 ; File.write( (const char*) &i4, sizeof(i4) );
    for ( j = 0 ; j < 6 ; j++ )
    {
      i4 =  0 ; File.write( (const char*) &i4, sizeof(i4) );
    }
    r8 = 1.e-15; File.write( (const char*) &r8, sizeof(r8) );
    for ( j = 0 ; j < 9 ; j++ )
    {
      i4 =  0 ; File.write( (const char*) &i4, sizeof(i4) );
    }
    i4 =   84 ; File.write( (const char*) &i4, sizeof(i4) ); // block tailer

    // comment lines

    i4 =    4 ; File.write( (const char*) &i4, sizeof(i4) ); // block header
    i4 =    0 ; File.write( (const char*) &i4, sizeof(i4) ); // zero
    i4 =    4 ; File.write( (const char*) &i4, sizeof(i4) ); // block tailer

    // number of particles

    i4 =    4 ; File.write( (const char*) &i4, sizeof(i4) ); // block header
    i4 = NrOut; File.write( (const char*) &i4, sizeof(i4) );
    i4 =    4 ; File.write( (const char*) &i4, sizeof(i4) ); // block tailer

    File.close();

  } // step == 0

// prepare surrounding box

  if ( WrtBox )
  {
    corners[0][x] =         0.;
    corners[0][y] =         0.;
    corners[1][x] = (float) box.edge.comp[x];
    corners[1][y] =         0.;
    corners[2][x] = (float) box.edge.comp[x];
    corners[2][y] = (float) box.edge.comp[y];
    corners[3][x] =         0.;
    corners[3][y] = (float) box.edge.comp[y];
    for ( k = 0 ; k < 4 ; k ++ )
    {
      corners[k  ][z] =         0.;
      corners[k+4][x] = (float) corners[k][x];
      corners[k+4][y] = (float) corners[k][y];
      corners[k+4][z] = (float) box.edge.comp[z];
    }
  } // corners

  if ( WrtWall && box.wall.on )
  {
    switch ( box.wall.dirctn )
    {
      case y :
        wall[0][x] =         0.;
        wall[0][z] =         0.;
        wall[1][x] = (float) box.edge.comp[x];
        wall[1][z] =         0.;
        wall[2][x] = (float) box.edge.comp[x];
        wall[2][z] = (float) box.edge.comp[z];
        wall[3][x] =         0.;
        wall[3][z] = (float) box.edge.comp[z];
        wall[0][y] = (float) box.wall.pos;
        wall[1][y] = (float) box.wall.pos;
        wall[2][y] = (float) box.wall.pos;
        wall[3][y] = (float) box.wall.pos;
        break; // y

      case z :
        wall[0][x] =         0.;
        wall[0][y] =         0.;
        wall[1][x] = (float) box.edge.comp[x];
        wall[1][y] =         0.;
        wall[2][x] = (float) box.edge.comp[x];
        wall[2][y] = (float) box.edge.comp[y];
        wall[3][x] =         0.;
        wall[3][y] = (float) box.edge.comp[y];
        wall[0][z] = (float) box.wall.pos;
        wall[1][z] = (float) box.wall.pos;
        wall[2][z] = (float) box.wall.pos;
        wall[3][z] = (float) box.wall.pos;
        break; // z

      default :
        cout << " unknown box.wall.dirctn" << endl;
        abort ();
    } // switch
  } // WrtWall

// write configuration ----------------------------------------

  ofstream File ( out_dcd.c_str(), ios::binary | ios::app );

  i4 = 4 * NrOut;

  for ( k = 0 ; k < 3 ; k ++ )
  {
    File.write( (const char*) &i4, sizeof(i4) ); // block header

    for ( i = 0 ; i < NrParticles ; i ++ )
    {
      for ( j = 0 ; j < NrSubs ; j ++ )
      {
        r4 = (float) particle[i].sub[j].pos.comp[k]; File.write( (const char*) &r4, sizeof(r4) );
      } // j
    } // i

    if ( WrtBox )
    {
      for ( j = 0 ; j < 8 ; j++ )
      {
        r4 = corners[j][k]; File.write( (const char*) &r4, sizeof(r4) );
      } // j
    } // corners
    if ( WrtWall && box.wall.on )
    {
      for ( j = 0 ; j < 4 ; j++ )
      {
        r4 = wall[j][k]; File.write( (const char*) &r4, sizeof(r4) );
      }
    } // wall

    File.write( (const char*) &i4, sizeof(i4) ); // block tailer
  } // k
  
  File.close();

  return 0;
}
