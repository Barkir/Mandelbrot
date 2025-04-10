#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../include/main.h"
#include "../include/ngraph.h"
#include "../include/graph.h"
#include "../include/process_cmd.h"

/*
 *  ~This is a mandelbrot manual~
 *
 *  To run this program type in:
 *  ./run --{graph/ngraph} {optimization mode} {iterations}
 *
*/


int main(int argc, char * argv[])
{

    MandelSettings param = {};
    if (ProcessCmd(&param, argc, argv) < 0) return MandelError(MANDEL_PROCESS_CMD_ERROR);

    if (param.GraphFlag == MANDEL_NGRAPH)
    {
        MandelbrotExecute(&param);
        return MandelError(MANDEL_DEFAULT);
    }


    SDLMandelbrotExecute(&param);
    return 0;

}
