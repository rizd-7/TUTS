#include<stdio.h>
#include <stdlib.h>
#include<pthread.h>

#define NUMBER_THREAD 1
#define INPUT_FILE_NAME "largeImageP3.ppm"
#define OUTPUT_FILE_NAME "output.ppm"


typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    int ThreadID;
    int width;
    int height;
    Pixel *pixels;
} ThreadData;

// Function to convert RGB pixel to grayscale
void to_grayscale(Pixel *pixel) {
    unsigned char gray = (pixel->r + pixel->g + pixel->b) / 3;
    pixel->r = pixel->g = pixel->b = gray;
}

void* applyGrayOnImage(void *arg){
    FILE *outputFile;
    ThreadData *data = (ThreadData*) arg;

    //EXTRACT THREAD DATA
    int ThreadID = data->ThreadID;
    int width = data->width;
    int height = data->height;
    Pixel *pixels = data->pixels;

    int PIXEL_NUMBER = width * height;
    int start = ThreadID * (PIXEL_NUMBER/NUMBER_THREAD);
    int end = (ThreadID == NUMBER_THREAD - 1) ? PIXEL_NUMBER : start + (PIXEL_NUMBER / NUMBER_THREAD);

    // APLY MODIFICATION TO PIXELS
    for (int i = start; i < end; i++) {
        to_grayscale(&pixels[i]);
    }

    pthread_exit(NULL);
}

int main(){

    FILE *inputFile,*outputFile;

    inputFile = fopen(INPUT_FILE_NAME,"r");
    outputFile = fopen(OUTPUT_FILE_NAME,"w");

    if (!inputFile || !outputFile) {
        perror("Error opening files.");
        return EXIT_FAILURE;
    }

     // Read PPM header
    char format[3];
    int width, height, maxColor;

    fscanf(inputFile, "%s", format);
    fscanf(inputFile, "%d %d", &width, &height);
    fscanf(inputFile, "%d", &maxColor);

    printf("===========HEADER============= \n");
    printf("%s \n",format);
    printf("%d X %d \n",width,height);
    printf("%d \n",maxColor);
    printf("==============================\n");

    if (format[0] != 'P' || format[1] != '3') {
        fprintf(stderr, "Unsupported PPM format.\n");
        return EXIT_FAILURE;
    }

    // Write the PPM header to the output file
    fprintf(outputFile, "P3\n%d %d\n%d\n", width, height, maxColor);

    Pixel *pixels = malloc(width * height * sizeof(Pixel));
    if (!pixels) {
        perror("Error allocating memory.");
        return EXIT_FAILURE;
    }

    //MAIN FILE READS INPUT FILE IN PIXELS ALLOCATED SPACE
    for (int i = 0; i < width * height; i++) {
        fscanf(inputFile, "%hhu %hhu %hhu", &pixels[i].r, &pixels[i].g, &pixels[i].b);
    }
    fclose(inputFile);

    // THREADS CREATION
    pthread_t threads[NUMBER_THREAD];
    ThreadData threadData[NUMBER_THREAD];
    for (int j = 0; j < NUMBER_THREAD; j++) {
        threadData[j].ThreadID = j;
        threadData[j].width = width;
        threadData[j].height = height;
        threadData[j].pixels = pixels;
        pthread_create(&threads[j], NULL, applyGrayOnImage, &threadData[j]);
    }


     // Read pixel data and apply grayscale filter
    /*for (int i = 0; i < width * height; i++) {
        fscanf(inputFile, "%hhu %hhu %hhu", &pixels[i].r, &pixels[i].g, &pixels[i].b);
        to_grayscale(&pixels[i]);
        fprintf(outputFile, "%d %d %d ", pixels[i].r, pixels[i].g, pixels[i].b);
    }
    */

     // Wait for all threads to finish
    for (int j = 0; j < NUMBER_THREAD; j++) {
        pthread_join(threads[j], NULL);
    }

    // Write the processed pixels to the output file
    for (int i = 0; i < width * height; i++) {
        fprintf(outputFile, "%d %d %d ", pixels[i].r, pixels[i].g, pixels[i].b);
        if ((i + 1) % width == 0) fprintf(outputFile, "\n");
    }


    // Free memory and close files
    free(pixels);
    fclose(outputFile);
    // pthread_exit(NULL);

    printf("Grayscale image processing complete. Output saved to output.ppm.\n");
    return 0;

}
