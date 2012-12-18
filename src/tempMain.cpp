#include <cstdio>
#include "ArrayList.h"

int main(int argc, char* argv[]) {
	printf("This is gonna be harder than I thought :(");

	IList* list = new ArrayList(0);
	
	for (int i = 0; i < 100; ++i) {
		list->add(i);
	}
	printf("\n");

	for (int i = 0; i < 100; ++i) {
		printf("%d ", (*list)[i]);
	}
	printf("\n");

	for (int i = 0; i < 100; ++i) {
		(*list)[99-i] = i;
	}

	
	for (int i = 0; i < 100; ++i) {
		printf("%d ", (*list)[i]);
	}
	printf("\n");

	printf("Size: %d", list->getSize());

	for (int i = 0; i < 100; ++i) {
		list->remove(0);
	}

	printf("Size after removal: %d", list->getSize());

	for (int i = 0; i < 100; ++i) {
		list->add(i);
	}
	printf("\n");

	int bla = (*list)[101];

	delete list;
}
