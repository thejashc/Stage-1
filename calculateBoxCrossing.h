mx = (int) round( ( particles[i].r.X - boxHalve[x] ) * boxRecip[x] );	
my = (int) round( ( particles[i].r.Y - boxHalve[y] ) * boxRecip[y] );
mz = (int) round( ( particles[i].r.Z - boxHalve[z] ) * boxRecip[z] );

if ( abs(mx) > 1 || abs(my) > 1 || abs(mz) > 1 )     // unacceptably large displacement
{
    simProg << "*** particle " << i << " of type " << particles[i].type 
            << " with positions : " << particles[i].r << " has escaped in step " << step << std::endl;
    simProg << mx << "  " << my << "  " << mz << std::endl;
    abort();
} // abs(mx)

if ( step % saveCount == 0 ){
    MSDStats << step << "\t" << i <<  "\t" << particleBoxCrossing[i][0] << "\t" << particleBoxCrossing[i][1] << "\t" << particleBoxCrossing[i][2] << std::endl;
}

particleBoxCrossing[i][0] += mx;        // Particle crossing the box in x,y,z directions
particleBoxCrossing[i][1] += my;
particleBoxCrossing[i][2] += mz;
