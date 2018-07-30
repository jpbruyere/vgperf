#include "vgperf.h"

#if WITH_VKVG
#include "vkvg/vgperf.h"
#endif

int star_points[11][2] = {
    { 0, 85 },
    { 75, 75 },
    { 100, 10 },
    { 125, 75 },
    { 200, 85 },
    { 150, 125 },
    { 160, 190 },
    { 100, 150 },
    { 40, 190 },
    { 50, 125 },
    { 0, 85 }
};

void initResults (results_t* res) {
    res->avg_time = 0;
    res->median_time = 0;
    res->std_deriv = 0;
    res->run_min = DBL_MAX;
    res->run_max = DBL_MIN;
}

options_t initOptions (int argc, char *argv[]) {
    options_t opt = {};
    opt.iterations = 100;
    opt.count = 1000;
    opt.width = 1024;
    opt.height = 800;
    opt.present = 0;
    opt.lineWidth = 1;
    opt.drawMode = DM_BOTH;
    opt.antialias = ANTIALIAS_DEFAULT;
    opt.saveImgs = 0;

    char antialias, drawMode;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'w':
                sscanf (argv[++i], "%d", &opt.width);
                break;
            case 'h':
                sscanf (argv[++i], "%d", &opt.height);
                break;
            case 'i':
                sscanf (argv[++i], "%d", &opt.iterations);
                break;
            case 'c':
                sscanf (argv[++i], "%d", &opt.count);
                break;
            case 'l':
                sscanf (argv[++i], "%d", &opt.lineWidth);
                break;
            case 'a':
                sscanf (argv[++i], "%c", &antialias);
                switch (antialias) {
                case 'n':
                    opt.antialias = ANTIALIAS_NONE;
                    break;
                case 'f':
                    opt.antialias = ANTIALIAS_FAST;
                    break;
                case 'g':
                    opt.antialias = ANTIALIAS_GOOD;
                    break;
                case 'b':
                    opt.antialias = ANTIALIAS_BEST;
                    break;
                }
                break;
            case 'd':
                sscanf (argv[++i], "%c", &drawMode);
                switch (drawMode) {
                case 'f':
                    opt.drawMode = DM_FILL;
                    break;
                case 's':
                    opt.drawMode = DM_STROKE;
                    break;
                case 'b':
                    opt.drawMode = DM_BOTH;
                    break;
                }
                break;
            case 'p':
                opt.present = 1;
                break;
            case 's':
                opt.saveImgs = 1;
                break;
            case '?':
                printHelp();
                exit(0);
                break;
            default:
                break;
            }
        }
    }
    return opt;
}

void printHelp () {
    printf ("\t-w x : Set test surface width. (int)\n");
    printf ("\t-h x : Set test surface height. (int)\n");
    printf ("\t-i x : Set iterations count. (int)\n");
    printf ("\t-c x : Set shape occurence count. (int)\n");
    printf ("\t-l x : Set line width (int).\n");
    printf ("\t-a [n|d|f|g|b] : Set antialias (n=none,d=default,f=fast,g=good,b=best).\n");
    printf ("\t-d [f|s|b] : Set draw mode. (f=fill,s=stroke,b=both fill and stroke).\n");
    printf ("\t-p :   Display drawings on screen.\n");
    printf ("\t-s :   Save produced images on disk.\n");
}
void outputResultsHeadRow (options_t* opt) {
    printf ("Iterations: %4d\n", opt->iterations);
    printf ("Count:      %4d\n", opt->count);
    printf ("Line width: %4d\n", opt->lineWidth);
    printf ("Resolution: %d x %d\n", opt->width, opt->height);

    switch (opt->antialias) {
    case ANTIALIAS_NONE:
        printf ("Antialias   = None\n");
        break;
    case ANTIALIAS_DEFAULT:
        printf ("Antialias   = Default\n");
        break;
    case ANTIALIAS_FAST:
        printf ("Antialias   = Fast\n");
        break;
    case ANTIALIAS_GOOD:
        printf ("Antialias   = Good\n");
        break;
    case ANTIALIAS_BEST:
        printf ("Antialias   = Best\n");
        break;
    }
    switch (opt->drawMode) {
    case DM_BOTH:
        printf ("Draw Mode   = FILL and STROKE\n");
        break;
    case DM_FILL:
        printf ("Draw Mode   = FILL\n");
        break;
    case DM_STROKE:
        printf ("Draw Mode   = STROKE\n");
        break;
    }

    printf ("_____________________________________________________________________________________\n");
    printf ("| Library  |  Test Name      |   Min    |   Max    |  Average |  Median  | Std Deriv|\n");
    printf ("|----------|-----------------|----------|----------|----------|----------|----------|\n");
}
void outputResultsOnOneLine (const char* libName, const char* testName, options_t* opt, results_t* res) {
    printf ("| %-8s | %-15s | ",libName, testName);
    printf ("%.6f | %.6f | %.6f | %.6f | %.6f |\n",
            res->run_min, res->run_max, res->avg_time, res->median_time, res->std_deriv);
}

