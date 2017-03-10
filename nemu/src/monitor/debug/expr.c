#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256,EQ,NEQ,REG,NUM,HEX,ADD,SUB,MUL,DIV,LP,RP,LSHIFT,RSHIFT,EITHER,BOTH,BIG,SMALL,BE,SE,OR,AND,DEREF,MINUS,MOD,XOR,NOT,NEITHER
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
    {"-", SUB},
	{"\\*", MUL},
	{"/", DIV},
	{"\\(", LP},
	{"\\)", RP},
	{"0x[0-9a-fA-F]{1,31}", HEX},
    {"[0-9]{1,31}", NUM},
	{">>", RSHIFT},
	{"<<", LSHIFT},
	{"\\|\\|", EITHER},
	{"&&", BOTH},
	{">=", BE},
	{"<=", SE},
	{">", BIG},
	{"<", SMALL},
    {"\\|", OR},
	{"&", AND},
	{"\\$[A-Za-z]{1,7}", REG},
	{"!=", NEQ},
	{"\\*", DEREF},
	{"-", MINUS},
	{"%", MOD},
	{"\\^", XOR},
	{"~", NOT},
	{"!", NEITHER},
	{" +", NOTYPE},				// spaces
	{"\\+",ADD},				// plus
	{"==",EQ},					// equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i,j;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE :
						break ;
					case NUM :
								 nr_token++;
								 tokens[nr_token].type=NUM;
								 if(substr_len>32) {
									 printf("TOO Much Number!\n");
									 assert(0);
								 }
								 for(j=0;j<31&&j<substr_len;j++)
								 {
									tokens[nr_token].str[j]=substr_start[j];
								 }
								 tokens[nr_token].str[substr_len]='\0';
								 break;
					case HEX :
								 nr_token++;
								 tokens[nr_token].type=HEX;
								 if(substr_len>32) {
									 printf("TOO Much Number!\n");
									 assert(0);
								 }
								 for(j=0;j<31&&j+2<substr_len;j++) {
								tokens[nr_token].str[j]=substr_start[j+2];
								 }
								 tokens[nr_token].str[substr_len]='\0';
								 break;
					case REG :
								 nr_token++;
								 tokens[nr_token].type=REG;
								 if(substr_len>7)
									 assert(0);
								 for(j=0;j<31&&j+1<substr_len;j++) {
								tokens[nr_token].str[j]=substr_start[j+1];
								 }
								 tokens[nr_token].str[substr_len]='\0';
								 break;
					case ADD :
								 nr_token++;
								 tokens[nr_token].type=rules[i].token_type;
								 break;
					case SUB :
							     nr_token++;
								 tokens[nr_token].type=rules[i].token_type;
							     break;
				    case MUL :
							     nr_token++;
							     tokens[nr_token].type=MUL;
							     break;
				    case DIV :
							     nr_token++;
							     tokens[nr_token].type=DIV;
							     break;
					case LP :
							     nr_token++;
							     tokens[nr_token].type=LP;
							     break;
					case RP :
							     nr_token++;
							     tokens[nr_token].type=RP;
							     break;
					case RSHIFT :
							     nr_token++;
							     tokens[nr_token].type=RSHIFT;
							     break;
					case LSHIFT :
							     nr_token++;
							     tokens[nr_token].type=LSHIFT;
                                 break;
					case EITHER :
							     nr_token++;
							     tokens[nr_token].type=EITHER;
							     break;
					case BOTH :
							     nr_token++;
							     tokens[nr_token].type=BOTH;
							     break;
					case BE :
							     nr_token++;
							     tokens[nr_token].type=BE;
							     break;
			        case SE :
							     nr_token++;
							     tokens[nr_token].type=SE;
							     break;
					case BIG :
								 nr_token++;
								 tokens[nr_token].type=BIG;
								 break;
					case SMALL :
								 nr_token++;
								 tokens[nr_token].type=SMALL;
								 break;
					case EQ :
								 nr_token++;
								 tokens[nr_token].type=EQ;
								 break;
					case NEQ :
								 nr_token++;
								 tokens[nr_token].type=NEQ;
								 break;
					case OR :
								 nr_token++;
								 tokens[nr_token].type=OR;
								 break;
					case AND :
								 nr_token++;
								 tokens[nr_token].type=AND;
								 break; 
					case MOD :
								 nr_token++;
								 tokens[nr_token].type=MOD;
								 break;
					case XOR :
								 nr_token++;
								 tokens[nr_token].type=XOR;
								 break;
					case NOT :
								 nr_token++;
								 tokens[nr_token].type=NOT;
								 break;
					case NEITHER :
								 nr_token++;
								 tokens[nr_token].type=NEITHER;
								 break;
					default: panic("please implement me");
				}
				break;
			}
		}
		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;	
		}
	}
	return true; 
}

