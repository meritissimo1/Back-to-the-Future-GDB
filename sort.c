#include <stdio.h>
#include <stdlib.h>
int bubble_sort(int *vec, int size) {
  for (int i = 0; i < size; i++) {
 	  for (int j = i; j < size-1; j++) {
     	if (vec[j] >= vec[j+1]) {
     	  int tmp = vec[j];
     	  vec[j] = vec[j+1];
     	  vec[j+1] = tmp;
     	}
 	  }
 	}
 }

 int sorted(int* vec, int size) {
 	for (int i = 0; i < size-1; i++) {
 	  if (vec[i] > vec[i+1])
     	return 0;
 	}
 	return 1;
 }

 int main() {
	int a[6] = {0, 3, 1, 5, 4, 2};
 	int b[6] = {5, 0, 3, 1, 2, 4};
 	int c[6] = {5, 4, 3, 2, 1, 0};
 	int d[6] = {3, 0, 4, 1, 2, 5};
 	int e[6] = {0, 4, 5, 1, 2, 3};
 	int** v = malloc (sizeof(int *) * 6);
 	int size = 6;
 	int fail = 0;
 	v[0] = a;
 	v[1] = b;
 	v[2] = c;
 	v[3] = d;
 	v[4] = e;
 	v[5] = NULL;

 	while (*v != NULL) {
 	  bubble_sort (*v, size);
 	  if (!sorted(*v, size))
     	fail++;
 	  v++;
 	}

 	if (fail)
 	printf ("Oh no! there were errors!\n");

 	return 0;
 }
