#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);
extern int w_num;
/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args) {
	char *arg = strtok(NULL, " ");
	int t ;
	if(arg == NULL)
	{
		cpu_exec(1);
		return 0;
	}
	else {
		t = atoi(arg);
		if(t <= 0)		{
			if(t == -1)
			{
				cpu_exec(-1);
				return 0 ;
			}
			else
			{
			printf("Unknown command!\n");
			return 0;
			}
		}
		else 
		{
			cpu_exec(t);
			return 0 ;
		}
	}
}

static int cmd_info(char *args) {
	char *arg = strtok(NULL, " ");
	if(arg==NULL) {
		printf("enter error!\n");
		return 0;
	}
	if(strcmp(arg,"r") == 0)
	{
		printf("eax:0x%8x\necx: 0x%8x\nedx: 0x%8x\nebx: 0x%8x\nesp: 0x%8x\nebp: 0x%8x\nesi: 0x%8x\nedi: 0x%8x\n",
				cpu.eax,cpu.ecx,cpu.edx,cpu.ebx,cpu.esp,cpu.ebp,cpu.esi,cpu.edi);	
		printf("eip: 0x%8x\n",cpu.eip);
	}
	if(strcmp(arg,"w") == 0)
		list_watchpoint();
	return 0;
}

static int cmd_x(char *args) {
	char *arg1 = strtok(NULL, " ");
	int n = atoi(arg1);
	char *arg2 = strtok(NULL, " ");
	uint32_t p ;
	sscanf(arg2,"%x" ,&p);
	int i,j;
	for(i=1 ; i<=n ; i++)
	{
		printf("%x: ",p);
		for(j=1;j<=4;j++,p++)
		{
			printf("%02x\t",swaddr_read(p,1));
		}
		printf("\n");
	}
	return 0;
}
static int cmd_p(char *args) {
	if(args==NULL) {
		printf("Nothing in the expression!\n");
		assert(0);
	}
	bool *suc;
	uint32_t res=0;
	suc=malloc(sizeof(bool));
	*suc=true;
	res=expr(args,suc);
	printf("0x%x (%u)\n",res,res);
	return 0;
}
static int cmd_w(char *args) {
	if(args==NULL) {
		printf("Nothing in the expression!\n");
		assert(0);
	}
	if(set_watchpoint(args)==1)
		return 0;
	else {
		printf("set the watchpoint error!\n");
		return 0;
	}
}

static int cmd_d(char *args) {
	int i,num;
	if(strcmp(args,"all")==0) {
		for(i=1;i<w_num;i++)
			delete_watchpoint(i);
		return 0;
	}
	else {
	sscanf(args,"%d",&num);
	if(delete_watchpoint(num)==1) {
		printf("Delete the NO.%d watchpoint success!\n",num);
		return 0 ;
	}
	else {
		printf("Delete the NO.%d watchpoint error!\n",num);
		return 0;
	}
	}
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Single implement", cmd_si},
	{ "info", "Type the information of register", cmd_info},
	{ "x", "Scanning the memory", cmd_x},
	{ "p", "Expression evaluation", cmd_p},
	{ "w", "Set the watchpoint", cmd_w},
	{ "d", "Delete the watchpoint", cmd_d},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