int check_parentheses(int p,int q) {
	int flag=1,i=p,fi=0;
	while(i<=q) {
		if(fi==0&&tokens[i].type!=NUM&&tokens[i].type!=HEX&&tokens[i].type!=LP&&tokens[i].type!=RP)
			flag=0;
		else if(fi<0)
			return -1;
		if(tokens[i].type==LP) fi++;
		else if(tokens[i].type==RP) fi--;
		i++;
	}
	if(fi!=0) return -1;
	else {
		if(flag==0) return 0;
		else if(flag==1) return 1;
	}
	assert(0);
}
uint32_t chtonum(int p) {
	int n=strlen(tokens[p].str),i=0,mo=10,sum=0;
	uint32_t res=0;
	if(tokens[p].type==NUM) {
		mo=10;
		for(;i<n;i++)
			res=res*mo+tokens[p].str[i]-'0';
	}
	else if(tokens[p].type==HEX) {
		mo=16;
		for(;i<n;i++) {
			if(tokens[p].str[i]>='a'&&tokens[p].str[i]<='f')
				sum=tokens[p].str[i]-'a'+10;
			else if(tokens[p].str[i]>='A'&&tokens[p].str[i]<='F')
				sum=tokens[p].str[i]-'A'+10;
			else sum=tokens[p].str[i]-'0';
			res=res*mo+sum;
		}
	}
	else if(tokens[p].type==REG) {
		if(strlen(tokens[p].str)==3) {
			for(i=0;i<8;i++) {
				if(strcmp(tokens[p].str,regsl[i])==0)
					return cpu.gpr[i]._32;
			}
			if(strcmp(tokens[p].str,"eip")==0)
				return cpu.eip;
		}
	}
	return res;
}

int op(int p,int q) {
	int fi=0,i,op=-1;
	int pos1=-1;    // last ==
	int pos2=-1;    // last ||
	int pos3=-1;    // last &&
	int pos4=-1;    // last == or !=
	int pos5=-1;    // last <=, >=,< or > 
	int pos6=-1;    // last <<or >>
	int pos7=-1;    // last + or -
	int pos8=-1;    // last / or *

	for(i=p;i<=q;i++)
	{
		if(tokens[i].type==LP) 
			fi++;
		else if(tokens[i].type==RP) 
			fi--;
		if(fi==0)
		{
			if(tokens[i].type==EQ)
				pos1=i;
			else if(tokens[i].type ==EITHER)
				pos2=i;
			else if(tokens[i].type ==BOTH)
                pos3=i;
            else if(tokens[i].type==EQ||tokens[i].type==NEQ)
				pos4=i;
			else if(tokens[i].type==SE||tokens[i].type==BE||tokens[i].type ==SMALL||tokens[i].type==BIG)
				pos5=i;
            else if(tokens[i].type==LSHIFT||tokens[i].type==RSHIFT)
				pos6=i;
			else if(tokens[i].type ==ADD||tokens[i].type==SUB)
				pos7=i;
			else if(tokens[i].type==MUL||tokens[i].type==DIV)
				pos8=i;
		}
		else if(fi<0)
			return -2;
    }
    if(pos1!=-1)   op = pos1 ;
	else if(pos2!=-1) op=pos2 ;
	else if(pos3!=-1) op=pos3 ;
    else if(pos4!=-1) op=pos4 ;
	else if(pos5!=-1) op=pos5 ;
	else if(pos6!=-1) op=pos6 ;
	else if(pos7!=-1) op=pos7 ;
	else if(pos8!=-1) op=pos8 ;
	return op;
}

uint32_t eval(int p,int q) {
	int opp;
	uint32_t end;
	if(p>q) {
		printf("Bad b Expression!\n");
	    return 0;
	}
	else if(p==q) {
		return chtonum(p);
	}
	else if(check_parentheses(p,q)==1) {
		return eval(p+1,q-1);
	}
	else if(check_parentheses(p,q)==0) {
		opp=op(p,q);
		if(opp==-2)
		{
			printf("bad a  expression\n");
			assert(0);
		}
		else if(opp==-1) {
			if(tokens[p].type==NOT)
			{
				sscanf(tokens[q].str,"%x",&end);
				return ~end;
			}
		    if(tokens[p].type==NEITHER)
			{
				sscanf(tokens[q].str,"%x",&end);
				return !end;
			}
			if(tokens[p].type==MINUS)
			{
				sscanf(tokens[q].str,"%x",&end);
				return -end;
			}
			if(tokens[p].type==DEREF)
			{
				sscanf(tokens[q].str,"%x",&end);
				end=swaddr_read(end,4);
				return end;
			}
		}
		uint32_t val1=eval(p,opp-1);
		uint32_t val2=eval(opp+1,q);
		switch(tokens[opp].type) {
			case ADD : return val1+val2;
			case SUB : return val1-val2;
			case MUL : return val1*val2;
			case DIV : return val1/val2;
			case RSHIFT :  return val1>>val2;
			case LSHIFT :  return val1<<val2;
			case OR : return val1|val2;
			case AND : return val1&val2;
			case EITHER : return val1||val2;
			case BOTH : return val1&&val2;
			case BE : return val1>=val2;
			case SE : return val1<=val2;
			case BIG : return val1>val2;
			case SMALL : return val1<val2;
			case EQ : return val1==val2;
			case NEQ : return val1!=val2;
		    case MOD : return val1%val2;
			case XOR : return val1^val2;
		    default : assert(0);
		}
	}
	assert(0);
}

uint32_t expr(char *e, bool *success) {
	int i;
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	for(i=0;i<nr_token;i++) {
		if(tokens[i].type==SUB&&(i==0||tokens[i-1].type==ADD||tokens[i-1].type==SUB||tokens[i-1].type==MUL||tokens[i-1].type==DIV||tokens[i-1].type==LP))
			tokens[i].type=MINUS;
	}
	for(i=0;i<nr_token;i++) {
		if(tokens[i].type==MUL&&(i==0||(tokens[i-1].type!=RP&&tokens[i-1].type!=HEX&&tokens[i-1].type!=NUM)))
			tokens[i].type=DEREF;
	}
    return eval(1,nr_token);
	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	//return 0;
}

