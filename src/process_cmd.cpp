#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/main.h"
#include "../include/process_cmd.h"
#include "../include/ngraph.h"

int MandelSettingsCtor(MandelSettings * param, int pksz, float stpx, float stpy, float rmax, const char * tfile)
{
    param->PackSize = pksz;
    param->StepX = stpx;
    param->StepY = stpy;
    param->Rmax = rmax;
    param->FileName = tfile;
    param->TransMat = {1.f, 0.f, 0.f};

    return 0;
}

int ProcessCmd(MandelSettings * param, int argc, char * argv[])
{


    if (argc == 4)
    {

        switch (atoi(argv[2]))
            {
                case 128:   MandelSettingsCtor(param, 4, StepX, StepY, Rmax, "info/mandel128.txt");
                            param->Function = Mandelbrot128;
                            break;

                case 256:   MandelSettingsCtor(param, 8, StepX, StepY, Rmax, "info/mandel256.txt");
                            param->Function = Mandelbrot256;
                            break;

                case 4:     MandelSettingsCtor(param, 4, StepX, StepY, Rmax, "info/mandel4.txt");
                            param->Function = Mandelbrot4;
                            break;

                case 1:     MandelSettingsCtor(param, 1, StepX, StepY, Rmax, "info/mandeldefault.txt");
                            param->Function = Mandelbrot;
                            break;


                default:    return MandelError(MANDEL_UNKNOWN_OPTIMIZATION_ERROR);
            }

        if (!strcmp(argv[1], "--ngraph"))
        {

            param->NumIter = atoi(argv[3]);
            param->GraphFlag = MANDEL_NGRAPH;
            return MANDEL_NGRAPH;
        }

        else if (!strcmp(argv[1], "--graph"))
        {
            param->NumIter = atoi(argv[3]);
            param->GraphFlag = MANDEL_GRAPH;
            return MANDEL_GRAPH;
        }
    }

    return -fprintf(stderr, "Typo in: ./run <--ngraph/--graph> <optimization mode> <iterations number>\n");
}

int MandelError(int error)
{
    // fprintf(stdout, "%d::%s\n", error, Err2Str(error));
    return error;
}

const char * Err2Str(int error)
{
    switch (error)
    {
        case MANDEL_FILE_OPEN_ERROR:            return "FILE_OPEN_ERROR";
        case MANDEL_MEMALLOC_ERROR:             return "MEMALLOC_ERROR";
        case MANDEL_UNKNOWN_OPTIMIZATION_ERROR: return "UNKNOW_OPTIMIZATION_ERROR";
        case MANDEL_PROCESS_CMD_ERROR:          return "PROCESS_CMD_ERROR";
        case MANDEL_NULLPOINTER_ERROR:          return "NULLPOINTER_ERROR";
        case MANDEL_DEFAULT:                    return "DEFAULT";
        default:                                return "UNKNOWN";
    }
}
