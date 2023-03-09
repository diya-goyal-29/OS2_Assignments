/*
 ============================================================================
 Name        : Assgn1Src_cs20btech11014.c
 Author      : Diya Goyal
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

int *nearest_point_x;
int *nearest_point_y;
double *nearest_point_distance;
int count = 0;
int num_points_per_thread;
int num_threads, src_x, src_y, num_destination_points;
char src_point_x[100], src_point_y[100];
char **dest_point_x, **dest_point_y;
int  *dest_x, *dest_y;
int thread_number = 1;

int coordinate_str_to_int(char *str, char *token) {

	char *temp_x = strtok(str, token);
	return atoi(temp_x);

}

void *nearest_point_calculator(void *param) {
	double *distance;
	distance = (double*) malloc(sizeof(double) * num_points_per_thread);
	int index_t = 0, max_num = num_points_per_thread + count;
	for(int i = count; i < max_num; i++){
		double d = pow((pow((src_x - dest_x[i]), 2) + pow((src_y - dest_y[i]), 2)), 0.5);
		distance[index_t] = d;
		index_t++;
		count++;


	}

	double min_distance = distance[0];

	for(int i = 0; i < num_points_per_thread; i++) {

		if(min_distance > distance[i]) {
			min_distance = distance[i];
			index_t = i;
		}
	}
	nearest_point_distance[thread_number - 1] = min_distance;
	nearest_point_x[thread_number - 1] = dest_x[count + index_t];
	nearest_point_y[thread_number - 1] = dest_y[count + index_t];

	thread_number++;

	pthread_exit(0);
}

int main() {

	FILE *f;
	f = fopen("sampleInput.txt", "r");

	if(f == NULL)
	{
	  printf("Error!");
	  exit(1);
	}

	fscanf(f, "%d\n", &num_threads);
	fscanf(f, "%[^,],%s\n", src_point_x, src_point_y);
	fscanf(f, "%d\n", &num_destination_points);


	src_x = coordinate_str_to_int(src_point_x, "(");

	src_y = coordinate_str_to_int(src_point_y, ")");
	printf("\n");
	dest_point_x = (char**)malloc(sizeof(char) * num_destination_points);
	dest_point_y = (char**)malloc(sizeof(char) * num_destination_points);
	for (int j = 0; j < num_destination_points; j++) {
		dest_point_x[j] = (char*) malloc(sizeof(char) * 10);
		dest_point_y[j] = (char*) malloc(sizeof(char) * 10);
	}

	dest_x = (int*)malloc(sizeof(int) * num_destination_points);
	dest_y = (int*)malloc(sizeof(int) * num_destination_points);

	for(int i = 0; i < num_destination_points; i++) {
		fscanf(f, "%[^,],%s ", dest_point_x[i],dest_point_y[i]);
		dest_x[i] = coordinate_str_to_int(dest_point_x[i], "(");
		dest_y[i] = coordinate_str_to_int(dest_point_y[i], ")");

	}

	fclose(f);


	nearest_point_x = (int*) malloc(sizeof(int) * num_threads);
	nearest_point_y = (int*) malloc(sizeof(int) * num_threads);
	nearest_point_distance = (double*) malloc(sizeof(double) * num_threads);

	num_points_per_thread = num_destination_points / num_threads;

	pthread_t tid[num_threads];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int i = 0;


	clock_t clock_start = clock();

	while(i < num_threads){

		pthread_create(&tid[i], &attr, nearest_point_calculator, NULL);

		i++;
	}

	i = 0;

	while(i < num_threads) {
		pthread_join(tid[i],NULL);
		i++;
	}
	clock_t clock_end = clock();
	float time_to_search = (float)(clock_end-clock_start)/CLOCKS_PER_SEC;

	int minimum_x_coord, minimum_y_coord;
	double minimum_distance;
	minimum_distance = nearest_point_distance[0];
	for(int j = 0; j < num_threads; j++) {
		if (minimum_distance >= nearest_point_distance[j]) {
			minimum_distance = nearest_point_distance[j];
			minimum_x_coord = nearest_point_x[j];
			minimum_y_coord = nearest_point_y[j];
		}
	}

	printf("The shortest distance from the source point (%d,%d) is %lf.\n", src_x, src_y, minimum_distance);
	printf("The time taken by the CPU is : %f\n", time_to_search);



	return EXIT_SUCCESS;
}
