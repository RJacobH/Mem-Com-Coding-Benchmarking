#include "MemComBenchmarking.h"

// int sendToGnu(unsigned long *A, int size) {
//     unsigned long x[size];
//     unsigned long y[size];
//     for (int i=0; i<size; i++) {
//         x[i] = A[2*i];
//         y[i] = A[2*i+1];
//     }

//     FILE *gnuplot = popen("gnuplot", "w");
//     if (!gnuplot) {
//         perror("popen");
//         exit(EXIT_FAILURE);
//     }

//     fprintf(gnuplot, "plot '-' u 1:2  t 'Price' w lp\n");
//     for (int i=0; i<size; i++) {
//         // printf("%lu, %lu,\n", x[i],y[i]);
//         fprintf(gnuplot, "%lu %lu\n", x[i], y[i]);
//     }
//     fprintf(gnuplot, "e\n");
//     fprintf(stdout, "Click Ctrl+d to quit...\n");
//     fflush(gnuplot);
//     getchar();

//     pclose(gnuplot);
//     exit(EXIT_SUCCESS);
//     return 0;
// }

int makeGraph(unsigned long *A, int size) {
    unsigned long x[size];
    unsigned long y[size];
    unsigned long maxX = 0;
    unsigned long maxY = 0;
    char * xLabel = "Buffer Size (KB)0";
    char * yLabel = "Latency per read (ns)0";
    double xLabLen = 16;
    double yLabLen = 22;

    float width = 100;
    float height = 40;
    for (int i=0; i<size; i++) {
        x[i] = A[2*i];
        if (x[i] > maxX) {
            maxX = x[i];
        }
        y[i] = A[2*i+1];
        if (y[i] > maxY) {
            maxY = y[i];
        }
    }
    
    double Xratio = width/maxX;
    double Yratio = height/maxY;
    char * graph = malloc (width * height);
    for (int i=0; i<height; i++) {
        for (int j=0; j<width; j++) {
            graph[(int)(i*width +j)] = ' ';
        }
    }

    for (int i=0; i<size; i++) {
        graph[(int) (floor(y[i]*Yratio)*width+floor(x[i]*Xratio))] = '*';
    }

    int o=0;
    for (int i=height; i>0; i--) {
        if(i<(height/2+yLabLen/2) && (yLabel[o]!='0')) {
            printf("%c", yLabel[o]);
            o++;
        } else {
            printf("|");//
        }
        for (int j=0; j<width; j++) {
            printf("%c",graph[(int)(i*width+j)]);
            if (j==width-1) {
                printf("\n");
            }
        }
    }

    printf("+");
    o=0;
    for (int j=0; j<width; j++) {
        if (j > width/2-xLabLen/2 && o < xLabLen) {
            while (xLabel[o] != '0') {
                printf("%c",xLabel[o]);
                o++;;
                j++;
            }
        } else {
            printf("-");
        }
    }
    printf("\n");
    return 0;
}


int main() {
    long maxBSKB = 1024; // max buffer size in KB
    long minBSKB = 16; // min buffer size in KB
    int numberOfSizes = 47; // how many different buffer sizes are there
    int sizeOfMany = 100000; // how many times the program iterates through each buffer size

    unsigned long maxBS = 1000*(unsigned long) maxBSKB; // max buffer size in bytes
    unsigned long minBS = 1000*(unsigned long) minBSKB; // max buffer size in bytes

    // unsigned long jump=((unsigned long)(maxBS-minBS)/(numberOfSizes-1)); // calculates the difference between sizes of N linearly
    double jump = pow((maxBS/minBS),(1/((double)numberOfSizes-1)));
    // printf("\njump = %f\n\n", jump);

    int blank; // initializes a variable which bytes are read to (avoiding print & hopefully reducing some cost per read)
    int size = sizeof(unsigned long);
    time_t t;
    srand((unsigned) time(&t)); // start random
    
    unsigned long * resultTable = calloc(2 * numberOfSizes, size); // make a table for results that can be updated with values of N and the mean times of access
    int maxMSize = 0;

    struct timespec start, stop;
    int i = 0;
    for (i = 0; i<numberOfSizes; i++) {
        unsigned long N = minBS * pow(jump,i); // calculates each buffer size exponentially
        // printf("%f\n",pow(jump,i));
        unsigned long * Array = malloc (N*size); // initializes the array of size N
        
        // printf("index = %i\n", index);
        unsigned long runningMean = 0;

        for (int j=sizeOfMany; j>=1; j--) { // calculates the mean time of access for some random indices some number of times per value of N

            int index = sizeOfMany * rand() % N; // initializes the index of the target byte
            
            // trying to avoid any unnecessary overhead, so i'm computing copy time this way

            clock_gettime(CLOCK_MONOTONIC, &start);
            blank = Array[index]; // read a byte from a pre-allocated buffer of size N at index & measure the latency
            clock_gettime(CLOCK_MONOTONIC, &stop);

            double start_time = start.tv_nsec;
            double stop_time = stop.tv_nsec;

            double total_time = stop_time - start_time;
            runningMean += (unsigned long) total_time;
        }
        
        int currentMean = runningMean/sizeOfMany;
        resultTable[2*i] = N; // N values are stored in even indices of the array
        resultTable[2*i+1] = currentMean; // mean copy times are stored in odd indices of the array
        // printf("currentMean = %d", currentMean);

        if (currentMean < 0) { // this little if statement is my attempt to deal with the processor being used for Other Things in the middle of the timing (or whatever causes the total_time to get so messed up sometimes)
            currentMean = 1;
            printf("\n crisis averted - (a mean time value was equal to some 16 digit number)\n");
            i--; // every time the currentMean seems to corrupt & make a 16 digit output instead of something more reasonable, this just writes over it/redoes it
        }
        if (floor(log10(currentMean)) + 1 > maxMSize - 2) { // this is a helpful bit for the table print out at the end
            maxMSize = (int) (floor(log10(currentMean)) + 3);
        }
    }

    // print it out (in two columns, the first of which is the length of the buffer (N), and the second column is the estimated mean time (in ns) to copy a single byte (not the entire buffer))
    // reminder: null character at the end of the buffer

    /* This section prints out the results in a nice looking table */
    int maxNSize = (int) floor(log10(minBS * pow(jump,(numberOfSizes - 1)))) + 1; // exponential increase between values of N
    maxNSize = fmax(maxNSize, 9); // hardcodes the size of "N (bytes)"
    maxMSize = fmax(maxMSize, 15); // hardcodes the size of "Mean Time (ns)"

    char bar[] = "––––––––––––––––––––––";
    printf("\n %*s | %s\n %.*s+%.*s\n", maxNSize, "N (bytes)", "Mean Time (ns)",    maxNSize*3+3, bar, maxMSize*3, bar);
    for (int i=0; i<numberOfSizes; i++) {
        printf(" %*lu | %lu - (i=%i)\n", maxNSize, resultTable[2*i], resultTable[2*i+1], i);
    }
    printf("\n");
    /* The data is sent to gnuplot to be plotted onto a nice-looking graph */
    // sendToGnu(resultTable, numberOfSizes);

    makeGraph(resultTable, numberOfSizes); // as gnuplot is complicated and I don't know how to deal with it yet, I made my own little graphing thing

    return 0;
}
