#include"net_header.h"
users *add_list(char *name,int input_fd)
{
	users *tmp;
	tmp=(users *)malloc(sizeof(users));
	strcpy(tmp->username,name);
	tmp->fd=input_fd;
	tmp->next=head;
	head=tmp;
//	return head;
}
void delete_all_list(void)
{
	users *next,*cur;
	cur=head;
	while(cur!=NULL)
	{
		next=cur->next;
		free(cur);
		cur=next;
	}
	head=NULL;
}
void delete_node(users *p)
{
	users *cur,*prev=NULL;
	cur=head;
	while(cur!=p&&cur!=NULL)
	{	
		prev=cur;
		cur=cur->next;
	}
	if(cur==NULL)
		return;
//	if(cur==head)
//	{
		head=cur->next;
		free(cur);
//	}
//	else
//	{
//		prev->next=cur->next;
//		free(cur);
//	}
	cur=NULL;
}
