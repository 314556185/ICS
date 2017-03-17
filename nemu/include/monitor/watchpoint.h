#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	char *str;
	int re;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */


} WP;

#endif
