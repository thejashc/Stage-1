#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "defs.h"

#include "headers.H"

using namespace std;

int main(void)
{

  int     i,j,k,m;
  int     NrParticles;
  int     iseed;
  vctr3D  rij;
  vctr4D  qdoti;
  double  wall, hght;
  bool    overlap;
  double  MinDist,MinDist2;
  double  dist2;
  double  Vwidth,Owidth;

  BoxData box;

// read control parameters

  ifstream File ( in_bld.c_str() );
  if ( ! File ) { cout << "*** input file is empty" << endl; abort(); }

//File >> NrParticles;         File.ignore(100,'\n');
  File >> box.dimens;          File.ignore(100,'\n');
  File >> box.edge.comp[x]
       >> box.edge.comp[y]
       >> box.edge.comp[z];    File.ignore(100,'\n');
  File >> wall;                File.ignore(100,'\n');
  File >> hght;                File.ignore(100,'\n');
  File >> MinDist;             File.ignore(100,'\n');
  File >> Vwidth;              File.ignore(100,'\n');
  File >> Owidth;              File.ignore(100,'\n');
  File >> iseed;               File.ignore(100,'\n');

  cout << "--- Generating random configuration ---" << endl << endl;
//cout << "    NrParticles : " << NrParticles      << endl;
  cout << "    Box dimens  : " << box.dimens       << endl;
  cout << "    Box edge    : " << box.edge.comp[x] << "  "
                               << box.edge.comp[y] << "  "
                               << box.edge.comp[z] << "  " << endl;
  cout << "    Wall        : " << wall             << endl;
  cout << "    height      : " << hght             << endl;
  cout << "    MinDist     : " << MinDist          << endl;
  cout << "    vel-width   : " << Vwidth           << endl;
  cout << "    omega-width : " << Owidth           << endl;
  cout << "    iseed       : " << iseed            << endl;
  cout << endl;

#include "read_check.h"

  File.close();  // break the connection

// read field

  FieldData   field;
  read_field( field );
  
// checks

/*
  if ( field.NrParticles != NrParticles )
  {
    cout << "*** Inconsistent numbers of particles ***" << endl;
    cout << field.NrParticles << " versus " << NrParticles;
    abort();
  }
*/
  NrParticles = field.NrParticles;

  if ( box.dimens == 2 && ! field.flat )
  { cout << "*** Flat box with non-flat particles" << endl; abort(); }

//free (File);   // free the memory

//ifstream File ( out_conf.c_str() );
  File.open( in_conf.c_str() );
  if ( File ) { cout << "*** output file does already exist" << endl; abort(); }
  File.close();

// initialisation - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  MinDist  = field.mxcut - field.rcut + MinDist; // closest permitted com-com distance
  MinDist2 = MinDist * MinDist;
  cout << "MinDist     : " << MinDist  << endl << endl;

  if ( box.dimens == 2 )
  {
    box.edge.comp[z] = 3.2 * field.mxcut;
    cout << "+++ Superfluous 3rd box edge reset to +++ " << box.edge.comp[z] << endl << endl;
  }

  box.wall.dirctn = box.dimens - 1;              // last direction, also used for wall and height-restrictions

// particles will be placed between wall and wall+hght in box.wall.dirctn direction

  box.wall.on  = ( wall >= 0. );
  box.wall.pos =   wall;
  if ( box.wall.on )
  {
    cout << "--- wall ---" << endl << endl;
    if ( wall > 0.25 * box.edge.comp[box.wall.dirctn] )
      { cout << "*** wall position rather high" << endl; abort(); }
    if ( wall < field.mxcut )
      { cout << "*** wall position rather low" << endl; abort(); }
    wall = wall + MinDist;
  }
  else
  {
    cout << "--- no wall ---" << endl << endl;
    wall = 0.;
  }

  box.SetAngRange();
  box.update();

  if ( hght <= 0. )                                      // negative -> default values:
  {
    if ( box.wall.on )
      { hght  = 0.25 * box.edge.comp[box.wall.dirctn]; }  // ... quarter box or ...
    else
      { hght  =        box.edge.comp[box.wall.dirctn]; }  // ... entire box
  }
  else
      { hght *=       box.edge.comp[box.wall.dirctn]; }  // positive -> fraction
  cout << "--- particles placed from " << wall << " to " << wall + hght << endl << endl;
  if ( wall + hght > box.edge.comp[box.wall.dirctn] )
    { cout << "*** please check height" << endl; abort(); }

  vector<ParticleData> particle( NrParticles, ParticleData(0) );

  Vwidth *= sqrt( 12. );
  Owidth *= sqrt( 12. );

  srand(iseed);

// generate box: positions and velocities ----------------------------

  for ( i = 0 ; i < NrParticles ; i ++ )
  {
    overlap = true;
    m = 0;
    while ( overlap )
    {
      m ++;
      particle[i].pos = null3D;
      for ( k  = 0 ; k < box.wall.dirctn ; k ++ )
      {
        particle[i].pos.comp[k] = rnd() * box.edge.comp[k];
      }
      // the one direction with optional wall and free surface
      particle[i].pos.comp[box.wall.dirctn] = rnd() * hght + wall;

      overlap = false;   // brute force check for overlap with previous particles
      j = 0;
      while ( ( ! overlap ) && ( j < i ) )
      {
        dist2 = 0.;
        rij = particle[i].pos - particle[j].pos;
        box.PBC(rij);
        dist2 = rij * rij;
        overlap = ( dist2 < MinDist2 );
        j = j + 1;
      } // !overlap
    } // overlap

    cout << i+1 << "  " << m << endl;

    for ( k = 0 ; k < box.dimens ; k ++ )
    {
      particle[i].vel.comp[k] = Vwidth * ( rnd() - 0.5 );
    }

  } // i

// angular part ------------------------------------------------------

  if ( field.NrSubs > 1 )
  {
    if ( box.dimens == 2 )          // 2D box
    {
      for ( i = 0 ; i < NrParticles ; i ++ )
      {
        particle[i].quat .comp[ang2D] =  twopi * ( rnd() - 0.5 );
        particle[i].omega.comp[ang2D] = Owidth * ( rnd() - 0.5 );
      } // i
    }
    else                            // 3D box
    {
      for ( i = 0 ; i < NrParticles ; i ++ )
      {
        dist2 = 1.;
        while ( dist2 > 0.25 )
        {
          particle[i].quat.comp[0] = rnd() - 0.5;
          particle[i].quat.comp[1] = rnd() - 0.5;
          particle[i].quat.comp[2] = rnd() - 0.5;
          particle[i].quat.comp[3] = rnd() - 0.5;
          dist2 = particle[i].quat * particle[i].quat;
        }
        particle[i].quat /= sqrt(dist2);
        particle[i].omega.comp[0] = Owidth * ( rnd() - 0.5 );
        particle[i].omega.comp[1] = Owidth * ( rnd() - 0.5 );
        particle[i].omega.comp[2] = Owidth * ( rnd() - 0.5 );
      } // i
    } // box.dimens
  } // field.NrSubs > 1

  box.file = 1; // file-type recognition flag for future flexibility
  box.type = 1;

  box.wall.mean  = box.wall.pos;
  box.wall.ampl  = 0.;
  box.wall.freq  = 0.;
  box.wall.phase = 0.;
  box.wall.dphse = 0.;

  write_config( particle, box, 0, NrParticles, field.NrSubs, out_bld, true );

  return 0;
}
