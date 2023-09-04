#define _CRT_SECURE_NO_WARNINGS
#define deg2rad 0.017453292519943295769

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>

struct matrix {
	float* arr = NULL;
	int rows = 0;
	int cols = 0;
	int size = 0;
};

enum matrixFillType {
	matrixZeros = 0,
	matrixRandom
};

// Функции для матриц
int matrixMultiplyByNumb(matrix*, matrix*, float);
int matrixOutput(matrix*);
int matrixSum(matrix*, matrix*, matrix*);
int matrixTranspose(matrix*, matrix*);
matrix* matrixNew(int, int, matrixFillType);
void matrixRenewArray(matrix*, int, int, float*);
int matrixDelete(matrix*);
float matrixGE(matrix*, int, int);
int matrixSE(matrix*, int, int, float);
int matrixMultiply(matrix*, matrix*, matrix*);
float matrixDot(matrix*, matrix*);
int matrixCross(matrix*, matrix*, matrix*);
int matrixRotZ(matrix*, float);
int matrixRotY(matrix*, float);
int matrixRotX(matrix*, float);
float matrixVecLen(matrix*);
int matrixNormVec(matrix*, matrix*);
float matrxVecAngle(matrix*, matrix*);
float matrxVecAzimuth(matrix*, matrix*);
matrix* matrixEye(int);
int matrixRot3D(matrix*, float, float, float);

int main(void) {
	setlocale(LC_ALL, "Russian");
	srand(3);

	matrix
		* M1 = matrixNew(2, 3, matrixRandom),
		* M2 = matrixNew(2, 1, matrixZeros),
		* M3 = matrixNew(1, 3, matrixRandom),
		* M4 = matrixNew(3, 1, matrixRandom),
		* M5 = matrixNew(1, 1, matrixRandom);

	matrixOutput(M1);
	matrixMultiplyByNumb(M1, M2, 2);
	printf("Умножаю на 2");
	matrixOutput(M2);
	printf("Транспонирую");
	matrixTranspose(M1, M1);
	matrixOutput(M1);
	printf("Умножаю");
	matrixMultiply(M1, M2, M2);
	matrixOutput(M2);
	printf("Умножаю вектора");
	matrixOutput(M4);
	matrixOutput(M3);
	matrixCross(M4, M3, M5);
	matrixOutput(M5);
	printf("Нормирую");
	matrixNormVec(M5, M5);
	matrixOutput(M5);

	printf("Вращаю по x на 10 градусов");
	matrixRotX(M5, 10);
	matrixOutput(M5);
	printf("Вращаю по y на 15 градусов");
	matrixRotY(M5, 15);
	matrixOutput(M5);
	printf("Вращаю по z на 45 градусов");
	matrixRotZ(M5, 45);
	matrixOutput(M5);

	printf("Вращаю по трём осям");
	matrixRot3D(M5, 10, 15, 45);
	matrixOutput(M5);	

	printf("Придаю значения элементам");
	matrixSE(M4, 0, 0, 1);
	matrixSE(M4, 1, 0, 2);
	matrixSE(M4, 2, 0, 3);
	matrixOutput(M4);

	matrixDelete(M1);
	matrixDelete(M2);
	matrixDelete(M3);
	matrixDelete(M4);
	matrixDelete(M5);

	return 0;
}

int matrixMultiplyByNumb(matrix* input, matrix* output, float numb) {
	if (input->cols != output->cols || input->rows !=  output->rows) {
		matrixRenewArray(output, input->rows, input->cols, (float*) malloc( sizeof(float)*input->size ));
	}
	for (int i = 0; i < input->cols * input->rows; i++) {
		output->arr[i] = input->arr[i] * numb;
	}
	return 0;
}

int matrixOutput(matrix* matr) {
	printf("\n");
	for (int i = 0; i < matr->rows; i++) {
		for (int v = 0; v < matr->cols; v++) {
			printf("%3.3f \t", (matr->arr[(matr->cols) * i + v]) );
		}
		printf("\n");
	}
	return 0;
}

