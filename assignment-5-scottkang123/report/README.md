In your own words, briefly explain the purpose of the experiment and the experimental setup. Be sure to clearly state on which machine you ran the experiments, and exactly what your command line arguments were, so that we can reproduce your work in case of any confusion.

The purpose of this experiment is to take a program that can take a long time to execute and use multi-threading to parallelize the expensive computation. Mandelbrot sets can be computationally expensive to generate. The task allows us to have an experience with concurrency and reduce the execution time by using threads to divide the work and running it concurrently. I used linux virtual machine through Ubuntu within my MacBook Pro M1 pro computer. The command line argument that I used to generate the mandel.bmp is 
./mandel -x 0.309127925 -y 0.0302901274 -s 0.0001 -m 3000 


For the following two configurations, measure and graph the execution time of your multithreaded mandel using 1, 2, 3, 4, 5, 10, 50, and 100 threads. The execution time of these experiments may be upset by other things going on in the machine. So, repeat each measurement five times, and use the average time achieved.

A: mandel -x -.5 -y .5 -s 1 -m 2000
B: mandel -x 0.2869325 -y 0.0142905 -s .000001 -W 1024 -H 1024 -m 1000
To measure the execution times, use time command. Make sure that you run all your experiments in the virtual machine with more than one core, e.g., 3 or 4 cores.

THE GRAPH AND THE TABLE FOR EXECUTION TIME OF DIFFERENT NUMBER OF THREADS ARE IN A SEPARATE PDF FILE

Explain the shape of the two curves. What is the minimum number of threads that each of the two configurations requires to generate an image such that adding more threads does not sustantially reduce the execution time? Why do curves A and B have a different shape?

