#ifndef PROCESS_CMD

int ProcessCmd(MandelSettings * param, int argc, char * argv[]);
int MandelSettingsCtor(MandelSettings * param, int pksz, float stpx, float stpy, float rmax, const char * tfile);;
const char * Err2Str(int error);
int MandelError(int error);


#endif
