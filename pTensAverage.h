/*
pTensor[0][0] /= counter;
pTensor[0][1] /= counter;
pTensor[0][2] /= counter;
pTensor[1][0] /= counter;
pTensor[1][1] /= counter;
pTensor[1][2] /= counter;
pTensor[2][0] /= counter;
pTensor[2][1] /= counter;
pTensor[2][2] /= counter;
*/

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
