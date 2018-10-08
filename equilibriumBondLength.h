i = 0;
while ( i < solidCount ){
        particles[i].equiBondLength.resize( particles[i].bondIndex[0] );
        i++;
}

// assign vectors each particle pair
i = 0;
while ( i < solidCount ){

    for ( k=1; k<= particles[i].bondIndex[0]; ++k ){

        j = particles[i].bondIndex[k] ;
        particles[i].equiBondLength[k] = particles[i].r - particles[j].r;
    }

i++;
}
