#include <cstdio>
#include "ArrayList.h"

int main(int argc, char* argv[]) {
	printf("This is gonna be harder than I thought :(");

	ArrayList list = ArrayList();

	for (int i = 0; i < 10; ++i) {
		list[i] = i;
	}
	printf("\n");

	for (int i = 0; i < 10; ++i) {
		printf("%d ", list[i]);
	}
	printf("\n");

	int bla = list[10];
}
