#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>

#include "defs.h"

#include "headers.H"

using namespace std;

int main()
{
  int    const vers = 130110;
  double const dEtotwarn = 1.e+0;
  double const dEkinwarn = 1.e-4;
  double const Qchckwarn = 1.e-6;

  int     NrParticles, NrSubs;
  int     step, PastSteps;
  int     i, si;
  int     mx,my,mz;
  EneData Ene;
  double  EkinTst, Eold;
//double  dvi[3];
  vctr3D  dis;
  vctr3D  vi,    dvi;
  vctr4D  qdoti, dqdoti;
  vctr4D  quati, gradq2;
//double  diffq2, lambda, norm2;
//int     iter;
  vctr3D  omegi, domegi;
  double  Qchck;
  double  MaxDisp, MaxFrc, MaxRot, MaxTrq, MaxQchck;
  double  dEtot, dEtotMax, dEkin, dEkinMax;
  double  tmp1, tmp2, tmp3;
  int     iter;

  time_t  time_begin,time_end;
  struct  tm * timeinfo;

//----------------------------------------------------------------------------------------

  cout << endl;
  cout << "Simul version" << vers << endl << endl;
  time (&time_begin);
  timeinfo = localtime ( &time_begin );
  cout << "The start date/time is: " << asctime (timeinfo) << endl << endl;

// initialize to suppress 'may be used uninitialized in this function' warnings

  PastSteps = 0;

// Read control parameters

  CtrlData      ctrl;
  read_control( ctrl );

// Read force field

  FieldData   field;
  read_field( field );
  field.grav = ctrl.grav;

/*
#ifdef ComList
  if ( ( field.NrSubs > 1 ) && ( field.rcut == field.mxcut ) )
  {
    { cout << "*** conflicting cut-offs in ComList ***" << endl
           << field.rcut << "  " << field.mxcut << endl; abort(); }
  }
#else
  if ( field.rcut != field.mxcut )
  {
    { cout << "*** conflicting cut-offs in non-ComList ***" << endl
           << field.rcut << "  " << field.mxcut << endl; abort(); }
  }
#endif
*/

  NrParticles = field.NrParticles;
  NrSubs      = field.NrSubs;

// Read start configuration

/*
  ParticleData RefParticle;
  RefParticle.sub.resize(NrSubs);                     // without a constructor or ...
  vector<ParticleData>  particle(field.NrParticles, RefParticle);
*/
                                                      // ... same trick with a constructor
		    //       creates and zeros
  vector<ParticleData>  particle( NrParticles, ParticleData(NrSubs) );
  vector<ParticleData>  partiOld( NrParticles, ParticleData(NrSubs) ); // for testing purposes

//particle.reserve(NrParticles);
//particle.resize(NrParticles);
//for ( i = 0 ; i < NrParticles ; i++ )
//{
//  particle[i].mass = field.mass;
//}

  BoxData box;

  read_config( particle, box, PastSteps, NrParticles, NrSubs );

  cout << "--- Final preparations ---" << endl << endl;

  if ( box.wall.on != ctrl.wall.on )
    { cout << "*** conflicting information on wall ***" << endl
           << box.wall.on << "  " << ctrl.wall.on << endl; abort(); }
  if ( ctrl.wall.on )
  {
    if ( ctrl.wall.mean - ctrl.wall.ampl < 2. * field.mxcut )
      { cout << "*** wall reaches too low ***" << endl
             << ctrl.wall.mean - ctrl.wall.ampl << " <  " << field.mxcut << endl; abort(); }
    tmp1            = box.wall.pos;                      // store old box.wall data before overwritten
    tmp2            = box.wall.phase;
    tmp3            = box.wall.freq;
    box.wall        = ctrl.wall;
    box.wall.dphse  = twopi * box.wall.freq * ctrl.dt;   // phase change per step
    box.wall.dirctn = box.dimens - 1;
    if ( PastSteps == 0 )
    {
      box.wall.pos   =   ctrl.wall.mean;
      box.wall.phase = - box.wall.dphse;                  // create start at mean for t = 0
    }
    else // box has history, create smooth continuation of wall motion
    {
      if ( abs( tmp1 - ctrl.wall.mean) > ctrl.wall.ampl )
        { cout << "*** wall deviation larger than amplitude ***" << endl
               << abs( tmp1 - ctrl.wall.mean ) << " > " << ctrl.wall.ampl << endl; abort(); }
      tmp2 -= twopi * round( tmp2 / twopi );             // final phase previous run
      box.wall.phase = asin( ( tmp1 - ctrl.wall.mean )   // initial phase current run:
                               / ctrl.wall.ampl );       //   possible phase jump, direction = up
      if ( abs( tmp2 ) < halfpi )      { i =   1; }      // was the wall moving up or down?
                                  else { i = - 1; }
      if ( tmp3 < 0 )                  { i = - i; }      // prev run counter-clockwise?
      if ( tmp3 * ctrl.wall.freq < 0 ) { i = - i; }      // direction reversal?
      if ( box.wall.freq < 0 )         { i = - i; }      // new run counter-clockwise?
      if ( i < 0 )                                       // start wall down i.s.o. up
      {
        if ( box.wall.phase < 0 ) { box.wall.phase = -pi - box.wall.phase; }
                             else { box.wall.phase =  pi - box.wall.phase; }
      }
      box.wall.pos   = box.wall.mean + box.wall.ampl * sin(box.wall.phase);
    }
    cout << "wall        : "                  << endl
         << "  pos   old : " << tmp1          << endl
         << "        new : " << box.wall.pos  << endl
         << "  phase old : " << tmp2          << endl
         << "  phase new : " << ctrl.wall.pos << endl << endl;
  } // ctrl.wall.on

  box.update();

  box.BckgrndFric = ctrl.BckgrndFric;

// reconstruct sub-particles

  switch ( box.dimens )
  {
    case 2 :
      if ( ! field.flat )
        { cout << "*** simulation 2D, particle 3D" << endl; abort(); }
      box.edge.comp[z] = 3.2 * field.mxcut;                      // 3 cells in perpendicular direction
      cout << "  Superfluous 3rd box edge reset to " << box.edge.comp[z] << endl << endl;
      box.update();
      if ( ang2D != z ) { cout << "*** Fatal ang2D: " << ang2D << endl; abort(); }
      field.inert.comp[ang2D][ang2D] = field.inert.comp[z][z];  // extract single relevant element
      for ( i = 0 ; i < NrParticles ; i ++ )
      {
        particle[i].pos.comp[z] = box.halve.comp[z]; // center
        particle[i].phi2rotmat();
        for ( si = 0 ; si < NrSubs ; si ++ )
          { particle[i].sub[si].pos = particle[i].pos
                                    + particle[i].rotmat * field.sub[si].pos; }
      } // i
      break; // 2

    case 3 :
      for ( i = 0 ; i < NrParticles ; i ++ )
      {
        particle[i].quat2rotmat();
        particle[i].angmom = particle[i].rotmat * ( field.inert * particle[i].omega );
        for ( si = 0 ; si < NrSubs ; si ++ )
          { particle[i].sub[si].pos = particle[i].pos
                                    + particle[i].rotmat * field.sub[si].pos; }
      } // i
      break; // 3

    default :
      cout << "*** Unknown box dimension: " << box.dimens << endl ; abort();
  } // box.dimens

  if ( min( min( box.edge.comp[x], box.edge.comp[y] ), box.edge.comp[z] ) < 3.1 * field.rcut )
  {
    cout << "*** Box too small relative to rcut" << endl;
    abort();
  }

  cout << " Warning levels set at" << endl;
  cout << "  dEtot : " << scientific << dEtotwarn << endl;
  cout << "  dEkin : " << scientific << dEkinwarn << endl;
  cout << "  Qchck : " << scientific << Qchckwarn << endl << endl;

  dEtotMax = 0.;
  dEkinMax = 0.;

  ctrl.lognext = 0; // ctrl.logintrvl;
  ctrl.dcdnext = 0; // ctrl.dcdintrvl;

  ofstream statis ( "output.statis" );
  ofstream EnePot ( "output.Epot" );

  cout << "--- starting simulation ---" << endl << endl;

//========================================================================================

  for ( step = 0 ; step < ctrl.NrSteps ; step++ )
  {

    if ( box.wall.on )
    {
      box.wall.phase += box.wall.dphse;
      box.wall.phase -= twopi * round( box.wall.phase / twopi );
      box.wall.pos    = box.wall.mean + box.wall.ampl * sin(box.wall.phase);
//    cout << "wall " << box.wall.pos << endl;
    }

//----------------------------------------------------------------------------------------

    force( particle, field, box, Ene );

//----------------------------------------------------------------------------------------

    Ene.trns = 0.;
    Ene.rot  = 0.;
    EkinTst  = 0.;
    MaxDisp  = 0.;
    MaxFrc   = 0.;
    MaxRot   = 0.;
    MaxTrq   = 0.;
    MaxQchck = 0.;

    partiOld = particle; // for testing purposes

// update positions of particles ---------------------------------------------------------

#ifdef Trace
   cout << "--- Verlet step pos ---" << endl << endl;
#endif

    for ( i = 0 ; i < NrParticles ; i++ )
    {

      dvi = particle[i].frc * ( ctrl.dt * field.minv );    // change of c.o.m. velocity
       vi = particle[i].vel + dvi * 0.5;                   // full-step velocity
      
      particle[i].vel += dvi;                              // update half-step velocity
      particle[i].pos += particle[i].vel * ctrl.dt;        // update full-step position

      mx = (int) round( ( particle[i].pos.comp[x] - box.halve.comp[x] ) * box.recip.comp[x] );
      my = (int) round( ( particle[i].pos.comp[y] - box.halve.comp[y] ) * box.recip.comp[y] );
      mz = (int) round( ( particle[i].pos.comp[z] - box.halve.comp[z] ) * box.recip.comp[z] );

      if ( abs(mx) > 1 || abs(my) > 1 || abs(mz) > 1 )     // unacceptably large displacement
      {
        cout << "*** particle " << i << " has escaped" << endl;
        cout << mx << "  " << my << "  " << mz << endl;
        particle[i].echo(box);
        abort();
      } // abs(mx)

      particle[i].pos.comp[x] -= mx * box.edge.comp[x];    // apply periodic boundary conditions
      particle[i].pos.comp[y] -= my * box.edge.comp[y];
      particle[i].pos.comp[z] -= mz * box.edge.comp[z];

      Ene.trns += ( vi * vi ) * field.mass;
      MaxDisp   = max( MaxDisp, vi * vi );
      MaxFrc    = max( MaxFrc , particle[i].frc * particle[i].frc );

    } // i

    MaxDisp = sqrt( MaxDisp ) * ctrl.dt;
    MaxFrc  = sqrt( MaxFrc  );

// update orientations of particles and positions of subs --------------------------------

    if ( NrSubs == 1 ) // particle with merely one sub, does not rotate
    {
#ifdef Trace
      cout << "--- skipping Verlet step rot ---" << endl << endl;
#endif
      for ( i = 0 ; i < NrParticles ; i ++ )
      {
        particle[i].sub[0].pos = particle[i].pos;
      }
    }

    else // particle with multiple subs, does rotate

    {
#ifdef Trace
      cout << "--- Verlet step rot ---" << endl << endl;
#endif

      switch ( box.dimens )
      {

        case 2 : // flat space

          for ( i = 0 ; i < NrParticles ; i ++ )
          {
            domegi.comp[ang2D]  = particle[i].trq.comp[ang2D]                         // ang velocity change
                                  * field.InrtInv.comp[ang2D][ang2D] * ctrl.dt;
             omegi.comp[ang2D]  = particle[i].omega.comp[ang2D]                       // ang vel at full step
                                  + 0.5 * domegi.comp[ang2D];
            particle[i].omega.comp[ang2D] += domegi.comp[ang2D];                      // update half-step ang vel
            particle[i].quat .comp[ang2D] += particle[i].omega.comp[ang2D] * ctrl.dt; // update full-step angle
            particle[i].phi2rotmat();                                                 // new rotation matrix
            for ( si = 0 ; si < NrSubs ; si ++ )
              {
                particle[i].sub[si].pos = particle[i].pos                             // new sub positions
                                        + particle[i].rotmat * field.sub[si].pos;
              }
            Ene.rot += omegi.comp[ang2D] * omegi.comp[ang2D] * field.inert.comp[ang2D][ang2D];
            MaxRot   = max( MaxRot, pow( particle[i].omega.comp[ang2D] , 2 ) );
            MaxTrq   = max( MaxTrq, pow( particle[i].trq  .comp[ang2D] , 2 ) );
          } // i
          break; // case 2
  
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        case 3: // full space // integration using simplest leap frog, Svanberg MolPhys 92 1085 (1997) scheme 0

          for ( i = 0 ; i < NrParticles ; i ++ )
          {
// cout << "omega: "; particle[i].omega.echo();
// cout << "quat : "; particle[i].quat.echo();
            quati               = particle[i].quat; // store
            omegi               = particle[i].omega;
            particle[i].angmom += particle[i].trq * ctrl.dt;
            particle[i].omega   =  field.InrtInv * ( ( ~ particle[i].rotmat ) * particle[i].angmom );
            particle[i].quat   += particle[i].omega2qdot() * ctrl.dt;
            particle[i].quat   /= sqrt( particle[i].quat * particle[i].quat );
// cout << "omega: "; particle[i].omega.echo();
// cout << "quat : "; particle[i].quat.echo();
            omegi     = ( omegi + particle[i].omega ) * 0.5;
            Ene.rot  += omegi * ( field.inert * omegi );
            dqdoti    = ( particle[i].quat - quati ) * ctrl.rdt;
            particle[i].qdot2omega(dqdoti,Qchck);
            Qchck  = abs(Qchck);
            if ( Qchck > Qchckwarn )
              { cout << "+++ quat dot dqot = " << Qchck << " +++" << endl; }
            MaxQchck = max( MaxQchck, Qchck );
//          Ene.rot += omegi * ( field.inert * omegi );
            particle[i].quat2rotmat();
            for ( si = 0 ; si < NrSubs ; si ++ )
            {
              particle[i].sub[si].pos =   particle[i].pos
                                        + particle[i].rotmat * field.sub[si].pos;
            }
            MaxRot = max( MaxRot, omegi * omegi );
            MaxTrq = max( MaxTrq, particle[i].trq * particle[i].trq );
          } // i
          break; // case 3
  
      } // box.dimens

      MaxRot = sqrt( MaxRot ) * ctrl.dt;
      MaxTrq = sqrt( MaxTrq );

    } // NrSubs > 1

//  integration step completed -----------------------------------------------------------

    Ene.trns *= 0.5;
    Ene.rot  *= 0.5;
    Ene.kin   = Ene.trns + Ene.rot;
    Ene.tot   = Ene.pot + Ene.kin;

// sub-based kinetic energy (for testing purposes)

    EkinTst = 0.;
    for ( i = 0 ; i < NrParticles ; i++ )
    {
      for ( si = 0 ; si < NrSubs ; si ++ )
      {
        dis = particle[i].sub[si].pos - partiOld[i].sub[si].pos;
        box.PBC(dis);
        particle[i].sub[si].vel = dis * ctrl.rdt;
        EkinTst += ( particle[i].sub[si].vel * particle[i].sub[si].vel ) * field.sub[si].mass;
      }
    }
    EkinTst *= 0.5;
/*  cout << step << "   "
         << Etot << "  " << Epot << "  " << Ekin << "  "
         << MaxDisp << "  " << pairs << endl;
*/

    if ( step == ctrl.lognext ) 
    {
      cout << step << endl;
      statis << scientific
             << step * ctrl.dt << "   "
             << Ene.tot << "  " << Ene.pot << "  " << Ene.kin << "  "
             << MaxDisp << "  " << MaxFrc << "  " << MaxRot << "  " << MaxTrq << endl;
      EnePot << step * ctrl.dt << "  "
             << Ene.trns << "  " << Ene.rot
             << Ene.fld << "  " <<  Ene.pairs << "  " << Ene.grav << "  " << Ene.wall << endl;
      ctrl.lognext += ctrl.logintrvl;
    } // lognext

    if ( step == ctrl.dcdnext ) 
    {
      write_dcd( particle, box, step, NrParticles, NrSubs );
      ctrl.dcdnext += ctrl.dcdintrvl;
    } // dcdnext

    if ( step != 0 )
    {
      dEtot    = abs( Ene.tot - Eold );
      dEtotMax = max( dEtotMax, dEtot );
      if ( dEtot > dEtotwarn )
      {
        cout << "+++ Too large energy change +++ "
             << step << " : "
             << setiosflags(ios::fixed) << setprecision(4)
             << setw(10) << Ene.tot << " vs "
             << setw(10) << Eold    << "   "
             << setw(10) << Ene.tot - Eold << endl;
//      abort();
      }

      dEkin    = abs( Ene.kin - EkinTst );
      dEkinMax = max( dEkinMax, dEkin );
      if ( dEkin > dEkinwarn )
      {
        cout << "+++ Inconsistent kinetic energies +++ "
             << step << " : "
             << setiosflags(ios::fixed) << setprecision(4)
             << setw(10) << Ene.kin << " vs "
             << setw(10) << EkinTst << "   "
             << setw(10) << Ene.kin - EkinTst << endl;
//      abort();
      } // dEkin
    } // step != 0

    Eold = Ene.tot;

  } // step

  write_config( particle, box, step, NrParticles, NrSubs, out_conf, true );
  statis.close();
  EnePot.close();

  cout << "Maximum total energy change in 1 step  : " << scientific << dEtotMax << endl;
  cout << "Maximum diff. between kinetic energies : " << scientific << dEkinMax << endl;
  cout << "Maximum deviation quat dot quatdot     : " << scientific << MaxQchck << endl << endl;

  cout << "--- time ---" << endl << endl;

  time (&time_end);
  timeinfo = localtime ( &time_end );
  cout << "The end date/time is: " << asctime (timeinfo) << endl << endl;

  cout << "runtime : " << difftime ( time_end, time_begin ) << " seconds" << endl << endl;

  cout << "--- done ---" << endl;

}
