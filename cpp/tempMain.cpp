#include <cstdio>
#include "ArrayList.h"

void printArrayList(ArrayList& list) {
	printf("size: %d\n", list.getSize());
	printf("capacity: %d\n", list.getCapacity());
	for (size_t i = 0; i < list.getSize(); ++i) {
		printf("%d ", list[i]);
	}
	printf("\n");
}


int main(int argc, char* argv[]) {
	// Even in C++ I gotta please the compiler by voiding unused args
	// </shocked...notreally>
	(void)argc; (void)argv;

	IList* list = new ArrayList();
	
	for (int i = 0; i < 100; ++i) {
		list->add(i);
		ArrayList* tmp = (ArrayList*)list;
		printArrayList(*tmp);
	}
	printf("\n");
	IList* other = new ArrayList(*list);
	for (int i = 0; i < 100; ++i) {
		list->remove(0);
		ArrayList* tmp = (ArrayList*)list;
		printArrayList(*tmp);
	}

	for (int i = 0; i < 100; ++i) {
		other->remove(0);
		ArrayList* tmp = (ArrayList*)other;
		printArrayList(*tmp);
	}

	delete list;
	delete other;
}
