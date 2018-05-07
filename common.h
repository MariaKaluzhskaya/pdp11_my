#ifndef _PDP_COMMON_H
#define _PDP_COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define L0(x) ((x)& 0xFF)
#define HI(x) (((x)>>8) & 0xFF)
#define pc reg[7]
#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD 2 // (1<<1)
#define HAS_XX 4 // (1<<2)
#define HAS_NN 8 // (1<<3)
#define Z NZVC[1]
#define N NZVC[2]
#define ostat 177564
#define odata 177566


typedef unsigned char byte;
typedef unsigned short int word;
typedef word adr;

struct Command {
	word opcode;
	word mask;
	char* name;
	void(*func)();
	byte param;
};

byte mem[64*1024];
word reg[8];
int NZVC[4];
word nn, xx;
int reg_number_sob;
int b_or_w;

struct Argument {
	adr a;
	word val;
	byte space;
};

struct Argument ss, dd;

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
struct Argument get_dd(word w);



#endif
