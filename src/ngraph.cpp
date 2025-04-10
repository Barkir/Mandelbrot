#include <stdlib.h>
#include <x86intrin.h>
#include <stdio.h>
#include <stdint.h>

#include "../include/main.h"
#include "../include/ngraph.h"
#include "../include/process_cmd.h"


int MandelbrotExecute(MandelSettings * param)
{
    uint64_t start = 0, end = 0;
    int * MandelArray = NULL;
    FILE * MandelFile = fopen(param->FileName, "w+");
    if (!MandelFile) return MandelError(MANDEL_FILE_OPEN_ERROR);

    for (int i = 0; i < param->NumIter; i++)
    {
        MandelArray = (int*) aligned_alloc(param->PackSize * 4, SDL_SCREEN_WIDTH * SDL_SCREEN_HEIGHT * sizeof(int));
        start = _rdtsc();
        param->Function(param, MandelArray);
        end = _rdtsc();
        fprintf(MandelFile, "%ld\n", end - start);
        free(MandelArray);
    }

    fclose(MandelFile);
}
