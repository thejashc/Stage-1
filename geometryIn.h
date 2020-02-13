#if !(RESTART)
    simProg << "----------------------------" << std::endl;
    simProg << "Reading Relevant file names" << std::endl;
    simProg << "---------------------------" << std::endl;

    std::string readSolidFrom;
    std::string readFluidFrom;
    #if RANDOM_PILLAR_ARRAY
        std::string readPillarCentersFrom;
    #endif

    readParam.open("./geometryFileList.in", std::ios_base::in);
    if ( ! readParam  ) { simProg << "*** input file is empty" << std::endl; abort(); }

    readParam >> buffer >> readSolidFrom;       readParam.ignore(256,'\n');    
    simProg << "Reading solid structure from " << readSolidFrom << std::endl;

    readParam >> buffer >> readFluidFrom;       readParam.ignore(256,'\n');    
    simProg << "Reading fluid structure from " << readFluidFrom << std::endl;

    #if RANDOM_PILLAR_ARRAY
        readParam >> buffer >> readPillarCentersFrom;       readParam.ignore(256,'\n');    

        simProg << "Reading pillar centers from " << readPillarCentersFrom << "\n" << std::endl;
    #endif

    readParam.close();
#endif

