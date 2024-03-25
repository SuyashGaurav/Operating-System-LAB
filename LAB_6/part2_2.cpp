#include<iostream>
#include<vector>
#include<fstream>
#include <sstream>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<cmath>
using namespace std;

#define SEM_NAME "/semLock"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

class Pixel{
    public:
    int r, g, b;
};


//Transformation 1
void RGB_to_GrayScale(key_t key, int pid, int height, int width, vector<vector<Pixel>> &imageMatrix){
    if (pid > 0) return;
    int shmid = shmget(key, sizeof( Pixel) * (height) * width, 0666 | IPC_CREAT);
    Pixel *values;
    values = ( Pixel *)shmat(shmid, NULL, 0);

    sem_t *semL = sem_open(SEM_NAME, O_RDWR);
    //luminosity method
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++){
            Pixel p = values[i * width + j];
            int gray_factor = ((0.21 * p.r) + (0.72 * p.g) + (0.07 * p.b));
            values[i * width + j].r = gray_factor;
            values[i * width + j].g = gray_factor;
            values[i * width + j].b = gray_factor;
            imageMatrix[i][j] = values[i*width + j];
        }
    }
    sem_post(semL);
}

// Transformation 2
void grayscale_to_edgeDetection(key_t key, int pid, int height, int width, vector<vector<Pixel>> &imageMatrix){
    if (pid > 0) return;

    int shmid = shmget(key, sizeof( Pixel) * (height) * width, 0666 | IPC_CREAT);
     Pixel *values;
    values = ( Pixel *)shmat(shmid, NULL, 0);

    sem_t *semL = sem_open(SEM_NAME, O_RDWR);

    int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    sem_wait(semL);
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            int gx = 0, gy = 0;
            for (int x=-1; x<=1; x++) {
                for (int y=-1; y<=1; y++) {
                    gx += imageMatrix[i+x][j+y].r * sobel_x[x+1][y+1];
                    gy += imageMatrix[i+x][j+y].r * sobel_y[x+1][y+1];
                }
            }

            int resultant = sqrt(gx * gx + gy * gy);
            resultant = min(255, resultant);
            values[i * width + j].r = resultant;
            values[i * width + j].g = resultant;
            values[i * width + j].b = resultant;
        }
    }
    sem_post(semL);
}

int main(int argc, char *argv[]){
    if(argc != 3) cout << "Error\n Usage: time ./a.out <path-to-original-image> <path-to-transformed-image>\n", exit(1);

    int width, height, maxValue;

    char P_3[3];
    FILE *inputFile = fopen(argv[1], "r");

    fscanf(inputFile, "%s%d%d%d", P_3, &width, &height, &maxValue);

    key_t key = 0x1239;
    int shmid = shmget(key, sizeof( Pixel) * (height) * width, 0666 | IPC_CREAT);

    Pixel *values;
    values = ( Pixel *)shmat(shmid, NULL, 0);

    int red, green, blue;
    vector<vector<Pixel>> imageMatrix;

    for (int i = height-1; i >= 0; i--){
        vector<Pixel> row;
        for (int j=0; j<width; j++){  
             Pixel temp;
            fscanf(inputFile, "%d%d%d", &red, &green, &blue);
            temp.r = red;
            temp.g = green;
            temp.b = blue;
            values[(i * width) + j] = temp;
            row.push_back(temp);
        }
        imageMatrix.push_back(row);
    }
    fclose(inputFile);

    sem_t *binarySemaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, 1);

    RGB_to_GrayScale(key, fork(), height, width, imageMatrix);
    grayscale_to_edgeDetection(key, fork(), height, width, imageMatrix);

    wait(NULL);
    wait(NULL);
    
    FILE *outputFile = fopen(argv[2], "w");
    fprintf(outputFile, "%s\n%d %d\n%d\n", P_3, width, height, maxValue); 

    for (int i=height-1; i>=0; i--){
        for (int j=0; j<width; j++){  
             Pixel temp;
            temp = values[(i * width) + j];
            
            fprintf(outputFile, "%d ", temp.r);
            fprintf(outputFile, "%d ", temp.g);
            fprintf(outputFile, "%d ", temp.b);
        }
        fprintf(outputFile, "\n");
    }
    
    fclose(outputFile);

    // Detaching the shared memory.
    shmdt(values);
    shmctl(shmid,IPC_RMID,NULL);
    return 0;
}