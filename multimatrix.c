#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#define MAX 50

//struct to store thread and matrix info
struct Matrices {
	int id;
    int** a;
    int** b;
    int row;
    int col;
    int** result;
};


int** generateMatrix(int zero, int dx, int dy) {
    int i, j; 
    int **matrix = (int **)malloc(dx * sizeof(int*));
    for(int i = 0; i < dx; i++) matrix[i] = (int *)malloc(dy * sizeof(int));
    for (i = 0; i < dx; i++) {
        matrix[i] = malloc(dy * sizeof(int));
        for(i = 0; i < dx; i++){
		    for(j = 0; j < dy; j++){
		        if(zero == 1){
			        matrix[i][j] = (rand() % MAX) + 1;
		        }
		        else{
		            matrix[i][j] = 0;
		        }
		    }
	    }
        
    }
    
    return matrix;
}


void displayMatrix(int** matrix, int dx, int dy) {
    printf("Matrix values:");
    int i, j;

    for (i = 0; i < dx; i++) {
        printf("\n");
    
        for (j = 0; j < dy; j++) {
            printf("%i, ", matrix[i][j]);
        }
    }

    printf("\n\n");
}

//single thraded multiplication
void multiply_normal(int** a, int** b, int** result, int adx, int ady, int bdx, int bdy) {
    int i,j,k;
	if(adx != bdy){
		printf("Can not operate the multiplication.\n");
	}
	else{
		for(i = 0; i < adx; i++){
		     int sum = 0;
			for(j = 0; j < bdy; j++){
				for(k=0; k < bdx; k++){
					sum += a[i][k] * b[k][j];
				}
				result[i][j]=sum;
				sum = 0;
			}
		}	
	}
}


// Thread function 
 void* threadwork(void* args) {
	struct Matrices* thread_data = (struct Matrices*) args;
	int id = (*thread_data).id;
	int row =(*thread_data).row;
	int col= (*thread_data).col;
	int i;
	int sum = 0;

	for (i = 0; i < row; i++) {
		int j;

		for (j = 0; j < col; j++) {
			sum += (*thread_data).a[id][j] * (*thread_data).b[j][i];
		}

		(*thread_data).result[id][i] = sum;
		sum = 0;
	}
} 

// Multithreaded multiplication 

void multithread(int** a, int** b, int** result, int adx,int ady,int bdx,int bdy) {
    pthread_t* threads;
    threads = malloc(adx * sizeof(pthread_t));
    struct Matrices thread_data[adx];
    int i;
    if(ady!=bdx){
        printf("Can not oparete the multiplication.");
    }
    else{
        for (i = 0; i < adx; i++) {
    		thread_data[i].id = i;
        	thread_data[i].a = a;
        	thread_data[i].b = b;
        	thread_data[i].result = result;
        	thread_data[i].row = adx;
        	thread_data[i].col = bdx;
        	
        	pthread_create(&threads[i], NULL, threadwork, &thread_data[i]);
        }
    
        for (i = 0; i < adx; i++) {
        	pthread_join(threads[i], NULL);
        }
    }
}

int main() {
    srand(time(NULL));  //seed-rand number
    int adx, ady, bdx, bdy;  
    printf("Enter rows and cols for matrix A:\n");
    scanf("%d",&adx);
    scanf("%d",&ady);
    printf("Enter rows and cols for matrix B:\n");
    scanf("%d",&bdx);
    scanf("%d",&bdy);
    int** matrixA = generateMatrix(1,adx,ady); 
    int** matrixB = generateMatrix(1,bdx,bdy);
    int** result_matrix = generateMatrix(0,adx,bdy); //creates [adx][bdy] sized matrix which contains zeros
    
    displayMatrix(matrixA, adx, ady);
    displayMatrix(matrixB, bdx, bdy);
    
    // Single-threaded result for multiplication
    clock_t single_begin = clock();
    multiply_normal(matrixA, matrixB, result_matrix, adx, ady, bdx, bdy);
    printf("Single threaded multiplication\n");
   
    displayMatrix(result_matrix,adx,bdy);
    clock_t single_end = clock();
    double single_time = (double)(single_end - single_begin) / CLOCKS_PER_SEC;
    printf("\nTotal time passed: %fs \n", single_time);
    printf("------------------------------\n");
    // Multi-threaded result for multiplication
    result_matrix = generateMatrix(0,adx,bdy);
    clock_t multi_begin = clock();
    multithread(matrixA, matrixB, result_matrix,adx, ady, bdx, bdy);
    printf("\nMulti threaded multiplication\n");
    printf("------------------------------\n");
    displayMatrix(result_matrix, adx,bdy);
    clock_t multi_end = clock();
    double multi_time = (double)(multi_end - multi_begin) / CLOCKS_PER_SEC;
    printf("\nTotal time passed: %fs \n", multi_time);
    
    return 0;
}
