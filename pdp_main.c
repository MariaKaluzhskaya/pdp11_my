#include "common.h"
/*#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned short int word;
typedef word adr;

byte mem[64*1024];
word reg[8];
int NZVC[4];
word nn, xx;
int reg_number_sob;
int b_or_w;

struct Argument {
	adr a;
	word val;
};
struct Argument ss, dd;
struct Command {
	word opcode;
	word mask;
	char* name;
	void(*func)();
	byte param;
} commands[] = {
	{0,       0177777, "halt", do_halt, NO_PARAM}, //0xFFF
	{0010000, 0170000, "mov",  do_mov, HAS_SS | HAS_DD},
	{0110000, 0170000, "movb", do_mov, HAS_SS | HAS_DD},
	{0060000, 0170000, "add",  do_add, HAS_SS | HAS_DD},
	{0077000, 0177000, "sob", do_sob, HAS_NN}, 
	{0005000, 0177000, "clr", do_clr, HAS_DD},
	{0000000, 0000000, "unknown", do_unknown, HAS_NN},
	{0000400, 0177400, "br", do_br, HAS_XX},
	{0001400, 0177400, "beq", do_beq, HAS_XX}
};

#define L0(x) ((x)& 0xFF)
#define HI(x) (((x)>>8) & 0xFF)
#define pc reg[7]
#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD 2 // (1<<1)
#define HAS_XX 4 // (1<<2)
#define HAS_NN 8 // (1<<3)
#define Z NZVC[1]


byte b_read  (adr a);            
void b_write (adr a, byte val);  
word w_read  (adr a);            
void w_write (adr a, word val);  
void load_file();
void run(adr pc0);
                
void mem_dump(adr start, word n);
void reg_dump();

void test_mem(); 
                               
void do_add();
void do_mov();
void do_unknown();
void do_halt();
void do_sob();
void do_clr();
void do_br();
void do_beq();

word get_nn(word w);
int get_reg_number_sob(word w);
struct Argument get_dd(word w);*/

int main(int argc, char * argv[])
{
	char * filename = NULL;
	if (argc > 1)
		filename = argv[1];
	
	test_mem();
	load_file(filename);
	run(01000);
	printf("reg[3] = %o\n", reg[3]);
	return 0;
}
