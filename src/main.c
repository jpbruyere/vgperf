#include "vgperf.h"

#ifdef WITH_VKVG
#include "vkvg/vgperf.h"
#endif
#ifdef WITH_CAIRO
#include "cairo/vgperf.h"
#endif

#ifdef WITH_SKIA
#include "skia/vgperf.h"
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

void _print_usage_and_exit () {
	printf("\nUsage: vgperf [options]\n\n");
	printf("\t-i iterations:\tSet iterations count (default=100).\n");
	printf("\t-n size:\tSet shape occurence count (default=100).\n");
	printf("\t-x width:\tOutput surface width (default=512).\n");
	printf("\t-y height:\tOutput surface height (default=512).\n");
	printf("\t-g gpu_type:\tSet prefered GPU type:\n");
	printf("\t\t\t - 0: Other\n");
	printf("\t\t\t - 1: Integrated (second choice)\n");
	printf("\t\t\t - 2: Discrete (first choice)\n");
	printf("\t\t\t - 3: Virtual\n");
	printf("\t\t\t - 4: Cpu\n");
	printf("\t-vsync:\t\tEnable VSync, disabled by default.\n");
	printf ("\t-O:\t\tPerform tests offscreen.\n");
	printf("\t-t index:\tRun only a single test, zero based index.\n");

	printf("\t-a [n|d|f|g|b]:\tSet antialias.\n");
	printf("\t\t\t - n: none\n");
	printf("\t\t\t - d: default\n");
	printf("\t\t\t - f: fast\n");
	printf("\t\t\t - g: good\n");
	printf("\t\t\t - b: best\n");
	printf ("\t-d [f|s|b]:\tSet draw mode.\n");
	printf("\t\t\t - f: fill (default)\n");
	printf("\t\t\t - s: stroke\n");
	printf("\t\t\t - b: fill and stroke\n");
	printf ("\t-l line_width:\tSet line width (default=1).\n");
	printf ("\t-c [b|r|s]:\tSet line caps.\n");
	printf("\t\t\t - b: butt (default)\n");
	printf("\t\t\t - r: round\n");
	printf("\t\t\t - s: square\n");
	printf ("\t-j [m|b|r]:\tSet line joins.\n");
	printf("\t\t\t - m: mitter (default)\n");
	printf("\t\t\t - b: bevel\n");
	printf("\t\t\t - r: round\n");
	printf ("\t-f [c|l|r|s]:\tSet fill type.\n");
	printf("\t\t\t - c: solid color (default)\n");
	printf("\t\t\t - l: linear gradient\n");
	printf("\t\t\t - r: radial gradient\n");
	printf("\t\t\t - s: surface\n");
	printf ("\t-rs:\t\tComma separated report with only median values.\n");
	printf ("\t-s:\t\tSave produced images on disk.\n");
	printf("\t-h:\t\t\tthis help message.\n");

	//printf ("\t-o [l|r|u|c|t|s|x] : Set shape (l=line(default),r=rectangle,u=rounded rectangle,c=circle,t=triangle,s=star,x=random).\n");
	printf("\n");
	exit(-1);
}

