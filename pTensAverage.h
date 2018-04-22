// ideal component
pIdeal[0][0] /= counter;
pIdeal[0][1] /= counter;
pIdeal[0][2] /= counter;
pIdeal[1][0] /= counter;
pIdeal[1][1] /= counter;
pIdeal[1][2] /= counter;
pIdeal[2][0] /= counter;
pIdeal[2][1] /= counter;
pIdeal[2][2] /= counter;

// non-ideal
pNonIdeal[0][0] /= ( counter * volume );
pNonIdeal[0][1] /= ( counter * volume );
pNonIdeal[0][2] /= ( counter * volume );
pNonIdeal[1][0] /= ( counter * volume );
pNonIdeal[1][1] /= ( counter * volume );
pNonIdeal[1][2] /= ( counter * volume );
pNonIdeal[2][0] /= ( counter * volume );
pNonIdeal[2][1] /= ( counter * volume );
pNonIdeal[2][2] /= ( counter * volume );

// Dissipative
pDissipative[0][0] /= ( counter * volume );
pDissipative[0][1] /= ( counter * volume );
pDissipative[0][2] /= ( counter * volume );
pDissipative[1][0] /= ( counter * volume );
pDissipative[1][1] /= ( counter * volume );
pDissipative[1][2] /= ( counter * volume );
pDissipative[2][0] /= ( counter * volume );
pDissipative[2][1] /= ( counter * volume );
pDissipative[2][2] /= ( counter * volume );

// Random
pRandom[0][0] /= ( counter * volume );
pRandom[0][1] /= ( counter * volume );
pRandom[0][2] /= ( counter * volume );
pRandom[1][0] /= ( counter * volume );
pRandom[1][1] /= ( counter * volume );
pRandom[1][2] /= ( counter * volume );
pRandom[2][0] /= ( counter * volume );
pRandom[2][1] /= ( counter * volume );
pRandom[2][2] /= ( counter * volume );
