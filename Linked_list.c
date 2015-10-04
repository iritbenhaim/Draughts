#include "Linked_list.h"
#include "Chess.h"
#include <stdlib.h>

extern int should_terminate;

linked_list new_list()
{
	linked_list l;
	l.first = malloc(sizeof(node));
	if (l.first == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return l;
	}
	l.first->next = NULL;
	l.first->data = NULL;
	l.last = l.first;
	l.len = 0;
	return l;
}



/*does not free the data!*/
void free_list(linked_list list)
{
	node* cur = list.first;
	node* prev = list.first;
	while (cur->next != NULL)
	{
		cur = cur->next;
		free(prev);
		prev = cur;
	}
	free(prev);
}

void list_add(linked_list* list, void* data)
{
	(*list).last->data = data;
	(*list).last->next = malloc(sizeof(node));
	if (NULL == (*list).last->next)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	(*list).last = (*list).last->next;
	((*list).last)->next = NULL;
	((*list).last)->data = NULL;
	(*list).len++;
}

/*removes a single node from the list if it contains specific data*/
void list_remove(linked_list* list, void* data)
{
	node* crnt = list->first;
	node* next = crnt->next;
	if (crnt->data == data) /*case node deleted is first*/
	{
		list->first = crnt->next;
		free(crnt->data);
		return;
	}

	for (int i = 0; i < list->len; i++)
	{
		if (next->data == data)
		{
			crnt->next = next->next; /*data found, removing node*/
			free(next);
			list->len = list->len - 1;
			return;
		}
		crnt = next; /*searching next node*/
		next = next->next;
	}
}