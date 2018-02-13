#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include <vector>

using namespace std;

#include "defs.h"

#include "force.H"

int force (       vector<ParticleData>&  particle,
            const FieldData&             field,
            const BoxData&               box,
                  EneData&               Ene
          )
{

  int const MaxPerCell = 100;

//int const x = 0;
//int const y = 1;
//int const z = 2;

  int    NrCells[3],MaxNrCells;
  double scale[3];

  int    i,j;
  int    ii,jj;
  int    mi[3],m,mj[3];
  int    mini[3],maxi[3];
  vctr3D dR, rij;
  double rij2;
  int    si,sj;
  vctr3D rsisj;
  double rsisj2,rsisj_2,sgm_rsisj2,sgm_rsisj6;
  double ffsisj,eesisj;
  vctr3D fsisj;
  double dist, dist_2, sgm_d2, sgm_d6, eeijw, ffijw;
  int    dm[13][3] = { {  0,  0,  1 },
                       {  1,  0, -1 },
                       {  1,  0,  0 },
                       {  1,  0,  1 },
                       { -1,  1, -1 },
                       { -1,  1,  0 },
                       { -1,  1,  1 },
                       {  0,  1, -1 },
                       {  0,  1,  0 },
                       {  0,  1,  1 },
                       {  1,  1, -1 },
                       {  1,  1,  0 },
                       {  1,  1,  1 } };

#ifdef Trace
  cout << "--- force calculation ---" << endl << endl;
#endif

  for ( i = 0 ; i < 3 ; i++ )
  {
    NrCells[i] = int( box.edge.comp[i] / field.mxcut ); // cellnr runs from 0 to NrCells-1
    scale  [i] = NrCells[i] * box.recip.comp[i];
    if ( NrCells[i] < 3 ) { cout << "*** NrCells[" << i << "] = " << NrCells[i] << endl ; abort(); }
  }

// periodic boundary conditions

  MaxNrCells = max( max( NrCells[x], NrCells[y] ), NrCells[z]);
  int    periodN[ MaxNrCells + 2 ][3]; 
  double periodR[ MaxNrCells + 2 ][3]; 

  for ( j = 0 ; j < 3 ; j++ ) 
  {
    periodN[0][j] = NrCells[j] - 1;          // left neighbour of leftmost cell
    periodR[0][j] = -box.edge.comp[j];       // correction to add to particle j in rij = ri - rj
    for ( i = 1 ;  i < NrCells[j] + 1 ; i++ )
    {
      periodN[i][j] = i - 1; // same cell
      periodR[i][j] = 0.;
    } // i
    periodN[NrCells[j] + 1][j] = 0;          // right neigbour of rightmost cell
    periodR[NrCells[j] + 1][j] = +box.edge.comp[j];
  } // j


// generate c.o.m.-based gridlist

// cout << "gridlist" << endl;
// cout << NrCells[x] << "  " << NrCells[y] << "  " << NrCells[z] << endl;
// cout <<   scale[x] << "  " <<   scale[y] << "  " <<   scale[z] << endl;

  int grid[NrCells[x]][NrCells[y]][NrCells[z]][MaxPerCell+1];

  for ( mi[x] = 0 ; mi[x] < NrCells[x] ; mi[x]++ )
  {
    for ( mi[y] = 0 ; mi[y] < NrCells[y] ; mi[y]++ )
    {
      for ( mi[z] = 0 ; mi[z] < NrCells[z] ; mi[z]++ )
      {
        grid[mi[x]][mi[y]][mi[z]][0] = 0;
      } // miz
    } // miy
  } // mix

  mini[x] = 0; maxi[x] = NrCells[x] - 1;
  mini[y] = 0; maxi[y] = NrCells[y] - 1;
  mini[z] = 0; maxi[z] = NrCells[z] - 1;
  if ( box.wall.on )
  {
    mini[box.wall.dirctn] ++; // cells at edge should be empty, for security
    maxi[box.wall.dirctn] --;
  }

  for ( i = 0 ; i < field.NrParticles ; i++ )
  {
    mi[x] = int( particle[i].pos.comp[x] * scale[x] );
    mi[y] = int( particle[i].pos.comp[y] * scale[y] );
    mi[z] = int( particle[i].pos.comp[z] * scale[z] );
    if ( mi[x] < mini[x] || mi[x] > maxi[x]   // debug
      || mi[y] < mini[y] || mi[y] > maxi[y]
      || mi[z] < mini[z] || mi[z] > maxi[z] )
    {
      cout << "*** particle " << i << " outside box" << endl;
      particle[i].pos.echo();
      cout << mini[x] << " < " << mi[x] << " < " << maxi[x] << endl;
      cout << mini[y] << " < " << mi[y] << " < " << maxi[y] << endl;
      cout << mini[z] << " < " << mi[z] << " < " << maxi[z] << endl;
      abort();
    }
    if ( grid[mi[x]][mi[y]][mi[z]][0] == MaxPerCell )
    {
      cout << "*** cell overfull" << endl;
      cout << mi[x] << "  " << mi[y] << "  " << mi[z] << endl;
      abort();
    }
    grid[mi[x]][mi[y]][mi[z]][0] ++ ;
//  cout << i << "  " << mix << "  " << miy << "  " << miz << "  " << grid[mix][miy][miz][0] << endl;
    grid[mi[x]][mi[y]][mi[z]][ grid[mi[x]][mi[y]][mi[z]][0] ] = i;
  } // i

// zero forces

  Ene.fld   = 0.;
  Ene.pairs = 0;
  for ( i = 0 ; i < field.NrParticles ; i ++ )
  {
    particle[i].frc  = null3D;
    particle[i].trq  = null3D;
    for ( si = 0 ; si < field.NrSubs ; si ++ )
    {
      particle[i].sub[si].frc = null3D;
    }
  }

// calculate energy and forces

  for ( mi[x] = 0 ; mi[x] < NrCells[x] ; mi[x]++ )
  {
    for ( mi[y] = 0 ; mi[y] < NrCells[y] ; mi[y]++ )
    {
      for ( mi[z] = 0 ; mi[z] < NrCells[z] ; mi[z]++ )
      {
        for ( ii = 1 ; ii <= grid[mi[x]][mi[y]][mi[z]][0] ; ii++ )
        {
          i = grid[mi[x]][mi[y]][mi[z]][ii];
// printf("i  %i %i %i %i %i \n",mix,miy,miz,ii,i);

          // particle j in same cell as i
          dR = null3D;
          for ( jj = ii + 1 ; jj <= grid[mi[x]][mi[y]][mi[z]][0] ; jj++ )
          {
            j = grid[mi[x]][mi[y]][mi[z]][jj];
// printf("j1 %i %i %i %i %i \n",mix,miy,miz,jj,j);

#include "pairforce.h"

          } // jj

          // particle j in neighbour cell to i
          for ( m = 0 ; m < 13 ; m++ )
          {
            mj[x]      = periodN[ mi[x] + dm[m][x] + 1 ][x];
            mj[y]      = periodN[ mi[y] + dm[m][y] + 1 ][y];
            mj[z]      = periodN[ mi[z] + dm[m][z] + 1 ][z];
            dR.comp[x] = periodR[ mi[x] + dm[m][x] + 1 ][x];
            dR.comp[y] = periodR[ mi[y] + dm[m][y] + 1 ][y];
            dR.comp[z] = periodR[ mi[z] + dm[m][z] + 1 ][z];
            for ( jj = 1 ; jj <= grid[mj[x]][mj[y]][mj[z]][0] ; jj++ )
            {
              j = grid[mj[x]][mj[y]][mj[z]][jj];
// printf("j2 %i %i %i %i %i %i \n",m,mjx,mjy,mjz,jj,j);

#include "pairforce.h"

            } // jj
          } // n
        } // ii
      } // miz
    } // miy
  } // mix

  Ene.pot = Ene.fld;

// walls

  if ( box.wall.on )
  {
    Ene.wall = 0.;
    for ( i = 0 ; i < field.NrParticles ; i ++ )
    {
      for ( si = 0 ; si < field.NrSubs ; si ++ )
      {
        dist = particle[i].sub[si].pos.comp[box.wall.dirctn] - box.wall.pos;
        if ( dist < field.rcut )
        {
          dist_2 = 1. / ( dist * dist );
          sgm_d2 = field.sigma2 * dist_2;
          sgm_d6 = sgm_d2 * sgm_d2 * sgm_d2;
          eeijw  = field.eps4 * sgm_d6 * (       sgm_d6 - 1. ) - field.Ecut;
          ffijw  = field.eps4 * sgm_d6 * ( 12. * sgm_d6 - 6. ) * sgm_d2;
          particle[i].sub[si].frc.comp[box.wall.dirctn] += ffijw * dist;
          Ene.wall += eeijw;
        }
      } // j
    } // i

    Ene.grav = 0.;
    for ( i = 0 ; i < field.NrParticles ; i ++ )
    {
      particle[i].frc.comp[box.wall.dirctn] -= field.grav * field.mass;
      Ene.grav += particle[i].pos.comp[box.wall.dirctn];
    }
    Ene.grav *= field.mass * field.grav;

    Ene.pot += Ene.wall + Ene.grav;

  } // wall.on

// friction on subs -- only to equilibrate boxes

  if ( box.BckgrndFric != 0. )
    {
      for ( i = 0 ; i < field.NrParticles ; i++ )
      {
        for ( si = 0 ; si < field.NrSubs ; si ++ )
        {
          particle[i].sub[si].frc -= particle[i].sub[si].vel * box.BckgrndFric;
        } // si
      }  // i
    } // BckgrndFric

// convert subforces into total generalized forces on particles

  for ( i = 0 ; i < field.NrParticles ; i ++ )
  {
    for ( si = 0 ; si < field.NrSubs ; si ++ )
    {
      particle[i].frc +=                                                   particle[i].sub[si].frc;
      particle[i].trq += ( ( particle[i].sub[si].pos - particle[i].pos ) ^ particle[i].sub[si].frc );
    } // si
  } // i

/*
  cout << "forces:" << endl;
  for ( i = 0 ; i < 10 ; i ++ )
    { particle[i].frc.echo(); }

  cout << "done with forces" << endl;
*/
  return 0;
}
