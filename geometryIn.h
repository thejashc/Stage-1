simProg << "----------------------------" << std::endl;
simProg << "Reading Relevant file names" << std::endl;
simProg << "---------------------------" << std::endl;

std::string readSolidFrom;
std::string readFluidFrom;
#if CAPILLARY_CYLINDER && HARD_SPHERES
    std::string readColloidsFrom;
#endif

readParam.open("./geometryFileList.in", std::ios_base::in);
if ( ! readParam  ) { simProg << "*** input file is empty" << std::endl; abort(); }

readParam >> buffer >> readSolidFrom;	    readParam.ignore(256,'\n');		
readParam >> buffer >> readFluidFrom;	    readParam.ignore(256,'\n');		
#if CAPILLARY_CYLINDER && HARD_SPHERES
    readParam >> buffer >> readColloidsFrom;	readParam.ignore(256,'\n');     
#endif

simProg << "Reading solid structure from " << readSolidFrom << std::endl;
simProg << "Reading fluid structure from " << readFluidFrom << std::endl;
#if CAPILLARY_CYLINDER && HARD_SPHERES
    simProg << "Reading colloid centers from " << readColloidsFrom << "\n" << std::endl;
#endif

readParam.close();
