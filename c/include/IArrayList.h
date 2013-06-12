#ifndef _C_KWAZ_ARRAY_LIST_
#define _C_KWAZ_ARRAY_LIST_

/* Description:
 * 	This is just a plain old list backed by a normal array.
 *
 * 	This allows for good random reads but will take up more memory than
 * 	a linked list.
 * 	
 * 	One difference between this ArrayList and other implementations is
 * 	that this implementation will resize the backing array if enough
 * 	remove operations happen.
 * 
 *
 * Configuration value explination:
 * 
 * Capacity (default 10):
 * 	The ArrayList's capacity determines how many elements the list can contain
 * 	before resisizing the backing array.
 * 	
 * 	NOTE: While this changes as you use the list, the backing array shall never
 * 	shrink below the original capacity used to create the list.
 * 
 * Add Reallocation Multiplier (default 2):
 * 	The multiplier used to expand an ArrayList's backing array whenever the
 * 	list's size equals its capacity.
 *
 * 	EX: A multiplier of 2 doubles the capacity.
 * 
 * Remove Reallocation Threshold (default 4):
 * 	The threshold value is multiplied by an ArrayList's size during
 * 	every remove operation.
 * 	If the resulting value is <= the ArrayList's capacity,
 * 	a reallocation (shrink) of the backing array is performed.
 *
 * 	EX: A threshold of 4 requires <= 25% of an ArrayList's capacity
 * 	to be used before triggering a reallocation during removal.
 *
 * 	NOTE: This value must never be below the remove reallocation divisor.
 * 	If this is true, the constructor will override the provided value with
 * 	double the remove reallocation divisor.
 *
 * Remove Reallocation Divisor (default 2):
 * 	The divisor used to shrink an ArrayList's backing array whenever the
 * 	list's size * reallocation threshold is greater than the list's capacity.
 *
 * 	EX: A divisor of 2 halfs the capacity.
 */

#include <stdlib.h>
#include <string.h>
#include "IList.h"

/* The internal ArrayList structure.
 * Defined in the *.c file
 */
struct _ArrayList;

/* The public interface structure.
 */
typedef struct ArrayList
{
	/* ICollection Methods */
	int (* const getSize)(const struct ArrayList* pList);
	void (* const destroy)(struct ArrayList* pList);
	void (* const add)(struct ArrayList* pList, const int pValue);

	/* IList Methods */
	int (* const get)(const struct ArrayList* pList, const size_t pIndex);
	int (* const remove)(struct ArrayList* pList, const size_t pIndex);

	/* ArrayList Methods */
	int (* const getCapacity)(const struct ArrayList* pList);

	IList* superType;
	struct _ArrayList* impl;

} ArrayList;

/* Start extern for c++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Constructors */
ArrayList* ArrayList_createDefault();
ArrayList* ArrayList_create(
	const size_t pCapacity,
	const unsigned int pAddReallocationMultiplier,
	const unsigned int pRemoveReallocationThreshold,
	const unsigned int pRemoveReallocationDivisor
);

/* Close c++ extern */
#ifdef __cplusplus
}
#endif

#endif
