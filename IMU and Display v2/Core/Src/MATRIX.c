#define deg2rad 0.017453292519943295769

#include "MATRIX.h"

#include <math.h>
#include <stdlib.h>

static void matrixRenewArray(struct matrix*, uint8_t, uint8_t, float*);

uint8_t matrixMultiplyByNumb(struct matrix* input, struct matrix* output, float numb) {	
	for (uint8_t i = 0; i < input->size; i++) {
		output->arr[i] = input->arr[i] * numb;
	}
	
	if(output != input){
		output->cols = input->cols;
		output->rows = input->rows;
		output->size = input->size;
	}
	
	return 0;
}

uint8_t matrixSum(struct matrix* input1, struct matrix* input2, struct matrix* output) {
	if (input1->cols != input2->cols || input1->rows != input2->rows) return 1;
	if (output->size < input2->size) return 1;

	for (uint8_t i = 0; i < input1->size; i++) {
		output->arr[i] = input1->arr[i] + input2->arr[i];
	}
	
		output->cols = input1->cols;
		output->rows = input1->rows;
		output->size = input1->size;	
	
	return 0;
}

uint8_t matrixTranspose(struct matrix* input, struct matrix *output) {

	if (input != output && output->size == input->size) {
		output->rows = input->cols;
		output->cols = input->rows;
		for (uint8_t i = 0; i < input->rows; i++) {
			for (uint8_t v = 0; v < input->cols; v++) {
				output->arr[output->cols * v + i] = matrixGE(input, i, v);
			}
		}
	}
	else {
		float tmp[9];
		for (uint8_t i = 0; i < input->rows; i++) {
			for (uint8_t v = 0; v < input->cols; v++) {
				tmp[input->rows * v + i] = matrixGE(input, i, v);
			}
		}
		matrixRenewArray(output, input->cols, input->rows, tmp);
	}
	return 0;
}

uint8_t matrixMultiply(struct matrix* input1, struct matrix* input2, struct matrix* output) {
	if (input1->cols != input2->rows) return 1;
	
	struct matrix tmp;
	matrixZeroes(&tmp, input1->rows, input2->cols);
	float sum;

	for (uint8_t r = 0; r < input1->rows; r++) {
		for (uint8_t c = 0; c < input2->cols; c++) {
			sum = 0;
			for (uint8_t i = 0; i < input1->cols; i++) {
				sum += matrixGE(input1, r, i) * matrixGE(input2, i, c);
			}
			matrixSE(&tmp, r, c, sum);
		}	 
	}
	matrixRenewArray(output, tmp.rows, tmp.cols, tmp.arr);
	return 0;
}

uint8_t matrixZeroes(struct matrix* newMatrix, uint8_t rows, uint8_t cols) {
	if(rows > 3 || cols > 3) return 1;

	for (uint8_t i = 0; i < rows * cols; i++)
		newMatrix -> arr[i] = 0;

	newMatrix->rows = rows;
	newMatrix->cols = cols;
	newMatrix->size = rows * cols;
	
	return 0;
}

void matrixRenewArray(struct matrix* input, uint8_t rows, uint8_t cols, float* array) {
	input->rows = rows;
	input->cols = cols;
	input->size = rows * cols;
	for (uint8_t i = 0; i < input->size; i++)
		input->arr[i] = array[i];
}

uint8_t matrixEye(struct matrix* newMatrix, uint8_t size) {
	if(size > 3) return 1;

	newMatrix->rows = size;
	newMatrix->cols = size;
	newMatrix->size = size * size;

	for (uint8_t r = 0; r < size; r++)
		for (uint8_t c = 0; c < size; c++)
			matrixSE(newMatrix, r, c, (float)(r == c ? 1 : 0));

	return 0;
}

float matrixGE(struct matrix* input, uint8_t row, uint8_t col) { // Get Element
	if (row > input->rows-1 || col > input->cols-1) return 0;
	return input->arr[(input->cols) * row + col];
}

uint8_t matrixSE(struct matrix* input, uint8_t row, uint8_t col, float val) { // Set Element
	if (row > input->rows-1 || col > input->cols-1) return 1;
	input->arr[(input->cols) * row + col] = val;
	return 0;
}

float matrixDot(struct matrix* input1, struct matrix* input2) {
	float result = 0;
	if (input1->size != 3 || input2->size != 3) return 1;

	for (uint8_t i = 0; i < 3; i++)
		result += input1->arr[i] * input2->arr[i];
	return result;
}

uint8_t matrixCross(struct matrix* input1, struct matrix* input2, struct matrix* output) {
	if (input1->size != 3 || input2->size != 3) return 1;
	float tmp[3];
	tmp[0] = input1->arr[1] * input2->arr[2] - input1->arr[2] * input2->arr[1];
	tmp[1] = -input1->arr[0] * input2->arr[2] + input1->arr[2] * input2->arr[0];
	tmp[2] = input1->arr[0] * input2->arr[1] - input1->arr[1] * input2->arr[0];
	matrixRenewArray(output, 3, 1, tmp);
	return 0;
}

