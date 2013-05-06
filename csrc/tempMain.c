#include <stdio.h>
#include "ArrayList.h"

void printArrayList(ArrayList* list) {
	printf("size: %d\n", list->size);
	printf("capacity: %d\n", list->capacity);
	int i;
	for (i = 0; i < list->size; ++i) {
		printf("%d ", list->values[i]);
	}
	printf("\n");
}

int main(int argc, char* argv[]) {
	ArrayList* list = ArrayList_createDefault();
	int i = 0;
	for (i = 0; i < 100; ++i) {
		ArrayList_add(list, i);
		printArrayList(list);
	}
	printf("\n");
	ArrayList* other = ArrayList_createCopy(list);
	for (i = 0; i < 100; ++i) {
		ArrayList_remove(list, 0);
		printArrayList(list);
	}

	for (i = 0; i < 100; ++i) {
		ArrayList_remove(other, 0);
		printArrayList(other);
	}

	ArrayList_destroy(list);
	ArrayList_destroy(other);
}