options_t initOptions (int argc, char *argv[]) {
	options_t opt = {0};
	opt.runSingleTest = -1;//run all tests
	opt.iterations = 100;
	opt.count = 100;
	opt.width = 512;
	opt.height = 512;
	opt.present = 1;
	opt.lineWidth = 1;
	opt.drawMode = DM_FILL;
	opt.antialias = ANTIALIAS_DEFAULT;
	opt.capStyle = LINE_CAP_BUTT;
	opt.joinStyle = LINE_JOIN_MITER;
	opt.fillType = FILL_TYPE_SOLID;
	opt.shape = SHAPE_LINE;
	opt.saveImgs = 0;
	opt.report = 0;

	char antialias, drawMode, lineCaps, lineJoins, fillType, shape;
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 't':
				sscanf (argv[++i], "%d", &opt.runSingleTest);
				break;
			case 'x':
				sscanf (argv[++i], "%d", &opt.width);
				break;
			case 'y':
				sscanf (argv[++i], "%d", &opt.height);
				break;
			case 'i':
				sscanf (argv[++i], "%d", &opt.iterations);
				break;
			case 'n':
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
				default:
					_print_usage_and_exit();
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
				default:
					_print_usage_and_exit();
				}
				break;
			case 'o':
				sscanf (argv[++i], "%c", &shape);
				switch (shape) {
				case 'l':
					opt.shape = SHAPE_LINE;
					break;
				case 'r':
					opt.shape = SHAPE_RECTANGLE;
					break;
				case 'u':
					opt.shape = SHAPE_ROUNDED_RECTANGLE;
					break;
				case 'c':
					opt.shape = SHAPE_CIRCLE;
					break;
				case 's':
					opt.shape = SHAPE_STAR;
					break;
				case 't':
					opt.shape = SHAPE_TRIANGLE;
					break;
				case 'x':
					opt.shape = SHAPE_RANDOM;
					break;
				default:
					_print_usage_and_exit();
				}
				break;
			case 'c':
				sscanf (argv[++i], "%c", &lineCaps);
				switch (lineCaps) {
				case 'b':
					opt.capStyle = LINE_CAP_BUTT;
					break;
				case 'r':
					opt.capStyle = LINE_CAP_ROUND;
					break;
				case 's':
					opt.capStyle = LINE_CAP_SQUARE;
					break;
				default:
					_print_usage_and_exit();
				}
				break;
			case 'j':
				sscanf (argv[++i], "%c", &lineJoins);
				switch (lineJoins) {
				case 'm':
					opt.joinStyle = LINE_JOIN_MITER;
					break;
				case 'b':
					opt.joinStyle = LINE_JOIN_BEVEL;
					break;
				case 'r':
					opt.joinStyle = LINE_JOIN_ROUND;
					break;
				default:
					_print_usage_and_exit();
				}
				break;
			case 'f':
				sscanf (argv[++i], "%c", &fillType);
				switch (fillType) {
				case 'c':
					opt.fillType = FILL_TYPE_SOLID;
					break;
				case 'l':
					opt.fillType = FILL_TYPE_LINEAR;
					break;
				case 'r':
					opt.fillType = FILL_TYPE_RADIAL;
					printf ("Radial gradient option is not implemented.\n");
					exit(0);
					break;
				case 's':
					opt.fillType = FILL_TYPE_SURFACE;
					break;
				default:
					_print_usage_and_exit();
				}
				break;
			case 'O':
				opt.present = 0;
				break;
			case 's':
				opt.saveImgs = 1;
				break;
			case 'r':
				opt.report = 1;
				break;
			case 'h':
			default:
				_print_usage_and_exit();
			}
		} else
			_print_usage_and_exit();
	}
	return opt;
}

