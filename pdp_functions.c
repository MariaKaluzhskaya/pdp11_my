#include "common.h"

struct Command commands[] = {
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

word get_nn(word w) {
	return w & 077;
}
int get_reg_number_sob(word w) {
	return (w >> 6) &7;
}
struct Argument get_dd(word w) {
	struct Argument res;
	int rn = w & 7;
	xx = w & 255;
	int mode = (w >> 3) & 7;
	switch(mode) {
		case 0:
				res.a = rn;
				res.val = reg[rn];
				printf("r%d ", rn);
				break;
		case 1:
				res.a = reg[rn];
				if(!b_or_w) 	
					res.val = w_read(res.a);
				else
					res.val = b_read(res.a);
				printf("(r%d) ", rn);
				break;
		case 2:
				res.a = reg[rn];
				if(!b_or_w) {
					res.val = w_read(res.a);
					reg[rn] += 2;
				}	
				else {
					res.val = b_read(res.a);
					reg[rn] += 1;
				} 				
				printf("rn = %d res.a=%o res.val=%o, reg[%d]=%o\n", rn, res.a, res.val, rn, reg[rn]);
				if (rn == 7)
					printf("#%o ", res.val);
				else
					printf("(r%d) ", rn);
				break;
		case 3:
				
		default:
				printf("MODE %d NOT IMPLEMENTED YET!\n", mode);
				exit(1);
	}
	return res;
}
void do_halt()
{
	printf("THE END!!!!\n");
	reg_dump();
	exit(0);
}

void do_add() {
	reg[dd.a] = ss.val + dd.val;
	Z = !(reg[dd.a]);
}

void do_clr()
{
	reg[dd.a] = 0;
	Z = 1;
}

void do_mov() {
	reg[dd.a] = ss.val;
	Z = !(reg[dd.a]); 
}

void do_unknown() {
	exit(1);
}

void run(adr pc0) {
	pc = pc0;
	int i;
	printf("adr   :opcode\n");
	while(1) {
		word w = w_read(pc);
		printf("%06o:%06o ", pc, w);
		pc += 2;
		for(i = 0; ; ++i)
		{
			struct Command cmd = commands[i];
			if((w & cmd.mask) == cmd.opcode)
			{
				b_or_w = (w >> 15) & 1;
				printf("%s ", cmd.name);
				//args
				if(cmd.param & HAS_NN) {
					nn = get_nn(w);
				}
				if(cmd.param & HAS_SS) {
					ss = get_dd(w>>6);
				}
				if(cmd.param & HAS_DD) {
					dd = get_dd(w);
				}
				if(cmd.param & HAS_NN)
				{
					reg_number_sob = get_reg_number_sob(w);
				}
				cmd.func();
				break;
			}
		}
		printf("\n");
	}
}
					
byte b_read(adr a) {
    return mem[a];
}

void b_write(adr a, byte val) {
    mem[a] = val;
}

word w_read  (adr a) {
    //assert(a % 2 == 0);
    word w;
    w = (word)mem[a + 1];
    w = w << 8;
    w  = w | (word)mem[a];
    return w;
}

void w_write(adr a, word val) {
    assert(a % 2 == 0);
    mem[a] = (byte)L0(val);
    mem[a+1] = (byte)HI(val);
}

void load_file(const char * filename) 
{
    unsigned int a, n;
    FILE * f;
    if (filename == NULL) 
		f = stdin;
	else {
		f = fopen(filename, "r");
		if (f == NULL) {
			perror("open pdp file");
			exit(1);
		}
	}
    unsigned int val;
    unsigned int i;
    while(fscanf(f, "%x%x", &a, &n) == 2)
    {
        for(i = 0; i < n; ++i)
        {
            fscanf(f, "%x", &val);
            b_write (a + i, (byte)val);
        }
    }
	fclose(f);
}

void mem_dump(adr start, word n)
{
    FILE* f = stdout;
    adr p;
    for(p = start; p < start + n; p+=2)
    {
        fprintf(f, "%06o : %06o\n", p, w_read(p));
    }

}
void reg_dump() {
	int i;
	for (i=0; i< 8; i++)
		printf("r%d:%o ", i, reg[i]);
	printf("\n");
}
void do_sob() {
	word g = pc - 2*nn;
	printf("nn=%o r%d pc=%o goto=%6o reg[%d]=%o", nn, reg_number_sob, pc, g, reg_number_sob, reg[reg_number_sob]);
	reg[reg_number_sob] --;
	if(reg[reg_number_sob] != 0) {
		pc = g;
		printf("   set pc=%o ", pc);
	}
}

void do_br()
{
	pc += 2 * xx;
}

void do_beq()
{
	if(Z)
		do_br();
}

void test_mem() {
    byte b0, b1;
    word w;
    w = 0x0d0c;
    w_write(2, w);
    b0 = b_read(2); //0c
    b1 = b_read(3); //0d
    printf("%04x = %02x%02x\n", w, b1, b0);
    assert(b0 == 0x0c);
    assert(b1 == 0x0d);
    b_write(4, 0x0c);
    b_write(5, 0x0d);
    w = w_read(4);
    printf("%04x = %02x%02x\n", w, b1, b0);
}

