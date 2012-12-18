#include <cstdio>
#include "ArrayList.h"

int main(int argc, char* argv[]) {
	printf("This is gonna be harder than I thought :(");

	IList* list = new ArrayList();
	
	for (int i = 0; i < 10; ++i) {
		list->add(i);
	}
	printf("\n");

	for (int i = 0; i < 10; ++i) {
		printf("%d ", (*list)[i]);
	}
	printf("\n");

	for (int i = 0; i < 10; ++i) {
		(*list)[9-i] = i;
	}

	
	for (int i = 0; i < 10; ++i) {
		printf("%d ", (*list)[i]);
	}
	printf("\n");

	printf("Size: %d", list->getSize());

	int bla = (*list)[10];

	delete list;
}