void outputResultsHeadRow (options_t* opt) {
	printf ("Iterations  = %4d\n", opt->iterations);
	printf ("Count       = %4d\n", opt->count);
	printf ("Line width  = %4d\n", opt->lineWidth);
	printf ("Shape       = ");
	switch (opt->shape) {
	case SHAPE_LINE:
		printf ("Line\n");
		break;
	case SHAPE_RECTANGLE:
		printf ("Rectangle\n");
		break;
	case SHAPE_ROUNDED_RECTANGLE:
		printf ("Rounded Rectangle\n");
		break;
	case SHAPE_CIRCLE:
		printf ("Circle\n");
		break;
	case SHAPE_TRIANGLE:
		printf ("Triangle\n");
		break;
	case SHAPE_STAR:
		printf ("Star\n");
		break;
	case SHAPE_RANDOM:
		printf ("Random\n");
		break;
	}

	printf ("Line caps   = ");
	switch (opt->capStyle) {
	case LINE_CAP_BUTT:
		printf ("BUTT\n");
		break;
	case LINE_CAP_ROUND:
		printf ("ROUND\n");
		break;
	case LINE_CAP_SQUARE:
		printf ("SQUARE\n");
		break;
	}
	printf ("Line joins  = ");
	switch (opt->joinStyle) {
	case LINE_JOIN_MITER:
		printf ("MITTER\n");
		break;
	case LINE_JOIN_BEVEL:
		printf ("BEVEL\n");
		break;
	case LINE_JOIN_ROUND:
		printf ("ROUND\n");
		break;
	}
	printf ("Fill mode   = ");
	switch (opt->fillType) {
	case FILL_TYPE_SOLID:
		printf ("Solid color\n");
		break;
	case FILL_TYPE_LINEAR:
		printf ("Linear gradient\n");
		break;
	case FILL_TYPE_RADIAL:
		printf ("Radial gradient\n");
		break;
	case FILL_TYPE_SURFACE:
		printf ("Surface\n");
		break;
	}

	printf ("Resolution  = %d x %d\n", opt->width, opt->height);

	printf ("Antialias   = ");
	switch (opt->antialias) {
	case ANTIALIAS_NONE:
		printf ("None\n");
		break;
	case ANTIALIAS_DEFAULT:
		printf ("Default\n");
		break;
	case ANTIALIAS_FAST:
		printf ("Fast\n");
		break;
	case ANTIALIAS_GOOD:
		printf ("Good\n");
		break;
	case ANTIALIAS_BEST:
		printf ("Best\n");
		break;
	}
	printf ("Draw Mode   = ");
	switch (opt->drawMode) {
	case DM_BOTH:
		printf ("FILL and STROKE\n");
		break;
	case DM_FILL:
		printf ("FILL\n");
		break;
	case DM_STROKE:
		printf ("STROKE\n");
		break;
	}

	printf ("____________________________________________________________________________________________\n");
	printf ("| Test Name       |    Library      |   Min    |   Max    |  Average |  Median  | Std Deriv|\n");
	printf ("|-----------------|-----------------|----------|----------|----------|----------|----------|\n");
}
void outputResultsOnOneLine (const char* libName, const char* testName, options_t* opt, results_t* res) {
	printf ("| %-15s | %-15s | ",testName, libName);
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
vgperf_context_t* vgperf_context_create (const char *libName, PFNinitLibrary initLibrary, PFNcleanupLibrary cleanupLibrary, PFNtest present) {
	vgperf_context_t* ctx = (vgperf_context_t*)calloc(1,sizeof(vgperf_context_t));
	ctx->libName = libName;
	ctx->init = initLibrary;
	ctx->cleanup = cleanupLibrary;
	ctx->present = present;
	return ctx;
}
void addTest (vgperf_context_t* ctx, const char* testName, void* pfnInit, void* pfnPerform, void* pfncleanup) {
	ctx->tests = (test_t*)realloc(ctx->tests, (ctx->testCount + 1)*sizeof(test_t));
	test_t* test = &ctx->tests[ctx->testCount];
	ctx->testCount++;
	test->test_name = testName;
	test->init = (PFNtest)pfnInit;
	test->perform = (PFNtest)pfnPerform;
	test->cleanup = (PFNtest)pfncleanup;
}
/**
 * @brief save resulting surface to disk
 * @param library context
 * @param current test
 */
void saveResultingImg (vgperf_context_t* ctx, test_t* test) {
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

void run_single_test (options_t* opt, vgperf_context_t* ctx, test_t* test) {

	results_t* res = &test->results;
	initResults (res);

	double run_time_values[opt->iterations];
	double start_time, stop_time, run_time, run_total = 0;

	for (int i=0; i<opt->iterations; i++) {

		reset_rnd();

		if (test->init)
			test->init (opt, ctx->libCtx);

		start_time = get_tick();

		test->perform (opt, ctx->libCtx);

		if (opt->present == 1 && ctx->present)
			ctx->present (opt, ctx->libCtx);

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

	if (opt->saveImgs == 1)
		saveResultingImg (ctx, test);

	res->avg_time = run_total / (double)opt->iterations;
	res->median_time = median_run_time(run_time_values, opt->iterations);
	res->std_deriv = standard_deviation(run_time_values,opt->iterations, res->avg_time);
}

void test_library (options_t* opt, vgperf_context_t* ctx) {
	ctx->libCtx = ctx->init (opt);

	/* Reinitialize random seed to a known state */
	reset_rnd();

	if (opt->runSingleTest < 0) {
		for (int t=0; t<ctx->testCount; t++)
			run_single_test(opt,ctx,&ctx->tests[t]);
	}else
		run_single_test(opt,ctx,&ctx->tests[opt->runSingleTest]);

	ctx->cleanup (ctx->libCtx);
}

int main(int argc, char *argv[]) {
	options_t opt = initOptions(argc, argv);

	vgperf_context_t* libs[10];
	int libCpt = 0;

#ifdef WITH_VKVG
	libCpt += init_vkvg_tests (&libs[libCpt]);
#endif

#ifdef WITH_CAIRO
	libCpt += init_cairo_tests (&libs[libCpt]);
#endif

#ifdef WITH_SKIA
	libCpt += init_skia_tests (&libs[libCpt]);
#endif


	for (int i=0; i<libCpt; i++)
		test_library(&opt, libs[i]);

	if (opt.report) {
		for (int l=0; l<libCpt; l++)
			printf(",%s",libs[l]->libName);
		printf("\n");
		for (int t=0; t<libs[0]->testCount; t++){
			printf ("%s", libs[0]->tests[t].test_name);
			for (int l=0; l<libCpt; l++)
				printf (",%f", libs[l]->tests[t].results.median_time);
			printf("\n");
		}

	} else {
		outputResultsHeadRow(&opt);
		for (int t=0; t<TESTS_COUNT; t++){
			for (int l=0; l<libCpt; l++) {
				if (t < libs[l]->testCount)
					outputResultsOnOneLine(libs[l]->libName, libs[l]->tests[t].test_name, &opt, &libs[l]->tests[t].results);
			}
		}
		printf ("--------------------------------------------------------------------------------------------\n\n");
	}


	for (uint i=0; i<libCpt; i++){
		free (libs[i]->tests);
		free (libs[i]);
	}

	return 0;
}
