for(i=0;i<dragForceOnCyl.size();++i){
    for(j=1; j<partIdxInCyl[i][0]; ++j){
        unsigned int partIdx=partIdxInCyl[i][j];

        dragForceOnCyl[i] += ( particles[partIdx].fC.X + particles[partIdx].fR.X + particles[partIdx].fD.X );
        liftForceOnCyl[i] += ( particles[partIdx].fC.Y + particles[partIdx].fR.Y + particles[partIdx].fD.Y );
    }
}

nSamples++;

if( step % saveCount == 0 )
{
    std::ofstream writeDragLiftForce("./data/dragForceOnCylinder.dat", std::ios_base::app );  
    if ( ! writeDragLiftForce ) { simProg << "*** The file could not be opened for writing purposes exist *** \n Aborting !! " << std::endl; abort(); }

    writeDragLiftForce << "\n";

    for( i=0; i<dragForceOnCyl.size(); ++i ){
        writeDragLiftForce << dragForceOnCyl[i] << "\t" << liftForceOnCyl[i] << "\t" << nSamples << "\n";

        dragForceOnCyl[i]=0.;
        liftForceOnCyl[i]=0.;
    }
    nSamples=0;

    writeDragLiftForce.close();

}
