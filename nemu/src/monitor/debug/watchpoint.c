#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32
int w_num=0;
static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

WP* new_wp() {
	WP *p=free_;
	if(free_==NULL) {
		printf("There is no wp'space!\n");
		assert(0);
	}
	if(head==NULL) {
		head=(WP*)malloc(sizeof(WP));
		head->NO=-1;
		head->next=NULL;
		p=head;
	}
	else if(head->next==NULL) {
		p=head;
	}
	else {
		p=head;
		while(p->next!=NULL)
			p=p->next;
	}
	if(free_->next==NULL) {
		p->next=free_;
		free_=NULL;
	}
	else {
		p->next=free_;
		free_=free_->next;
	}
	p=p->next;
	p->next=NULL;
	return p;
}

void free_wp(WP *wp) {
	WP *p=head;
	if(head==wp)
	{
		head=head->next;
		wp->next=free_;
		free_=wp;
	}
	else {
		while(p->next!=wp) {
			p=p->next;
		}
		p->next=wp->next;
		wp->next=free_;
		free_=wp;
	}
}

void list_watchpoint() {
	WP* p=head;
	if(head==NULL||head->next==NULL)
		printf("There is no watchpoint!\n");
	else {
		printf("******There is the information of watchpoint******\n");
		while(p!=NULL) {
			printf("No.%d  expression:%s  oldvalue:%x  newvalue:%x\n",p->NO,p->expression,p->oldvalue,p->newvalue);
			p=p->next;
		}
	}
}

int set_watchpoint(char *e) {
	WP* p=new_wp();
	bool *suc;
	suc=malloc(sizeof(bool));
	*suc=true;
	p->expression=(char*)malloc(sizeof(strlen(e)+1));
	strcpy(p->expression,e);
	p->oldvalue=p->newvalue=expr(e,suc);
	p->NO=w_num++;
	return 1;
}

bool delete_watchpoint(int NO) {
	WP* p=head;
	WP* q;
	if(NO<=w_num) {
		while(NO) {
			p=p->next;
			NO--;
		}
		q=p->next;
		while(q->next!=NULL) 
			q->NO--;
		free_wp(p);
		return 1;
	}
		else 
			return 0;
}
/* TODO: Implement the functionality of watchpoint */


