#include "bitmap.h"
#include <pthread.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Arguments needed for each thread

struct thread_args
{
    struct bitmap* bm;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    int max;    
    int height_start;
    int height_finish;
};

int iteration_to_color(int i, int max);
int iterations_at_point(double x, double y, int max);
void *compute_image(void *args);

void show_help()
{
    printf("Use: mandel [options]\n");
    printf("Where options are:\n");
    printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
    printf("-x <coord>  X coordinate of image center point. (default=0)\n");
    printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
    printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
    printf("-W <pixels> Width of the image in pixels. (default=500)\n");
    printf("-H <pixels> Height of the image in pixels. (default=500)\n");
    printf("-o <file>   Set output file. (default=mandel.bmp)\n");
    printf("-n <num_thread> set the number of threads (default=1)\n");  // ASK FOR # of Threads
    printf("-h          Show this help text.\n");
    printf("\nSome examples are:\n");
    printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
    printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
    printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int main(int argc, char* argv[])
{
    signed char c;

    // These are the default configuration values used
    // if no command line arguments are given.

    const char* outfile = "mandel.bmp";
    double xcenter = 0;
    double ycenter = 0;
    double scale = 4;
    int image_width = 500;
    int image_height = 500;
    int max = 1000;
    int num_thread = 1;     // default number of thread

    // For each command line argument given,
    // override the appropriate configuration value.

    while ((c = getopt(argc, argv, "x:y:s:W:H:m:o:n:h")) != -1) {
        switch (c) {
        case 'x':
            xcenter = atof(optarg);
            break;
        case 'y':
            ycenter = atof(optarg);
            break;
        case 's':
            scale = atof(optarg);
            break;
        case 'W':
            image_width = atoi(optarg);
            break;
        case 'H':
            image_height = atoi(optarg);
            break;
        case 'm':
            max = atoi(optarg);
            break;
        case 'o':
            outfile = optarg;
            break;
        case 'n':                  // when n is called modify the number of threads
            num_thread = atoi(optarg);
            break;
        case 'h':
            show_help();
            exit(1);
            break;
        }
    }
    
    // Display the configuration of the image.
    printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s num_threads = %d \n", xcenter, ycenter, scale, max, outfile, num_thread);

    // Create a bitmap of the appropriate size.
    struct bitmap* bm = bitmap_create(image_width, image_height);

    // Fill it with a dark blue, for debugging
    bitmap_reset(bm, MAKE_RGBA(0, 0, 255, 0));
    
    pthread_t threads[num_thread]; //create an array of threads
    struct thread_args args[num_thread]; //create an array of struct arguments for threads
    
    //Calculate the height that each tthread is responsible for
    int height_step = image_height / num_thread;
    
    //If not evenly divisible by num_threads, then assign the leftover to the last thread
    int leftover = image_height % num_thread;      
   
    for(int i=0;i<num_thread;i++){
        //Modify the arguments for each thread
        args[i].bm = bm;
        args[i].xmin = xcenter - scale;
        args[i].xmax = xcenter + scale;
        args[i].ymin = ycenter - scale;
        args[i].ymax = ycenter + scale;       
        args[i].max = max;      
        args[i].height_start = i * height_step; 
        //Assign the leftover to the last thread (0 if evenly divisible)
        if(i == num_thread-1){
            args[i].height_finish = (i+1) * height_step + leftover;
        }else{
            args[i].height_finish = (i+1) * height_step;
        }
        pthread_create(&threads[i], NULL, &compute_image, &args[i]);
    }
          
    //wait for all the threads to finish 
    for(int i=0;i<num_thread;i++){
        if(pthread_join(threads[i],NULL)){
            printf("Error with pthread_join"); 
        }
    }

    // Save the image in the stated file.
    if (!bitmap_save(args -> bm, outfile)) {
        fprintf(stderr, "mandel: couldn't write to %s: %s\n", outfile, strerror(errno));
        return 1;
    }

    return 0;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void* compute_image(void *arg)
{
    struct thread_args *args = (struct thread_args*) arg;
    int i, j;

    int width = bitmap_width(args -> bm);
    int height = bitmap_height(args -> bm);

    int height_start = args -> height_start;
    int height_finish = args -> height_finish;
    
    // For every pixel in the image...

    for (j = height_start; j < height_finish; j++) {

        for (i = 0; i < width; i++) {

            // Determine the point in x,y space for that pixel.
            double x = args -> xmin + i * (args -> xmax - args -> xmin) / width;
            double y = args -> ymin + j * (args -> ymax - args -> ymin) / height;

            // Compute the iterations at that point.
            int iters = iterations_at_point(x, y, args -> max);

            // Set the pixel in the bitmap.
            bitmap_set(args -> bm, i, j, iters);
        }
    }
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point(double x, double y, int max)
{
    double x0 = x;
    double y0 = y;

    int iter = 0;

    while ((x * x + y * y <= 4) && iter < max) {

        double xt = x * x - y * y + x0;
        double yt = 2 * x * y + y0;

        x = xt;
        y = yt;

        iter++;
    }

    return iteration_to_color(iter, max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color(int i, int max)
{
    int gray = 255 * i / max;
    return MAKE_RGBA(gray, gray, gray, 0);
}