uint8_t matrixRotZ(struct matrix* output, float angle) {
	float s, c;
	angle *= deg2rad;
	s = sin(angle);
	c = cos(angle);
	output->arr[0] = c;
	output->arr[1] = -s;
	output->arr[2] = 0;
	output->arr[3] = s;
	output->arr[4] = c;
	output->arr[5] = 0;
	output->arr[6] = 0;
	output->arr[7] = 0;
	output->arr[8] = 1;
	//f = [cos(a) - sin(a) 0; sin(a) cos(a) 0; 0 0 1];
	return 0;
}

uint8_t matrixRotY(struct matrix* output, float angle) {
	float s, c;
	angle *= deg2rad;
	s = sin(angle);
	c = cos(angle);
	output->arr[0] = c;
	output->arr[1] = 0;
	output->arr[2] = s;
	output->arr[3] = 0;
	output->arr[4] = 1;
	output->arr[5] = 0;
	output->arr[6] = -s;
	output->arr[7] = 0;
	output->arr[8] = c;
	//f = [cos(a) 0 sin(a); 0 1 0; -sin(a) 0 cos(a)];
	return 0;
}

uint8_t matrixRotX(struct matrix* output, float angle) {
	float s, c;
	angle *= deg2rad;
	s = sin(angle);
	c = cos(angle);
	output->arr[0] = 1;
	output->arr[1] = 0;
	output->arr[2] = 0;	
	output->arr[3] = 0;
	output->arr[4] = c;
	output->arr[5] = -s;
	output->arr[6] = 0;
	output->arr[7] = s;
	output->arr[8] = c;
	//f=[1 0 0; 0 cos(a) -sin(a); 0 sin(a) cos(a)];
	return 0;
}

uint8_t matrixRot3D(struct matrix* output, float angleX, float angleY, float angleZ) {
	if (output->size != 9) return 1;
	struct matrix eye, tmp;
	matrixEye(&eye, 3);
	matrixZeroes(&tmp, 3, 3);
	matrixRotX(&tmp, angleX);
	matrixMultiply(&eye, &tmp, &eye);
	matrixRotY(&tmp, angleY);
	matrixMultiply(&eye, &tmp, &eye);
	matrixRotZ(&tmp, angleZ);
	matrixMultiply(&eye, &tmp, &eye);
	for (uint8_t i = 0; i < 9; i++)
		output->arr[i] = eye.arr[i];
	return 0;
}

uint8_t matrixRotArbVec(struct matrix* output, struct matrix* rotVec, float angle){
		if (output->size != 9 || rotVec->size != 3) return 1;
		float c, s, lx, ly, lz;
		c=cos(angle);
		s=sin(angle);
		lx=rotVec->arr[0]; ly=rotVec->arr[1]; lz=rotVec->arr[2];
		matrixSE(output,	0,	0, c+(1-c)*lx*lx		);
		matrixSE(output,	0,	1, (1-c)*lx*ly-s*lz	);
		matrixSE(output,	0,	2, (1-c)*lz*lx+s*ly	);
		matrixSE(output,	1,	0, (1-c)*lx*ly+s*lz	);
		matrixSE(output,	1,	1, c+(1-c)*ly*ly		);
		matrixSE(output,	1,	2, (1-c)*lz*ly-s*lx	);
		matrixSE(output,	2,	0, (1-c)*lx*lz-s*ly	);
		matrixSE(output,	2,	1, (1-c)*ly*lz+s*lx	);
		matrixSE(output,	2,	2, c+(1-c)*lz*lx		);
		
		return 0;
}

float matrixVecLen(struct matrix* input) {
	if (input->rows * input->cols != 3) return 0;
	return (float)sqrt(pow(input->arr[0], 2) + pow(input->arr[1], 2) + pow(input->arr[2], 2));
}

uint8_t matrixNormVec(struct matrix* input, struct matrix* output) {
	float norm = (float)sqrt(pow(input->arr[0], 2) + pow(input->arr[1], 2) + pow(input->arr[2], 2));
	if (input->size != 3) return 1;
	if (output->size != 3)
		matrixRenewArray(output, 3, 1, (float*) malloc( sizeof(float)*9 ));

	matrixMultiplyByNumb(input, output, 1 / matrixVecLen(input));
	return 0;
}

float matrixVecAngle(struct matrix* input1, struct matrix* input2) {
	if (input1->rows * input1->cols != 3) return 0;
	if (input2->rows * input2->cols != 3) return 0;
	struct matrix thirdVec;
	matrixZeroes(&thirdVec, 3, 1);
	float angle;
	matrixCross(input1, input2, &thirdVec);
	angle = (float)acos(matrixDot(input1, input2) / (matrixVecLen(input1) * matrixVecLen(input2)));
	return angle;
}

float matrixVecAzimuth(struct matrix* input1, struct matrix* input2) {
	if (input1->rows * input1->cols != 3) return 0;
	if (input2->rows * input2->cols != 3) return 0;
	struct matrix thirdVec;
	matrixZeroes(&thirdVec, 3, 1);	
	float angle;
	matrixCross(input1, input2, &thirdVec);
	if(thirdVec.arr[2] != 0)
		angle = (float)acos(matrixDot(input1, input2) / (matrixVecLen(input1) * matrixVecLen(input2)))*(thirdVec.arr[2]>0?1:-1);
	else 
		angle = (float)acos(matrixDot(input1, input2) / (matrixVecLen(input1) * matrixVecLen(input2)));
	return angle;
}

#undef deg2rad
