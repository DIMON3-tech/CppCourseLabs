#include <iostream>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <commands.h>
#include "bmp.h"


void gridSetup(uint64_t** grid, std::string wayToTsvFile) {
    std::ifstream fileTsv;

    fileTsv.open(wayToTsvFile);
    if (!fileTsv.is_open()) {
        std::cout << "Ошибка открытия файла" << std::endl;
        fileTsv.close();

    }
    while(!fileTsv.eof()) {
        fileTsv >> var.coordinateX;
        fileTsv >> var.coordinateY;
        fileTsv >> var.numberOfPiles;
        grid[var.coordinateX - 1][var.coordinateY - 1] += var.numberOfPiles;

    }
    fileTsv.close();

}

uint64_t** nextIteration(uint64_t** nextGrid, uint64_t** grid, int lengthOfArray, int widthOfArray) {

    bool leftBorder = false;
    bool rigthBorder = false;
    bool upBorder = false;
    bool downBorder = false;

    // use "help array" to fill next iteration
    for (int firstIndex = 0; firstIndex < lengthOfArray; firstIndex++) { // rewrite cells which contain less than 4 piles
        for (int secondIndex = 0; secondIndex < widthOfArray; secondIndex++) {
            uint64_t number = grid[firstIndex][secondIndex];
            if (number < 4) {
                nextGrid[firstIndex][secondIndex] = grid[firstIndex][secondIndex];

            }

        }
    }


    for (int firstIndex = 0; firstIndex < lengthOfArray; firstIndex++) { // bring down piles for surrounding cells
        for (int secondIndex = 0; secondIndex < widthOfArray; secondIndex++) {
            uint64_t number = grid[firstIndex][secondIndex];
            if (number >= 4) {
                if (firstIndex > 0) { upBorder = true; }
                if (firstIndex + 1 < lengthOfArray) { downBorder = true; }
                if (secondIndex + 1 < widthOfArray) { rigthBorder = true; }
                if (secondIndex > 0) { leftBorder = true; }
                nextGrid[firstIndex][secondIndex] = grid[firstIndex][secondIndex] - 4;

                if (downBorder) {
                    nextGrid[firstIndex + 1][secondIndex] += 1;

                }
                if (upBorder) {
                    nextGrid[firstIndex - 1][secondIndex] += 1;

                }

                if (rigthBorder) {
                    nextGrid[firstIndex][secondIndex + 1] += 1;

                }
                if (leftBorder) {
                    nextGrid[firstIndex][secondIndex - 1] += 1;

                }

            }

        }
    }



    for (int firstIndex = 0; firstIndex < lengthOfArray; firstIndex++) {
        for (int secondIndex = 0; secondIndex < widthOfArray; secondIndex++) {
            grid[firstIndex][secondIndex] = nextGrid[firstIndex][secondIndex];
            nextGrid[firstIndex][secondIndex] = 0;

        }

    }
    return grid;
}

