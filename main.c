// Solely so stupid VS Code can find "CLOCK_PROCESS_CPUTIME_ID"
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000000000 // 1 GB

char global_array[SIZE];
char *global_ptr;
size_t dont_optimize_away;

static double get_elapsed_seconds(struct timespec start, struct timespec end) {
	return (double)(end.tv_sec - start.tv_sec) + 1.0e-9 * (double)(end.tv_nsec - start.tv_nsec);
}

static void time_static_array(void) {
	static char static_arr[SIZE];

	struct timespec start;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (size_t i = 0; i < SIZE; i++) {
		static_arr[i] = i;
	}

	struct timespec end;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	for (size_t i = 0; i < SIZE; i++) {
		dont_optimize_away += static_arr[i];
	}

	printf("time_static_array took %.2f seconds\n", get_elapsed_seconds(start, end));
}

static void time_global_array(void) {
	struct timespec start;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (size_t i = 0; i < SIZE; i++) {
		global_array[i] = i;
	}

	struct timespec end;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	printf("time_global_array took %.2f seconds\n", get_elapsed_seconds(start, end));
}

static void time_local_malloc(void) {
	char *local_ptr = malloc(SIZE);

	struct timespec start;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (size_t i = 0; i < SIZE; i++) {
		local_ptr[i] = i;
	}

	struct timespec end;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	free(local_ptr);

	printf("time_local_malloc took %.2f seconds\n", get_elapsed_seconds(start, end));
}

static void time_global_malloc(void) {
	global_ptr = malloc(SIZE);

	struct timespec start;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for (size_t i = 0; i < SIZE; i++) {
		global_ptr[i] = i;
	}

	struct timespec end;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	free(global_ptr);

	printf("time_global_malloc took %.2f seconds\n", get_elapsed_seconds(start, end));
}

int main(void) {
	printf("Before pages have been mapped:\n");
	time_static_array();
	time_global_array();
	time_local_malloc();
	time_global_malloc();

	printf("\nAfter pages have been mapped:\n");
	for (size_t i = 0; i < 2; i++) {
		time_static_array();
		time_global_array();
		time_local_malloc();
		time_global_malloc();
		printf("\n");
	}
}
