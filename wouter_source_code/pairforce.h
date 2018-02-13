//  Calculate the force between particles i and j

  rij = particle[i].pos - ( particle[j].pos + dR );

  // spherical particles for the moment

  rij2 = rij * rij;

  if ( rij2 < field.mxcut2 )
  {

    for ( si = 0 ; si < field.NrSubs ; si ++ )
    {
      for ( sj = 0 ; sj < field.NrSubs ; sj ++ )
      {
        rsisj = particle[i].sub[si].pos - ( particle[j].sub[sj].pos + dR );
//      rsisj.echo();

        rsisj2 = rsisj * rsisj;
   
        if ( rsisj2 < field.rcut2 )
        {
//cout << i << "  " << si << "  " << j << "  " << sj << "  " << rsisj2 << endl;
          rsisj_2    = 1. / rsisj2;
          sgm_rsisj2 = field.sigma2 * rsisj_2;
          sgm_rsisj6 = sgm_rsisj2 * sgm_rsisj2 * sgm_rsisj2;
          eesisj     = field.eps4 * sgm_rsisj6 * (       sgm_rsisj6 - 1. ) - field.Ecut;
          ffsisj     = field.eps4 * sgm_rsisj6 * ( 12. * sgm_rsisj6 - 6. ) * sgm_rsisj2;
                                 // ffij = - d eeij / d rij * 1 / rij^2

          fsisj = rsisj * ffsisj;
//cout << "fisijsj "; fsisj.echo();

          particle[i].sub[si].frc += fsisj;
          particle[j].sub[sj].frc -= fsisj;

          Ene.fld   += eesisj;
          Ene.pairs ++;
        } // rsisj2

      } // sj
    } // si


  } // rij2
