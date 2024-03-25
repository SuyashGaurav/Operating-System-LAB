#include<iostream>
#include<vector>
#include<fstream>
#include <sstream>
#include<cmath>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
using namespace std;

class Pixel{
    public:
        int r, g, b;
};

//Transformation 1
void RGB_to_GrayScale(vector<vector<Pixel>> &imageMatrix, int fd[2], int height, int width){
    close(fd[0]); // Close unused read end
    //luminosity method
    for(int i=0; i<imageMatrix.size(); i++){
        for(int j=0; j<imageMatrix[i].size(); j++){
            Pixel p = imageMatrix[i][j];
            int gray_factor = ((0.21 * p.r) + (0.72 * p.g) + (0.07 * p.b));
            imageMatrix[i][j].r = gray_factor;
            imageMatrix[i][j].g = gray_factor;
            imageMatrix[i][j].b = gray_factor;
            write(fd[1], &imageMatrix[i][j], sizeof(Pixel));
        }
    }
    close(fd[1]); // Close write end
}

// Transformation 2
void grayscale_to_edgeDetection(int fd[2], vector<vector<Pixel>> &imageMatrix, int height, int width){
    close(fd[1]); // Close unused write end
    vector<vector<Pixel>> transformedImage(height, vector<Pixel>(width));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Pixel p;
            read(fd[0], &p, sizeof(Pixel));
            transformedImage[i][j].r = p.r;
            transformedImage[i][j].g = p.g;
            transformedImage[i][j].b = p.b;
        }
    }

    vector<vector<Pixel>> tempMatrix = transformedImage;
    int rows = imageMatrix.size();
    int cols = imageMatrix[0].size();

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
            transformedImage[i][j].r = resultant;
            transformedImage[i][j].g = resultant;
            transformedImage[i][j].b = resultant;
        }
    }
    imageMatrix = transformedImage;
    close(fd[0]); // Close read end
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

    // Creating pipes
    int fd[2];
    if (pipe(fd) == -1) {
        perror("Pipe failed");
        exit(1);
    }

    // Forking
    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) { // Child process for Transformation 1
        RGB_to_GrayScale(imageMatrix, fd, height, width);
        exit(0);
    } else { // Parent process for Transformation 2
        grayscale_to_edgeDetection(fd, imageMatrix, height, width);
        wait(NULL);

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
    }

    return 0;
}
