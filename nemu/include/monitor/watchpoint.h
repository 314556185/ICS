#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	uint32_t oldvalue;
	uint32_t newvalue;
	char* expression;
	/* TODO: Add more members if necessary */
} WP;

WP* new_wp();
int set_watchpoint(char *e);
void free_wp(WP *wp);
bool delete_watchpoint(int NO);
void list_watchpoint();

#endif
