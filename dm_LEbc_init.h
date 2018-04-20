/*
dm_LEbc	= {	
	{  0,  0,  1 },			
	{  1,  0, -1 },                 
	{  1,  0,  0 },                 
	{  1,  0,  1 },                 

	{  dmCell1,  1, -1 },                 // Cells in the xy plane in ( z - 1 )
	{  dmCell2,  1, -1 },                                                      
	{  dmCell3,  1, -1 },                                                      
	{  dmCell4,  1, -1 },                                                      

	{  dmCell1,  1,  0 },                 // Cells in the xy plane in ( z + 0 )                                     
	{  dmCell2,  1,  0 },                 
	{  dmCell3,  1,  0 },                                                      
	{  dmCell4,  1,  0 },                                                      

	{  dmCell1,  1,  1 },                 // Cells in the xy plane in ( z + 1 )                                     
	{  dmCell2,  1,  1 },                                                      
	{  dmCell3,  1,  1 },
	{  dmCell4,  1,  1 }                  
};
*/

dm_LEbc[0][0] =  0;	// {0, 0, 1}
dm_LEbc[0][1] =  0;
dm_LEbc[0][2] =  1;

dm_LEbc[1][0] =  1;	// {1, 0, -1}
dm_LEbc[1][1] =  0;
dm_LEbc[1][2] = -1;

dm_LEbc[2][0] =  1;	// {1, 0, 0}
dm_LEbc[2][1] =  0;
dm_LEbc[2][2] =  0;

dm_LEbc[3][0] =  1;	// {1, 0, 1}
dm_LEbc[3][1] =  0;
dm_LEbc[3][2] =  1;

dm_LEbc[4][0] =  dmCell1; 	// {dmCell1, 1, -1}, Cells in the xy plane in ( z - 1 )
dm_LEbc[4][1] =  1;
dm_LEbc[4][2] = -1;

dm_LEbc[5][0] = dmCell2;	// {dmCell2, 1, -1}
dm_LEbc[5][1] =  1;
dm_LEbc[5][2] = -1;

dm_LEbc[6][0] = dmCell3;	// {dmCell3, 1, -1}
dm_LEbc[6][1] =  1;
dm_LEbc[6][2] = -1;

dm_LEbc[7][0] = dmCell4;	// {dmCell4, 1, -1}
dm_LEbc[7][1] =  1;
dm_LEbc[7][2] = -1;

dm_LEbc[8][0] = dmCell1;	// {dmCell1, 1, 0}, Cells in the xy plane in ( z + 0 )
dm_LEbc[8][1] =  1;
dm_LEbc[8][2] =  0;

dm_LEbc[9][0] = dmCell2;	// {dmCell2, 1, 0}
dm_LEbc[9][1] =  1;
dm_LEbc[9][2] =  0;

dm_LEbc[10][0] = dmCell3;	// {dmCell3, 1, 0}
dm_LEbc[10][1] =  1;
dm_LEbc[10][2] =  0;

dm_LEbc[11][0] = dmCell4;	// {dmCell4, 1, 0}
dm_LEbc[11][1] =  1;
dm_LEbc[11][2] =  0;

dm_LEbc[12][0] = dmCell1;	// {dmCell1, 1, 1}, Cells in the xy plane in ( z + 1 )
dm_LEbc[12][1] =  1;
dm_LEbc[12][2] =  1;

dm_LEbc[13][0] = dmCell2;	// {dmCell2, 1, 1}
dm_LEbc[13][1] =  1;
dm_LEbc[13][2] =  1;

dm_LEbc[14][0] = dmCell3;	// {dmCell3, 1, 1}
dm_LEbc[14][1] =  1;
dm_LEbc[14][2] =  1;

dm_LEbc[15][0] = dmCell4;	// {dmCell4, 1, 1}
dm_LEbc[15][1] =  1;
dm_LEbc[15][2] =  1;
