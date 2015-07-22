

typedef struct node node;
typedef struct linked_list linked_list;

/*linked list node*/
struct node
{
	void* data;
	node* next;
};

/*linked list*/
struct linked_list
{
	node* first;
	node* last;
	int len;
};

void free_list(linked_list list);
void list_add(linked_list* list, void* data);
linked_list new_list();