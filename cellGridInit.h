// initializing NrCells[3], scale	
for ( i = 0 ; i < 3 ; i++ )
{
    NrCells[i] = int( boxEdge[i] / rcutoff ); // cellnr runs from 0 to NrCells-1
    scale[i] = NrCells[i] * boxRecip[i] ;
    if ( NrCells[i] < 3 ) { simProg << "*** NrCells[" << i << "] = " << NrCells[i] << std::endl ; abort(); }

    // simProg << "NrCells[" << i << "] = " << NrCells[i] << std::endl;
}

simProg << " NrCells[x] = " << NrCells[x] << ", NrCells[y] = " << NrCells[y] << ", NrCells[z] = " << NrCells[z] << std::endl;

// initializing mini[3], maxi[3]
mini[x] = 0; maxi[x] = NrCells[x] - 1;
mini[y] = 0; maxi[y] = NrCells[y] - 1;
mini[z] = 0; maxi[z] = NrCells[z] - 1;

// defining grid matrix 
grid.resize( NrCells[x] );
for ( i = 0 ; i < NrCells[x] ; ++i ) {
    grid[i].resize( NrCells[y] );
    for (  j = 0 ; j < NrCells[y] ; ++j ){
        grid[i][j].resize( NrCells[z] );
        for ( k = 0 ; k < NrCells[z] ; ++k ){
            grid[i][j][k].resize( MaxPerCell ); 	
        } // k 
    }// j
}// i

// defining periodN 
MaxNrCells = std::max( std::max( NrCells[x], NrCells[y] ), NrCells[z]);
simProg << " MaxNrCells = " << MaxNrCells << std::endl;
periodN.resize( MaxNrCells + 2 );
periodR.resize( MaxNrCells + 2 );
for ( i = 0 ; i < MaxNrCells + 2 ; ++ i ){
    periodN[ i ].resize( 3 );
    periodR[ i ].resize( 3 );
}

for ( j = 0 ; j < 3 ; j++ )
{
    periodN[0][j] = NrCells[j] - 1;    // left neighbour of leftmost cell
    periodR[0][j] = -boxEdge[j];       // correction to add to particle j in rij = ri - rj
    for ( i = 1 ;  i < NrCells[j] + 1 ; i++ )
    { 
        periodN[i][j] = i - 1; 	   // same cell
        periodR[i][j] = 0.;
    } // i
    periodN[NrCells[j] + 1][j] = 0;          // right neigbour of rightmost cell
    periodR[NrCells[j] + 1][j] = +boxEdge[j];
} // j
