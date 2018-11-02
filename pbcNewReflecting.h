mx = (int) round( ( particles[i].r.X - boxHalve[x] ) * boxRecip[x] );
my = (int) round( ( particles[i].r.Y - boxHalve[y] ) * boxRecip[y] );
mz = (int) round( ( particles[i].r.Z - boxHalve[z] ) * boxRecip[z] );

if ( abs(mx) > 1 || abs(my) > 1 || abs(mz) > 1 )     // unacceptably large displacement
{
    simProg << "*** particle " << i << " has escaped" << std::endl;
    simProg << mx << "  " << my << " " << std::endl;
    abort();
} // abs(mx)

particles[i].r.X -= mx * boxEdge[x];    // apply periodic boundary conditions in the x and y directions
particles[i].r.Y -= my * boxEdge[y];

if ( abs(mz) == 1.0 ) {

    // simProg << " Particle " << i << " has temporarily gone out of the box " << std::endl;

    // simProg << " initial particle position = " << particles[i].r.Z << ", initial particle velocity = " << particles[i].w.Z << std::endl;

    zOld                = particles[i].r.Z - particles[i].w.Z * dt;	// calculate the previous z-position
    tApp                = ( particles[i].w.Z > 0. ) ? ( ( boxEdge[z] - zOld ) / particles[i].w.Z ) : ( zOld / fabs( particles[i].w.Z ) ); 
    tSep                = dt - tApp;					// time in which particle separates from top face of the box
    particles[i].w.Z 	*= -1.0;					// reverse the velocity of the particle: away from the top of the box

    // simProg << " zOld = " << zOld << ", tApp = " << tApp << ", tSep = " << tSep << std::endl;

    if ( tSep < 0.0 || tSep > dt ){
        simProg << "** tSep is     = " 	<< tSep << " **" << std::endl;
        simProg << "** tApp is     = " 	<< tApp << " **" << std::endl;
        simProg << "** velocity is = " 	<< particles[i].w.Z << " **" << std::endl;
        simProg << "** zOld is     = " 	<< zOld << " **" << std::endl;
        abort();
    }

    particles[i].r.Z 	= ( particles[i].w.Z < 0. ) ? ( boxEdge[z] +  particles[i].w.Z * tSep ) : ( particles[i].w.Z * tSep );	// finally, integrate EOM from the top face of the box with reverse velocity

    // simProg << " final particle position = " << particles[i].r.Z << ", initial particle velocity = " << particles[i].w.Z << std::endl;
}
