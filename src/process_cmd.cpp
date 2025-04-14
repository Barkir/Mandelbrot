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

    if (argc >= 3)
    {
    if (!strcmp(argv[1], "--graph"))
    {
        param->GraphFlag = MANDEL_GRAPH;
        if (!strcmp(argv[2], "-sse"))
        {
            MandelSettingsCtor(param, 4, StepX, StepY, Rmax, "info/mandel128.txt");
            param->Function = Mandelbrot128;
        }
        else if (!strcmp(argv[2], "-avx"))
        {
            MandelSettingsCtor(param, 8, StepX, StepY, Rmax, "info/mandel256.txt");
            param->Function = Mandelbrot256;
        }
        else if (!strcmp(argv[2], "-array"))
        {
            MandelSettingsCtor(param, 4, StepX, StepY, Rmax, "info/mandel4.txt");
            param->Function = Mandelbrot4;
        }
        else if (!strcmp(argv[2], "-def"))
        {
            MandelSettingsCtor(param, 1, StepX, StepY, Rmax, "info/mandeldefault.txt");
            param->Function = Mandelbrot;
        }
        else
        {
            MandelSettingsCtor(param, 1, StepX, StepY, Rmax, "info/mandeldefault.txt");
            param->Function = Mandelbrot;
        }
        return MANDEL_GRAPH;
    }

    if (!strcmp(argv[1], "--ngraph"))
    {
        if (argc == 4)
        {


            if (!strcmp(argv[2], "-sse"))
            {
                MandelSettingsCtor(param, 4, StepX, StepY, Rmax, "info/mandel128.txt");
                param->Function = Mandelbrot128;
            }


            else if (!strcmp(argv[2], "-avx"))
            {
                MandelSettingsCtor(param, 8, StepX, StepY, Rmax, "info/mandel256.txt");
                param->Function = Mandelbrot256;
            }

            else if (!strcmp(argv[2], "-array"))
            {
                MandelSettingsCtor(param, 4, StepX, StepY, Rmax, "info/mandel4.txt");
                param->Function = Mandelbrot4;
            }

            else if (!strcmp(argv[2], "-def"))
            {
                MandelSettingsCtor(param, 1, StepX, StepY, Rmax, "info/mandeldefault.txt");
                param->Function = Mandelbrot;
            }

            else
            {
                MandelSettingsCtor(param, 1, StepX, StepY, Rmax, "info/mandeldefault.txt");
                param->Function = Mandelbrot;
            }
        }

        return MANDEL_NGRAPH;
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
