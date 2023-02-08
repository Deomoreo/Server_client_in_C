#include <stddef.h> // required for NULL
#include <stdio.h>
#include <stdlib.h>
typedef struct list_node
{
   struct list_node *next; //8 byte because pointer
   
} list_node;

typedef struct string_item
{
    struct list_node node;  //8 byte
    const char *string;     //8 byte because pointer
}string_item;

//DOUBLE LINKED LIST ELEMENTS 
typedef struct list_double_node
{
    struct list_double_node *prev;
    struct list_double_node *next;
    void* value;
}list_double_node;

typedef struct string_item_double
{
    struct list_double_node node;  //8 byte
    const char *string;     //8 byte because pointer
}string_item_double;

void invert_LinkedList(list_node** head)
{
    if (!head)
        return;

    list_node* curr = *head;
    list_node* prev = NULL;
    list_node* next = NULL;

    while (curr)
    {
        //next node is current node's next 
        next = curr->next;
        // current node's next is now set to previous, first iteration becomes null
        curr->next = prev;
        //cycle forward: current becomes previous, next becomes current
        prev = curr;
        curr = next;
    }
    
    *head = prev;
    
    return;
}

list_node* list_get_tail(list_node** head)
{
    list_node* current_node = *head; //this means first node of the linkedlist when written like this
    list_node* last_node = NULL;
    while (current_node)
    {
        last_node = current_node;
        current_node = current_node->next;
    }
    return last_node;
}
int get_count(list_double_node** head)
{
    list_double_node* current_node = (list_double_node*)*head;
    //printf("GETTING COUNT: head IS %p \n",current_node); //this means first node of the linkedlist when written like this
    int i=0;

    list_double_node* last_node= NULL;
    while (current_node)
    {
       
        last_node = current_node;
        //printf("malefitz %p\n",last_node->value);

       // printf("NEXT NODE POINTER %p\n",last_node->next);
        current_node = current_node->next;
        //printf("CURRENT NODE POINTER %p\n",last_node);
        i++;
    }
    return i;
}
list_node* list_append(struct list_node** head,struct list_node* item)
{
    list_node* tail = list_get_tail(head);
    if (!tail) //if linkedlist is empty then the item becomes the head
    {
        *head = item;
    }
    else //else the item gets appended after the tail
    {
        tail->next = item;
    }
    item->next = NULL; //next node must be null
    return item;
}

list_node* remove_node(struct list_node** head,struct list_node* item_to_replace)
{
    
    list_node* current_node = *head;
    list_node* next_node= current_node->next;
    while(current_node->next)
    {
        
        if(next_node == item_to_replace)
        {
            list_node* node_after= next_node->next;
            next_node->next = NULL;
            current_node->next = node_after;
            free(next_node);
            return current_node;
        }
        current_node = next_node;
        next_node = current_node->next;
    }
    return current_node;
    
}
list_node* list_pop( list_node** head)
{
    list_node* current_node = *head;
    if (!current_node)
    {
        return NULL;
    }
    *head = (*head)->next;
    current_node->next = NULL;
    return current_node;
}

list_double_node* doublelist_get_tail(list_double_node** head);
list_double_node* doublelist_get_tail(list_double_node** head)
{
    list_double_node* current_node =*head;
    //printf("GETTING TAIL: head IS %p \n",current_node); //this means first node of the linkedlist when written like this

    list_double_node* last_node= NULL;
    while (current_node)
    {
        last_node = current_node;
        current_node = current_node->next;
    }
    return last_node;
}

struct list_double_node* doublelist_append(list_double_node** head, list_double_node *item)
{
        
    list_double_node* first = doublelist_get_tail((list_double_node**)head);
    // printf("append: first   IS %p \n",first);
    // printf("append: head    IS %p \n",((*head))); 
     
    list_double_node *tail = NULL ;
    
    if(!first)
    {
    //    puts("CALLOCING");
       *head = item;
       //printf("allocated %p\n",item);
       return item;
    }
    else
    {
       //printf("HEAD EXISTS head is %p\n",*head);
        
       tail = doublelist_get_tail(head);
    }
    if (!tail)
    {
        *head = item;
    }
    else
    {
        tail->next = item;
    }
    item->prev = tail;
    item->next = NULL;
    return item;
}
void PrintList(string_item* my_linked_list)
{
    string_item* string = my_linked_list;
    while (string)
    {
        //printf("%s\n", string->string);
        string = (struct string_item *)string->node.next;
    }
}