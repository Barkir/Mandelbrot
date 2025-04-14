#ifndef MAIN_H
#define MAIN_H
#endif

#define DEBUG

#ifdef DEBUG
#define LOG(...) \
    fprintf(stdout, "(%s) %d: ", __func__, __LINE__);   \
    fprintf(stdout, __VA_ARGS__);  \
    fprintf(stdout, "\n")           \

#endif

#ifndef DEBUG
#define LOG(...)
#endif

const int Niter = 256;
const float Rmax = 4;

const int SDL_SCREEN_WIDTH = 800;
const int SDL_SCREEN_HEIGHT = 800;

const float StepX = 2.0 / (SDL_SCREEN_WIDTH - 1);
const float StepY = 2.0 / (SDL_SCREEN_HEIGHT - 1);


enum MandelFlags
{
    MANDEL_GRAPH,
    MANDEL_NGRAPH,
    MANDEL_DEFAULT,

    MANDEL_SSE,
    MANDEL_AVX,
    MANDEL_AVX512,
    MANDEL_ARRAY,
    MANDEL_SINGLE,

    MANDEL_FILE_OPEN_ERROR,
    MANDEL_MEMALLOC_ERROR,
    MANDEL_UNKNOWN_OPTIMIZATION_ERROR,
    MANDEL_PROCESS_CMD_ERROR,
    MANDEL_NULLPOINTER_ERROR

};

struct Transform
{
    float ampl;
    float delx;
    float dely;
};

typedef void (*MandelFunc) (void*, int*);

struct MandelSettings
{
    int PackSize;
    float StepX;
    float StepY;
    float Rmax;
    int NumIter;
    const char * FileName;
    int GraphFlag;

    MandelFunc Function;
    Transform TransMat;

};
