#ifndef _LIST_H
#define _LIST_H

#include "main.h"

#define LIST_MAX_NUM                    1000
#define LIST_U32_MIN_CNT                0

struct xLIST_ITEM
{
	volatile u16  xItemValue;                /*used for circulation*/
	volatile u32  value;                     /*used for sorting*/
	struct xLIST_ITEM* volatile pxNext;		/*< Pointer to the next ListItem_t in the list. */
	struct xLIST_ITEM* volatile pxPrevious;	/*< Pointer to the previous ListItem_t in the list. */
	void* pvOwner;										/*< Pointer to the object that contains the list item. */
	void* volatile pvContainer;				/*< Pointer to the list in which this list item is placed (if any). */
};

typedef struct xLIST_ITEM ListItem_t;

struct xMINI_LIST_ITEM
{
	volatile u16 xItemValue;
	volatile u32 value;
	struct xLIST_ITEM* volatile pxNext;
	struct xLIST_ITEM* volatile pxPrevious;
};

typedef struct xMINI_LIST_ITEM MiniListItem_t;

typedef struct xLIST
{
	u16 uxNumberOfItems;
	ListItem_t* volatile pxIndex;			/*< Used to walk through the list.  */
	MiniListItem_t xListEnd;			   /*<  used as a marker. */
}List_t;

extern List_t  list_unload;
extern List_t  list_load;


void vListInitialise(List_t* const pxList);
void vListInitialiseItem(ListItem_t* pxItem);
void vListInsert(List_t* const pxList, ListItem_t* const pxNewListItem); // sort form small to big  according  to  xItemvalue;
void vListInsertsort(List_t* const pxList, ListItem_t* const pxNewListItem); //sort from big to small according to value;
void vListInsertEnd(List_t* const pxList, ListItem_t* const pxNewListItem);
void uxListRemove(ListItem_t* pxItemToRemove);
#endif