/* BACKPROJECTION C++ IMPLEMENTATION Using pthreads
 * Last worked on by: Kevin He, Summer 2017.
 * splits voxels up and assigns each group to a thread
 */
#include <math.h>
#include <matrix.h>
#include <mex.h>
#include <pthread.h>
#include <thread>

#define MAX_LASERS 100
#define MAX_NX 20000

/* Struct to pass pthread starting function */
typedef struct  _backproject_args {
    double *d1l;
    double *laserpos;
    double *cpos;
    double *d4l;
    double *sI;
    double shift;
    double tpp;
    int nlasers;
    int nx;
    int nt;
    int nvoxels;
    double *output;
    double intensity_corr;
    int vox_indx_start;
    int vox_indx_end;
    double *voxels;
    double *lasernormal;
    double *cameranormal;
} backproject_args;

/* Define distance */
inline double distance2(double* v1,double *v2)
{
    double a = v1[0]-v2[0];
    double b = v1[1]-v2[1];
    double c = v1[2]-v2[2];

    return sqrt(a*a+b*b+c*c);
}

/* Define approximation */
inline double round(double x)
{
    return floor(x+0.5);
}

/* Forward decleration */
void *voxel_calc(void *input);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    /* threading variables */
    unsigned int num_threads = std::thread::hardware_concurrency();

    #define xsI prhs[0]       // datset.data
    #define xlaserpos prhs[1] // Laser positions on wall
    #define xvoxels prhs[2]   // Voxel space
    #define xcpos prhs[3]     // Camera positions on wall
    #define xlcop prhs[4]     // Laser origin
    #define xccop prhs[5]     // Camera origin
    #define xtpp prhs[6]      // deltat
    #define xshift prhs[7]    // t0
    #define xInCorr prhs[8]   // Include distance factors? (new feat 05/17)
    #define xcameranorm prhs[9]
    #define xlasernorm prhs[10]

    /* Macros for output argument */
    #define xoutput plhs[0]

    /* Defining doubles */
    double *sI = mxGetPr(xsI);                       // data
    double *laserpos = mxGetPr(xlaserpos);           // emitter positions (laser spots)
    double *voxels = mxGetPr(xvoxels);               // voxels
    double *cpos = mxGetPr(xcpos);                   // probe positions (camera spots)
    double *geo_laser_cop = mxGetPr(xlcop);          // laser position
    double *geo_camera_cop = mxGetPr(xccop);         // camera position
    double tpp = *((double *)mxGetData(xtpp));       // deltat - distance traveled by a photon per time bin
    double shift = *((double *)mxGetData(xshift));   // t0 - distance corresponding to index 0
    double InCorr = *((double *)mxGetData(xInCorr)); // if InCorr == 1 physical corrections, ==0 otherwise
    double *cameranormal = mxGetPr(xcameranorm);
    double *lasernormal = mxGetPr(xlasernorm);

    /* Defining integers */
    int nlasers = mxGetN(xsI);     // number of laser spot positions on wall
    int nx = mxGetN(xcpos);        // number of camera spot positions on wall
    int nt = mxGetM(xsI)/nx;       // number of time bins (exclusive - final bin is nt-1, and first is 0)
    int nvoxels = mxGetN(xvoxels); // number of voxels
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);


    /* Writing on the command line */
    mexPrintf("[C++] Starting BACKPROJECTION PTHREAD... \n");
    printf("[C++] Starting BACKPROJECTION PTHREAD... \n");

    int x = 0,p = 0, t = 0, i; //x will index camera positions; p will index voxel positions, t is thread number

    mxArray *out_array = xoutput = mxCreateDoubleMatrix(nvoxels,1,mxREAL);
    double *output = mxGetPr(out_array);

    // Clear Value from Memory
    for (int i=0; i<nvoxels; i++) output[i]=0;

    int tpos = 0;
    double *d1l=new double[nlasers];
    double *d4l=new double[nx];

    /* Calculating all d1 */
    for(tpos = 0; tpos < nlasers; tpos++)
    {
        d1l[tpos] = distance2(geo_laser_cop,&laserpos[tpos*3]);
    }

    /* Calculating all d4 */
    for (x = 0; x < nx; x++)
    {
        d4l[x] = distance2(&cpos[x*3],geo_camera_cop);
    }

    int vox_chunk = nvoxels/num_threads;

    for (p = 0; p < nvoxels; p += vox_chunk) // for all voxels
    {
        /*Pack args object for pthreads */
        backproject_args *args = (backproject_args *)malloc(sizeof(backproject_args));
        args->d1l = d1l;
        args->laserpos = laserpos;
        args->cpos = cpos;
        args->d4l = d4l;
        args->sI = sI;
        args->shift = shift;
        args->tpp = tpp;
        args->nlasers = nlasers;
        args->nx = nx;
        args->nt = nt;
        args->nvoxels = nvoxels;
        args->output = output;
        args->intensity_corr = InCorr;
        args->vox_indx_start = p;
        args->voxels = voxels;
        args->lasernormal = lasernormal;
        args->cameranormal = cameranormal;

        if (t != num_threads - 1)
        {
            args->vox_indx_end = (p + vox_chunk);
        }
        else
        {
            args->vox_indx_end = nvoxels;
            p = nvoxels;
        }

        //printf("Creating thread number %d for voxels %d to %d\n", t, args->vox_indx_start, args->vox_indx_end);
        mexPrintf("Thread %d started, indices %d:%d\n", t, args->vox_indx_start, args->vox_indx_end);
        mexEvalString("pause(.001);"); // to dump string.
        printf("Thread %d started, indices %d:%d\n", t, args->vox_indx_start, args->vox_indx_end);

        pthread_create(&threads[t++], NULL, voxel_calc, (void *)args);
    }

    for (i = 0; i < t; i++)
    {
        //Wait for all threads to complete
        pthread_join(threads[i], NULL);
        mexPrintf("Thread %d Finished!\n", i);
        mexEvalString("pause(.001);"); // to dump string.
        printf("Thread %d Finished!\n", i);

    }

    free(threads);
    delete[] d1l;
    delete[] d4l;
    mexPrintf("Backproject done!\n");
    mexEvalString("pause(.001);"); // to dump string.
} // end mexFunction

