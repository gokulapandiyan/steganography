#include<stdio.h>
#include<stdlib.h>
typedef struct node{
    struct node *prev;
    int data;
    struct node *next;
}Dlist;

void print_list(Dlist *head)
{
	/* Cheking the list is empty or not */
	if (head == NULL)
	{
		printf("INFO : List is empty\n");
	}
	else
	{
	    printf("Head -> ");
	    while (head)		
	    {
		    /* Printing the list */
		    printf("%d <-", head -> data);

		    /* Travering in forward direction */
		    head = head -> next;
		    if (head)
		        printf("> ");
	    }
    	printf(" Tail\n");
    }
}

int main(){
    Dlist *head=NULL;
    Dlist *tail=NULL;
    Dlist *new=calloc(1,sizeof(Dlist));
    new->data=5;
    new->prev=NULL;
    new->next=NULL;
    if(!head){
        printf("list is empty Adding elements\n");
        head=new;
        tail=new;
    }
    new->prev=tail;
    tail->next=new;
    tail=new;
    print_list(head);

}