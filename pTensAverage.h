// ideal component
pIdeal[0][0] /= pcounter;
pIdeal[0][1] /= pcounter;
pIdeal[0][2] /= pcounter;
pIdeal[1][0] /= pcounter;
pIdeal[1][1] /= pcounter;
pIdeal[1][2] /= pcounter;
pIdeal[2][0] /= pcounter;
pIdeal[2][1] /= pcounter;
pIdeal[2][2] /= pcounter;

// Conservative - non-kinetic 
pNonIdeal[0][0] /= ( pcounter * volume );
pNonIdeal[0][1] /= ( pcounter * volume );
pNonIdeal[0][2] /= ( pcounter * volume );
pNonIdeal[1][0] /= ( pcounter * volume );
pNonIdeal[1][1] /= ( pcounter * volume );
pNonIdeal[1][2] /= ( pcounter * volume );
pNonIdeal[2][0] /= ( pcounter * volume );
pNonIdeal[2][1] /= ( pcounter * volume );
pNonIdeal[2][2] /= ( pcounter * volume );

// Conservative - kinetic
pNonIdealKin[0][0] /= ( pcounter * volume );
pNonIdealKin[0][1] /= ( pcounter * volume );
pNonIdealKin[0][2] /= ( pcounter * volume );
pNonIdealKin[1][0] /= ( pcounter * volume );
pNonIdealKin[1][1] /= ( pcounter * volume );
pNonIdealKin[1][2] /= ( pcounter * volume );
pNonIdealKin[2][0] /= ( pcounter * volume );
pNonIdealKin[2][1] /= ( pcounter * volume );
pNonIdealKin[2][2] /= ( pcounter * volume );

// Dissipative
pDissipative[0][0] /= ( pcounter * volume );
pDissipative[0][1] /= ( pcounter * volume );
pDissipative[0][2] /= ( pcounter * volume );
pDissipative[1][0] /= ( pcounter * volume );
pDissipative[1][1] /= ( pcounter * volume );
pDissipative[1][2] /= ( pcounter * volume );
pDissipative[2][0] /= ( pcounter * volume );
pDissipative[2][1] /= ( pcounter * volume );
pDissipative[2][2] /= ( pcounter * volume );

// Random
pRandom[0][0] /= ( pcounter * volume );
pRandom[0][1] /= ( pcounter * volume );
pRandom[0][2] /= ( pcounter * volume );
pRandom[1][0] /= ( pcounter * volume );
pRandom[1][1] /= ( pcounter * volume );
pRandom[1][2] /= ( pcounter * volume );
pRandom[2][0] /= ( pcounter * volume );
pRandom[2][1] /= ( pcounter * volume );
pRandom[2][2] /= ( pcounter * volume );
