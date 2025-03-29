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

const float MaxY = 20.;
const float MaxX = 20.;
const int Niter = 256;

const float dx = 0.01;
const float dy = 0.01;
const float rMax = 4;
const float ampl = 20;

const float SDL_SCREEN_WIDTH = 1000;
const float SDL_SCREEN_HEIGHT = 1000;

// TODO: header зависимости
// TODO: дебажная версия с санитайзером

enum Mandelflags
{
    GRAPH,
    NO_GRAPH
};

void inline add4(float * a, float * b, float * save);
void inline sub4(float * a, float * b, float * save);
void inline print4(float * val);
void inline mul4(float * a, float * b, float * save);
void inline cmple4(float * a, float * b, float * save);
void inline mulnum(float * a, float val, float * save);
int inline isnull4(float * a);
