
#ifndef __MATRIX_H
#define __MATRIX_H

#include "main.h"

struct matrix {
	float arr[9];
	uint8_t rows;
	uint8_t cols;
	uint8_t size;
};

uint8_t matrixMultiplyByNumb(struct matrix* input, struct matrix* output, float numb);
uint8_t matrixSum(struct matrix* input1, struct matrix* input2, struct matrix* output);
uint8_t matrixTranspose(struct matrix* input, struct matrix *output);
uint8_t matrixZeroes(struct matrix* newMatrix, uint8_t rows, uint8_t cols);
uint8_t matrixEye(struct matrix* newMatrix, uint8_t size);
float matrixGE(struct matrix* input, uint8_t row, uint8_t col);
uint8_t matrixSE(struct matrix* input, uint8_t row, uint8_t col, float val);
uint8_t matrixMultiply(struct matrix* input1, struct matrix* input2, struct matrix* output);
float matrixDot(struct matrix* input1, struct matrix* input2);
uint8_t matrixCross(struct matrix* input1, struct matrix* input2, struct matrix* output);
uint8_t matrixRotZ(struct matrix* output, float angle);
uint8_t matrixRotY(struct matrix* output, float angle);
uint8_t matrixRotX(struct matrix* output, float angle);
uint8_t matrixRot3D(struct matrix* output, float angleX, float angleY, float angleZ);
uint8_t matrixRotArbVec(struct matrix* output, struct matrix* rotVec, float angle);
float matrixVecLen(struct matrix* input);
uint8_t matrixNormVec(struct matrix* input, struct matrix* output);
float matrixVecAngle(struct matrix* input1, struct matrix* input2);
float matrixVecAzimuth(struct matrix* input1, struct matrix* input2);

#endif /* __MATRIX_H */