double get_tick (void)
{
    struct timeval now;
    gettimeofday (&now, NULL);
    return (double)now.tv_sec + (double)now.tv_usec / 1000000.0;
}

double median_run_time (double data[], int n)
{
    double temp;
    int i, j;
    for (i = 0; i < n; i++)
        for (j = i+1; j < n; j++)
        {
            if (data[i] > data[j])
            {
                temp = data[j];
                data[j] = data[i];
                data[i] = temp;
            }
        }
    if (n % 2 == 0)
        return (data[n/2] + data[n/2-1])/2;
    else
        return data[n/2];
}

double standard_deviation (const double data[], int n, double mean)
{
    double sum_deviation = 0.0;
    int i;
    for (i = 0; i < n; ++i)
    sum_deviation += (data[i]-mean) * (data[i]-mean);
    return sqrt (sum_deviation / n);
}

void addTest (test_context_t* ctx, const char* testName, PFNtest pfnInit, PFNtest pfnPerform, PFNtest pfncleanup) {
    ctx->tests = (test_t*)realloc(ctx->tests, (ctx->testCount + 1)*sizeof(test_t));
    test_t* test = &ctx->tests[ctx->testCount];
    ctx->testCount++;
    test->test_name = testName;
    test->init = pfnInit;
    test->perform = pfnPerform;
    test->cleanup = pfncleanup;
}

void test_library (options_t* opt, test_context_t* ctx) {
    ctx->libCtx = ctx->init (opt);

    /* Reinitialize random seed to a known state */
    srnd();

    for (uint t=0; t<TESTS_COUNT; t++){
        test_t* test = &ctx->tests[t];
        results_t* res = &ctx->tests[t].results;

        initResults (res);

        double run_time_values[opt->iterations];
        double start_time, stop_time, run_time, run_total = 0;

        for (int i=0; i<opt->iterations; i++) {

            if (test->init)
                test->init (opt, ctx->libCtx);

            start_time = get_tick();

            test->perform (opt, ctx->libCtx);

            stop_time = get_tick();

            run_time = stop_time - start_time;
            run_time_values[i] = run_time;

            if (run_time < res->run_min)
                res->run_min = run_time;
            if (run_time > res->run_max)
                res->run_max = run_time;
            run_total += run_time;

            if (test->cleanup)
                test->cleanup (opt, ctx->libCtx);
        }
        if (opt->saveImgs == 1) {
            char fileName[256] = "";
            strcpy (fileName, ctx->libName);
            strcat (fileName, "_");
            strcat (fileName, test->test_name);
            strcat (fileName, ".png");
            for (int i=0; i<strlen(fileName); i++){
                if (fileName[i] == ' ')
                    fileName[i] = '_';
            }
            ctx->saveImg (ctx->libCtx, fileName);
        }

        res->avg_time = run_total / (double)opt->iterations;
        res->median_time = median_run_time(run_time_values, opt->iterations);
        res->std_deriv = standard_deviation(run_time_values,opt->iterations, res->avg_time);
    }

    ctx->cleanup (ctx->libCtx);
}

int main(int argc, char *argv[]) {
    options_t opt = initOptions(argc, argv);

    test_context_t* libs = (test_context_t*)malloc(0);
    int libCpt = 0;

#if WITH_VKVG
    libs = (test_context_t*)realloc (libs, (libCpt+1)*sizeof(test_context_t));
    init_vkvg_tests (&libs[libCpt]);
    test_library(&opt, &libs[libCpt]);
    libCpt++;
#endif

#if WITH_CAIRO
    libs = (test_context_t*)realloc (libs, (libCpt+1)*sizeof(test_context_t));
    init_cairo_tests (&libs[libCpt]);
    test_library(&opt, &libs[libCpt]);
    libCpt++;
#endif

    outputResultsHeadRow(&opt);
    for (uint t=0; t<TESTS_COUNT; t++){
        for (uint l=0; l<libCpt; l++) {
            outputResultsOnOneLine(libs[l].libName, libs[l].tests[t].test_name, &opt, &libs[l].tests[t].results);
        }
    }




    /*opt.test_name = "vkvg rectangles and";


    results_t res = performTest (&opt);

    //outputResults(&opt, &res);

    */



    free (libs);

    return 0;
}
