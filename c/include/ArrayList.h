#ifndef _KWAZ_ARRAY_LIST_
#define _KWAZ_ARRAY_LIST_

/* Default configuration values */
#define ARRAY_LIST_INITIAL_CAPACITY 10

/* The multiplier used to expand an ArrayList's backing array whenever the
 * list's size equals its capacity.
 *
 * EX: A multiplier of 2 doubles the capacity.
 */
#define ARRAY_LIST_ADD_REALLOCATION_MULTIPLIER 2

/* The threshold value is multiplied by an ArrayList's size.
 * If the resulting value is <= the ArrayList's capacity,
 * a reallocation (shrink) of the backing array is performed.
 *
 * EX: A threshold of 4 requires <= 25% of an ArrayList's capacity
 * to be used before triggering a reallocation during removal.
 */
#define ARRAY_LIST_REMOVE_REALLOCATION_THRESHOLD 4

/* The divisor used to shrink an ArrayList's backing array whenever the
 * list's size * reallocation threshold is greater than the list's capacity.
 *
 * EX: A divisor of 2 halfs the capacity.
 */
#define ARRAY_LIST_REMOVE_REALLOCATION_DIVISOR 2

#include <stdlib.h>
#include <string.h>
#include "IList.h"

typedef struct {
	int* values;
	size_t size;
	size_t capacity;

	IList* superType;
} ArrayList;

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Constructors
 * Pass NULL if you do not intend to work with a list's supertype.
 * e.g: ArrayList_create(NULL):
 */
ICollection* ArrayList_ICollection_create();
IList* ArrayList_IList_create(ICollection* pCollection);
ArrayList* ArrayList_create(IList* pList);
ArrayList* ArrayList_createCopy(const ArrayList* pOther);

/* Destructors */
void ArrayList_ICollection_destroy(ICollection* pCollection);
void ArrayList_IList_destroy(IList* pList);
void ArrayList_destroy(ArrayList* pList);

/* External and Interface Methods */
int ArrayList_ICollection_getSize(const ICollection* pCollection);
int ArrayList_IList_getSize(const IList* pList);

void ArrayList_ICollection_add(ICollection* pCollection, const int pValue);
void ArrayList_IList_add(IList* pList, const int pValue);
void ArrayList_add(ArrayList* pList, const int pValue);

int ArrayList_IList_remove(IList* pList, const size_t pIndex);
int ArrayList_remove(ArrayList* pList, const size_t pIndex);

int ArrayList_IList_get(const IList* pList, const size_t pIndex);
int ArrayList_get(const ArrayList* pList, const size_t pIndex);

/* Internal methods */
int* ArrayList_allocateArray(
	const int* pOrigValues,
	const size_t pOrigValuesSize,
	const size_t pNewCapacity
);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