int matrixSum(matrix* input1, matrix* input2, matrix* output) {
	if (input1->cols != input2->cols || input1->rows != input2->rows) return 1;
	if (output->cols != input2->cols || output->rows != input2->rows) {
		float* tmp = (float*) malloc(sizeof(float) * input1->size);
		for (int i = 0; i < input1->size; i++) {
			tmp[i] = input1->arr[i] + input2->arr[i];
		}
		matrixRenewArray(output, input1->rows, input1->cols, tmp);
	}
	else {
		for (int i = 0; i < input1->cols * input1->rows; i++) {
			output->arr[i] = input1->arr[i] + input2->arr[i];
		}
	}
	return 0;
}

int matrixTranspose(matrix* input, matrix *output) {

	if (input != output && output->size == input->size) {
		output->rows = input->cols;
		output->cols = input->rows;
		for (int i = 0; i < input->rows; i++) {
			for (int v = 0; v < input->cols; v++) {
				output->arr[output->cols * v + i] = matrixGE(input, i, v);
			}
		}
	}
	else {
		float* tmp = (float*)malloc(sizeof(float) * input->size);
		for (int i = 0; i < input->rows; i++) {
			for (int v = 0; v < input->cols; v++) {
				tmp[input->rows * v + i] = matrixGE(input, i, v);
			}
		}
		matrixRenewArray(output, input->cols, input->rows, tmp);
	}
	return 0;
}

int matrixMultiply(matrix* input1, matrix* input2, matrix* output) {
	if (input1->cols != input2->rows) return 1;
	
	matrix* tmp = matrixNew(input1->rows, input2->cols, matrixZeros);

	int cols = input2->cols, rows = input1->rows, sumNumb = input1->cols;
	float sum;

	for (int r = 0; r < input1->rows; r++) {
		for (int c = 0; c < input2->cols; c++) {
			sum = 0;
			for (int i = 0; i < sumNumb; i++) {
				sum += matrixGE(input1, r, i) * matrixGE(input2, i, c);
			}
			matrixSE(tmp, r, c, sum);
		}	 
	}
	matrixRenewArray(output, tmp->rows, tmp->cols, tmp->arr);
	delete tmp;
	return 0;
}

matrix* matrixNew(int rows, int cols, matrixFillType type) {
	matrix *newMatrix = new matrix;
	float* array = (float*) malloc(sizeof(float) * rows * cols);

	if (type == matrixZeros)
		for (int i = 0; i < rows * cols; i++)
			array[i] = 0;
	if (type == matrixRandom)
		for (int i = 0; i < rows * cols; i++)
			array[i] = (float) (rand()%10);

	newMatrix->rows = rows;
	newMatrix->cols = cols;
	newMatrix->arr = array;
	newMatrix->size = rows * cols;
	return newMatrix;
}

void matrixRenewArray(matrix* input, int rows, int cols, float* arr) {
	if (input->arr != NULL)
		delete[](input->arr);
	input->arr = arr;
	input->rows = rows;
	input->cols = cols;
	input->size = rows * cols;
}

matrix* matrixEye(int size) {
	matrix* newMatrix = new matrix;
	float* array = new float[size * size];

	newMatrix->rows = size;
	newMatrix->cols = size;
	newMatrix->arr = array;
	newMatrix->size = size * size;

	for (int r = 0; r < size; r++)
		for (int c = 0; c < size; c++)
			matrixSE(newMatrix, r, c, (float)(r == c ? 1 : 0));

	return newMatrix;
}

int matrixDelete(matrix* input) {
	if (input->arr != NULL)
		delete[](input->arr);
	delete input;

	return 0;
}

float matrixGE(matrix* input, int row, int col) { // Get Element
	if (row > input->rows-1 || col > input->cols-1) return 0;
	return input->arr[(input->cols) * row + col];
}

int matrixSE(matrix* input, int row, int col, float val) { // Set Element
	if (row > input-> rows-1 || col > input-> cols-1) return 1;
	input->arr[(input->cols) * row + col] = val;
	return 0;
}

float matrixDot(matrix* input1, matrix* input2) {
	float result = 0;
	if (input1->size != 3 || input2->size != 3) return 1;

	for (int i = 0; i < 3; i++)
		result += input1->arr[i] * input2->arr[i];
	return result;
}

