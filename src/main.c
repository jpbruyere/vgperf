#include "vgperf.h"

#if WITH_VKVG
#include "vkvg/vgperf.h"
#endif

char const * tests_names [] = {
    "Line stroke",
    "rect",
    "rectangles",
    "Circle drawing"
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
    opt.drawMode = DM_FILL;

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
            case 'f':
                opt.drawMode = DM_FILL;
                break;
            case 's':
                opt.drawMode = DM_STROKE;
                break;
            case 'b':
                opt.drawMode = DM_BOTH;
                break;
            default:
                break;
            }
        }
    }
    return opt;
}

void printHelp () {
    printf ("\t-w x : Set test surface width.\n");
    printf ("\t-h x : Set test surface height.\n");
    printf ("\t-i x : Set iterations count.\n");
    printf ("\t-c x : Set shape occurence count.\n");
    printf ("\t-f :   Set shape draw mode to fill.\n");
    printf ("\t-s :   Set shape draw mode to stroke.\n");
    printf ("\t-b :   Set shape draw mode to fill and stroke.\n");
}
void outputResultsHeadRow (options_t* opt) {
    printf ("Iterations: %4d\n", opt->iterations);
    printf ("Count:      %4d\n", opt->count);
    printf ("Resolution: %d x %d\n", opt->width, opt->height);

    printf ("__________________________________________________________________________________________\n");
    printf ("| Library  |  Test Name      | DM |   Min    |   Max    |  Average |  Median  | Std Deriv|\n");
    printf ("|----------|-----------------|----|----------|----------|----------|----------|----------|\n");
}
void outputResultsOnOneLine (const char* libName, const char* testName, options_t* opt, results_t* res) {
    printf ("| %-8s | %-15s | ",libName, testName);
    switch (opt->drawMode) {
    case DM_BOTH:
        printf ("B  | ");
        break;
    case DM_FILL:
        printf ("F  | ");
        break;
    case DM_STROKE:
        printf ("S  | ");
        break;
    }
    printf ("%.6f | %.6f | %.6f | %.6f | %.6f |\n",
            res->run_min, res->run_max, res->avg_time, res->median_time, res->std_deriv);
}

void outputResults (options_t* opt, results_t* res) {
    printf ("Test name: %s\n", opt->test_name);
    printf ("\nOptions\n");
    printf ("=======\n");
    printf ("Iterations  = %d\n", opt->iterations);
    printf ("Shape count = %d\n", opt->count);
    printf ("Canva size  = %d x %d\n", opt->width, opt->height);
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
    printf ("\nResults\n");
    printf ("=======\n");
    printf ("Minimum = %f (sec)\n", res->run_min);
    printf ("Maximum = %f (sec)\n", res->run_max);
    printf ("Average = %f (sec)\n", res->avg_time);
    printf ("Median  = %f (sec)\n", res->median_time);
    printf ("Std reriv = %f \n", res->std_deriv);
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

void test_library (options_t* opt, test_context_t* ctx) {
    ctx->libCtx = ctx->init (opt);

    for (uint t=0; t<TESTS_COUNT; t++){

        /* Reinitialize random seed to a known state */
        srnd();

        results_t* res = &ctx->tests[t].results;

        initResults (res);

        double run_time_values[opt->iterations];
        double start_time, stop_time, run_time, run_total = 0;

        ctx->tests[t].init (opt, ctx->libCtx);

        for (int i=0; i<opt->iterations; i++) {

            start_time = get_tick();

            ctx->tests[t].perform (opt, ctx->libCtx);

            stop_time = get_tick();

            run_time = stop_time - start_time;
            run_time_values[i] = run_time;

            if (run_time < res->run_min)
                res->run_min = run_time;
            if (run_time > res->run_max)
                res->run_max = run_time;
            run_total += run_time;
        }

        ctx->tests[t].cleanup (opt, ctx->libCtx);

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
            outputResultsOnOneLine(libs[l].libName, tests_names[t], &opt, &libs[l].tests[t].results);
        }
    }




    /*opt.test_name = "vkvg rectangles and";


    results_t res = performTest (&opt);

    //outputResults(&opt, &res);

    */



    free (libs);

    return 0;
}
