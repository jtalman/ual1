 std::string sxfFile = argv[1];

 std::string outputFile = "./out/cpp/";
 outputFile += mysxfbase;
 outputFile += ".sxf";
 std::string mapFile = "./out/cpp/";
 mapFile += mysxfbase;
 mapFile += ".map1";
 std::string twissFile = "./out/cpp/";
 twissFile += mysxfbase;
 twissFile += ".twiss";
 std::string apdfFile = "./eteapot.apdf";
 std::string orbitFile = "./out/cpp/";
 orbitFile += mysxfbase;
 orbitFile += ".orbit";

 int split = 1;            // old split specification
 int splitForBends = 0;    // new split specification
 int splitForQuads = 0;    // new split specification
 int splitForSexts = 0;    // new split specification
 int order = 2;
 int turns;                // specified as 1 in trtrin (for post processing)
                           // might be overwritten tp multiple turns (e.g. 10) in simulatedProbeValues