int matrixCross(matrix* input1, matrix* input2, matrix* output) {
	if (input1->size != 3 || input2->size != 3) return 1;
	float* tmp = new float[3];
	tmp[0] = input1->arr[1] * input2->arr[2] - input1->arr[2] * input2->arr[1];
	tmp[1] = -input1->arr[0] * input2->arr[2] + input1->arr[2] * input2->arr[0];
	tmp[2] = input1->arr[0] * input2->arr[1] - input1->arr[1] * input2->arr[0];
	matrixRenewArray(output, 3, 1, tmp);
	return 0;
}

int matrixRotZ(matrix* output, float angle) {
	float s, c;
	if (output->size != 9) 
		matrixRenewArray(output, 3, 3, new float[9]);
	angle *= (float)deg2rad;
	s = (float)sin(angle);
	c = (float)cos(angle);
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

int matrixRotY(matrix* output, float angle) {
	float s, c;
	if (output->size != 9)
		matrixRenewArray(output, 3, 3, new float[9]);
	angle *= (float)deg2rad;
	s = (float)sin(angle);
	c = (float)cos(angle);
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

int matrixRotX(matrix* output, float angle) {
	float s, c;
	if (output->size != 9)
		matrixRenewArray(output, 3, 3, new float[9]);
	angle *= (float)deg2rad;
	s = (float)sin(angle);
	c = (float)cos(angle);
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

int matrixRot3D(matrix* output, float angleX, float angleY, float angleZ) {
	if (output->cols * output->rows != 9)
		matrixRenewArray(output, 3, 3, new float[9]);
	matrix* eye = matrixEye(3);
	matrix* tmp = matrixNew(3, 3, matrixZeros);
	matrixRotX(tmp, angleX);
	matrixMultiply(eye, tmp, eye);
	matrixRotY(tmp, angleY);
	matrixMultiply(eye, tmp, eye);
	matrixRotZ(tmp, angleZ);
	matrixMultiply(eye, tmp, eye);
	for (int i = 0; i < 9; i++)
		output->arr[i] = eye->arr[i];
	matrixDelete(eye);
	matrixDelete(tmp);
	return 0;
}

float matrixVecLen(matrix* input) {
	if (input->rows * input->cols != 3) return 0;
	return (float)sqrt(pow(input->arr[0], 2) + pow(input->arr[1], 2) + pow(input->arr[2], 2));
}

int matrixNormVec(matrix* input, matrix* output) {
	float norm = (float)sqrt(pow(input->arr[0], 2) + pow(input->arr[1], 2) + pow(input->arr[2], 2));
	if (input->size != 3) return 1;
	if (output->size != 3)
		matrixRenewArray(output, 3, 1, new float[3]);

	matrixMultiplyByNumb(input, output, 1 / matrixVecLen(input));
	return 0;
}

float matrxVecAngle(matrix* input1, matrix* input2) {
	if (input1->rows * input1->cols != 3) return 0;
	if (input2->rows * input2->cols != 3) return 0;
	matrix* thirdVec = matrixNew(3, 1, matrixZeros);
	float angle;
	matrixCross(input1, input2, thirdVec);
	angle = (float)acos(matrixDot(input1, input2) / (matrixVecLen(input1) * matrixVecLen(input2)));
	matrixDelete(thirdVec);
	return angle;
}

float matrxVecAzimuth(matrix* input1, matrix* input2) {
	if (input1->rows * input1->cols != 3) return 0;
	if (input2->rows * input2->cols != 3) return 0;
	matrix* thirdVec = matrixNew(1, 3, matrixZeros);
	float angle;
	matrixCross(input1, input2, thirdVec);
	if(thirdVec->arr[2] != 0)
		angle = (float)acos(matrixDot(input1, input2) / (matrixVecLen(input1) * matrixVecLen(input2)))*(thirdVec->arr[2]>0?1:-1);
	else 
		angle = (float)acos(matrixDot(input1, input2) / (matrixVecLen(input1) * matrixVecLen(input2)));
	matrixDelete(thirdVec);
	return angle;
}