/* voxel_calc :- pthread dummy function that takes a chunk of voxels and applies the
   backprojection algorithm to it.
 */
void *voxel_calc(void *input) {
    backproject_args *args = (backproject_args *)input;

    double *sI =  args->sI;                // data
    double *laserpos = args->laserpos;     // emitter positions (laser spots)
    double *voxels = args->voxels;         // voxels
    double *cpos = args->cpos;             // probe positions (camera spots)
    double tpp = args->tpp;                // deltat - distance traveled by a photon per time bin
    double shift = args->shift;            // t0 - distance corresponding to index 0
    double InCorr = args->intensity_corr;  // if InCorr == 1 physical corrections, ==0 otherwise
    double *cameranormal = args->cameranormal;
    double *lasernormal = args->lasernormal;

    /* Defining integers */
    int nlasers = args->nlasers;     // number of laser spot positions on wall
    int nx = args->nx;               // number of camera spot positions on wall
    int nt = args->nt;               // number of time bins (exclusive - final bin is nt-1, and first is 0)
    int nvoxels = args->nvoxels;     // number of voxels

    int x=0,p=0; //x will index camera positions; p will index voxel positions
    double *output = args->output;

    int tpos = 0;
    double* d1l = args->d1l;
    double* d4l = args->d4l;
    int total = args->vox_indx_end - args->vox_indx_start;

    for (p = args->vox_indx_start; p < args->vox_indx_end; p++)
    {
        double thesum = 0;
        double * voxel1 = &voxels[p*3];
        int pp = p - args->vox_indx_start;

        #ifdef VERBOSE
        if (total/10 != 0 && pp % (total / 10) == 0)
        {
            printf("Thread starting at %d %d percent done\n", args->vox_indx_start, (pp / (total / 10)) * 10);
        }
        #endif /* completion tracking */

        for (tpos = 0; tpos < nlasers; tpos++) // for all laser positions
        {
            double * laserpos1 = &laserpos[tpos*3];
            double * lasernorm1 = &lasernormal[tpos*3];

            double d1 = d1l[tpos];
            double d2 = distance2(laserpos1,voxel1);
            for (x = 0; x < nx; x++) // for all camera positions
            {
                double * cpos1 = &cpos[x*3];
                double * cameranorm1 = &cameranormal[x*3];
                double d3 = distance2(voxel1,cpos1);
                double d4 = d4l[x];
                double d = d1 + d2 + d3 + d4;
                double vlv[3];
                double vcv[3];

                vlv[0]= (voxel1[0]-laserpos1[0])/d2;
                vlv[1]= (voxel1[1]-laserpos1[1])/d2;
                vlv[2]= (voxel1[2]-laserpos1[2])/d2;
                double dotlv = vlv[0]*lasernorm1[0]+vlv[1]*lasernorm1[1]+vlv[2]*lasernorm1[2];

                vcv[0]= (voxel1[0]-cpos1[0])/d3;
                vcv[1]= (voxel1[1]-cpos1[1])/d3;
                vcv[2]= (voxel1[2]-cpos1[2])/d3;
                double dotcv = vcv[0]*cameranorm1[0]+vcv[1]*cameranorm1[1]+vcv[2]*cameranorm1[2];

                /* Do intensity correction? */
                double intensity_correction = d2*d2*d3*d3;

                int tindex = round((d-(shift))/tpp);
                int index = 0;

                /* Backprojection core */
                if ((tindex>=0) && (tindex<nt))
                {
                    index = tpos*nx*nt + tindex + x*nt;
                    thesum = thesum + sI[index] * intensity_correction;

                }

            }
        }
        output[p] = thesum;
    }
    free(input);
    pthread_exit(NULL);
}
