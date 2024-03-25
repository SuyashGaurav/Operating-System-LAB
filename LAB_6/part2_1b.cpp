#include<iostream>
#include<vector>
#include<fstream>
#include <sstream>
#include<cmath>
#include<thread>
#include<semaphore.h>
using namespace std;

sem_t s;
sem_t T1_done;

class Pixel{
    public:
        int r, g, b;
};

//Transformation 1
void RGB_to_GrayScale(vector<vector<Pixel>> &imageMatrix){
    sem_wait(&s);
    //luminosity method
    for(int i=0; i<imageMatrix.size(); i++){
        for(int j=0; j<imageMatrix[i].size(); j++){
            Pixel p = imageMatrix[i][j];
            int gray_factor = ((0.21 * p.r) + (0.72 * p.g) + (0.07 * p.b));
            imageMatrix[i][j].r = gray_factor;
            imageMatrix[i][j].g = gray_factor;
            imageMatrix[i][j].b = gray_factor;
        }
    }
    sem_post(&T1_done);
    sem_post(&s);
}

// Transformation 2
void grayscale_to_edgeDetection(vector<vector<Pixel>> &imageMatrix){
    sem_wait(&T1_done);
    sem_wait(&s);
    vector<vector<Pixel>> tempMatrix = imageMatrix;

    int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int i=1; i<imageMatrix.size()-1; i++) {
        for (int j=1; j<imageMatrix[i].size()-1; j++) {
            int gx = 0, gy = 0;

            for (int x=-1; x<=1; x++) {
                for (int y=-1; y<=1; y++) {
                    gx += tempMatrix[i+x][j+y].r * sobel_x[x+1][y+1];
                    gy += tempMatrix[i+x][j+y].r * sobel_y[x+1][y+1];
                }
            }

            int resultant = sqrt(gx * gx + gy * gy);
            resultant = min(255, resultant);
            imageMatrix[i][j].r = resultant;
            imageMatrix[i][j].g = resultant;
            imageMatrix[i][j].b = resultant;
        }
    }
    sem_post(&s);
}

int main(int argc, char *argv[]){
    if(argc != 3) cout << "Error\n Usage: time ./a.out <path-to-original-image> <path-to-transformed-image>\n", exit(1);
    int width, height, maxValue;

    char P_3[3];
    FILE *inputFile = fopen(argv[1], "r");

    fscanf(inputFile, "%s%d%d%d", P_3, &width, &height, &maxValue);

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
            row.push_back(temp);
        }
        imageMatrix.push_back(row);
    }
    fclose(inputFile);

    //Initialize semaphore with value 1 (simple lock/unlock feature)
    sem_init(&s, 0, 1);
    //Initialize semaphore T1_done with value 0
    sem_init(&T1_done, 0, 0);

    thread T1(RGB_to_GrayScale, ref(imageMatrix));
    thread T2(grayscale_to_edgeDetection, ref(imageMatrix));

    T1.join();
    T2.join();

    FILE *outputFile = fopen(argv[2], "w");

    fprintf(outputFile, "%s\n%d %d\n%d\n", P_3, width, height, maxValue); 

    for (int i=0; i<height; i++){
        for (int j=0; j<width; j++){  
            Pixel temp;
            temp = imageMatrix[i][j];
            
            fprintf(outputFile, "%d ", temp.r);
            fprintf(outputFile, "%d ", temp.g);
            fprintf(outputFile, "%d ", temp.b);
        }
        fprintf(outputFile, "\n");
    }
    
    fclose(outputFile);
    return 0;
}