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
const float rMax = 4;

const int SDL_SCREEN_WIDTH = 1200;
const int SDL_SCREEN_HEIGHT = 1200;

const float step_x = 2.0 / SDL_SCREEN_WIDTH;
const float step_y = 2.0 / SDL_SCREEN_HEIGHT;


enum Mandelflags
{
    SSE_GRAPH,
    SSE_NGRAPH,
    SSE_DEFAULT,

    SSE_128,
    SSE_256,
    SSE_512,
    SSE_VEC,
    SSE_SNGL,

    SSE_FILE_OPEN_ERROR,
    SSE_MEMALLOC_ERROR
};

struct Transform
{
    float ampl;
    float delx;
    float dely;
};

typedef void (*Mandelfunc) (void*, int*, void *);

struct Mandelparam
{
    int pksz;
    float stpx;
    float stpy;
    float rmax;
    int iter;
    const char * tfile;
    int graph_flag;

    Mandelfunc func;

};

int process_cmd(Mandelparam * param, int argc, char * argv[]);
int mandel_param_init(Mandelparam * param, int pksz, float stpx, float stpy, float rmax, const char * tfile);