int main(int argc, char* argv[]) {
    system("chcp 65001");
    for (int index = 1; index < argc; index++) {
        if ((argv[index][0] == '-' && argv[index][1] == 'l') ||
            (argv[index][0] == '-' && argv[index][1] == '-' && argv[index][2] == 'l')) {
            var.length = atoi(argv[index + 1]);
            //std::cout << var.length;
        }
        if ((argv[index][0] == '-' && argv[index][1] == 'w') ||
            (argv[index][0] == '-' && argv[index][1] == '-' && argv[index][2] == 'w')) {
            var.width = atoi(argv[index + 1]);
            //std::cout << variables.width;
        }
        if ((argv[index][0] == '-' && argv[index][1] == 'i') ||
            (argv[index][0] == '-' && argv[index][1] == '-' && argv[index][2] == 'i')) {
            var.wayToTsvFile = argv[index + 1];
            //std::cout << var.wayToTsvFile;
        }
        if ((argv[index][0] == '-' && argv[index][1] == 'o') ||
            (argv[index][0] == '-' && argv[index][1] == '-' && argv[index][2] == 'o')) {
            var.wayToSavePictures = argv[index + 1];
            //std::cout << variables.wayToSavePictures;
        }
        if ((argv[index][0] == '-' && argv[index][1] == 'm') ||
            (argv[index][0] == '-' && argv[index][1] == '-' && argv[index][2] == 'm')) {
            var.maxNumberOfIterations = atoi(argv[index + 1]);
            //std::cout << variables.maxNumberOfIterations;
        }
        if ((argv[index][0] == '-' && argv[index][1] == 'f') ||
            (argv[index][0] == '-' && argv[index][1] == '-' && argv[index][2] == 'f')) {
            var.frequency = atoi(argv[index + 1]);

        }

    }

    uint64_t** grid = new uint64_t* [var.length];
    for (int index = 0; index < var.length; index++) { // create array "array"
        grid[index] = new uint64_t[var.width];
    }
    for (int firstIndex = 0; firstIndex < var.length; firstIndex++) { // fill "array" by nulls
        for (int secondIndex = 0; secondIndex < var.width; secondIndex++) {
            grid[firstIndex][secondIndex] = 0;
        }

    }

    uint64_t** nextGrid = new uint64_t* [var.length];
    for (int index = 0; index < var.length; index++) { // create array "array"
        nextGrid[index] = new uint64_t[var.width];
    }
    for (int firstIndex = 0; firstIndex < var.length; firstIndex++) { // fill "array" by nulls
        for (int secondIndex = 0; secondIndex < var.width; secondIndex++) {
            nextGrid[firstIndex][secondIndex] = 0;
        }

    }

    gridSetup(grid, var.wayToTsvFile); // set sandpiles in grid

    while(var.new_iteration ){ //do collapse while there are cells with more than 3 piles
        for(int i = 0; i < var.length; i++){
            for(int j = 0; j < var.width; j++){
                std::cout << grid[i][j];
            }
        }
        std::cout << "\n";
        var.counterOfIterations++;
        var.counterFrequencyIterations++;
        nextIteration(nextGrid, grid, var.length, var.width);
        if(true) {
            std::string filename = static_cast<std::string>(var.wayToSavePictures) + "fileBMP" + std::to_string(var.counterOfIterations) + ".bmp";
            std::cout << filename << std::endl;
            //std::cout << "hello from if" << std::endl;
            std::ofstream fout(filename, std::ios::binary);
            bmp.sizeOfBitmapFile = bmp.kSizeOfBmpHeader + bmpInformation.sizeOfThisHeader + var.length * var.width * bmp.kSizeOFPixel;
            bmpInformation.width = var.width;
            bmpInformation.length = var.length;
            fout.write((char *) &bmp, bmp.kSizeOfBmpHeader /*14 */);
            fout.write((char *) &bmpInformation, bmpInformation.sizeOfThisHeader/* 40 */);
            for (int first_index = 0; first_index < var.length; first_index++) {
                for (int second_index = 0; second_index < var.width; second_index++) {
                    if (grid[first_index][second_index] == 0) {
                        fout.write((char *) &pixelWhite, 3);

                    }
                    if (grid[first_index][second_index] == 1) {
                        fout.write((char *) &pixelGreen, bmp.kSizeOFPixel);
                    }

                    if (grid[first_index][second_index] == 2) {
                        fout.write((char *) &pixelPurple, bmp.kSizeOFPixel);
                    }

                    if (grid[first_index][second_index] == 3) {
                        fout.write((char *) &pixelYellow, bmp.kSizeOFPixel);
                    }

                    if (grid[first_index][second_index] == 0) {
                        fout.write((char *) &pixelBlack, bmp.kSizeOFPixel);
                    }

                }
            }
            fout.close();
        }
        var.new_iteration = false;
        for (int first_index = 0; first_index < var.length; first_index++) {
            for (int second_index = 0; second_index < var.width; second_index++) {
                if (grid[first_index][second_index] >= 4) {
                    var.new_iteration = true; // we can make one more collapse
                }

            }

        }
        //std::cout << "new_iteration" << " " << var.new_iteration << std::endl;

    }
    std::cout << "grid after while" << std::endl;
    for(int i = 0; i < var.length; i++){
        for(int j = 0; j < var.width; j++){
            std::cout << grid[i][j];
        }
    }




}
// -i C:\\Users\\dimab\\Downloads\\book1.txt
// labwork_3_DIMON3_tech.exe -l 4 -w 4 -i C:\\Users\\dimab\\Downloads\\book1.txt -o D:\\laba 3 ++\\labwork-3-DIMON3-tech\\cmake-build-debug\\
// D:\laba 3 ++\labwork-3-DIMON3-tech\cmake-build-debug