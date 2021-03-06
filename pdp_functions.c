#include <stdlib.h>
#include "./common.h"

struct Command commands[] = {
	{0,       0177777, "halt", do_halt, NO_PARAM}, //0xFFF
	{0010000, 0170000, "mov",  do_mov, HAS_SS | HAS_DD},
	{0110000, 0170000, "movb", do_mov, HAS_SS | HAS_DD},
	{0060000, 0170000, "add",  do_add, HAS_SS | HAS_DD},
	{0077000, 0177000, "sob", do_sob, HAS_NN}, 
	{0005000, 0177000, "clr", do_clr, HAS_DD},
	{0000400, 0177400, "br", do_br, HAS_XX},
	{0001400, 0177400, "beq", do_beq, HAS_XX},
	{0100000, 0177400, "bpl", do_bpl, HAS_XX},
	{0105700, 0177700, "tstb", do_tst, HAS_DD},
	{0005700, 0177700, "tst", do_tst, HAS_DD},
	{0004000, 0177000, "jsr", do_jsr, HAS_DD | HAS_R},
	{0000200, 0177700, "rts", do_rts, HAS_R1},
	{0000000, 0000000, "unknown", do_unknown, HAS_NN}
};
void setNZ(word w) {
	if (b_or_w) 
	{	// byte
		w = w & 0xFF;
		N = (w >> 7) & 1;
	} 
	else 
	{		// word
		w = w & 0xFFFF;
		N = (w >> 15) & 1; 
	}
	Z = !w;
}
word get_nn(word w) {
	return w & 077;
}

char get_xx(word w) {
	char res = w & 0377;
	printf("%d\n", res);
	return (w & 0377);
}

int get_reg_number_sob(word w) {
	return (w >> 6) &7;
}
struct Argument get_dd(word w) {
	struct Argument res;
	int rn = w & 7;
	xx = w & 255;
	nn = w + 1;
	int mode = (w >> 3) & 7;
	
	res.space = 1;

	switch(mode) {
		case 0:
				res.space = 0;
				res.a = rn;
				res.val = reg[rn];
				printf("r%d ", rn);
				break;
		case 1:
				
				res.a = reg[rn];
				if(!b_or_w) {
					res.val = w_read(res.a);
				}
				else {
					res.val = b_read(res.a);
				}
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
					if (rn == 6 || rn == 7)
						reg[rn]++;
				} 				// TODO: +1 if 
				//printf("rn = %d res.val=%o, reg[%d]=%o\n", rn, res.val, rn, reg[rn]);
				if (rn == 7)
					printf("#%o ", res.val);
				else
					printf("(r%d)+ ", rn);
				break;
		case 3: 
				res.a = w_read(reg[rn]);
				if(!b_or_w) {
					res.val = w_read(res.a);
					reg[rn] += 2;
				}	
				else {
					res.val = b_read(res.a);
					reg[rn] += 1;
					if (rn == 6 || rn == 7)
						reg[rn]++;
				} 				// TODO: +1 if 
				if (rn == 7)
					printf("#%o ", res.val);
				else
					printf("@(r%d)+ ", rn);
				break;
		case 4:
				res.a = reg[rn];
				if(!b_or_w) {
					reg[rn] -= 2;
					res.val = w_read(res.a);
				}	
				else {
					reg[rn] -= 1;
					res.val = b_read(res.a);
					if (rn == 6 || rn == 7)
						reg[rn]++;
				} 				// TODO: +1 if 
				//printf("rn = %d res.val=%o, reg[%d]=%o\n", rn, res.val, rn, reg[rn]);
				if (rn == 7)
					printf("#%o ", res.val);
				else
					printf("-(r%d) ", rn);
				break;
		case 6: 	
				memory = w_read(pc);
				pc += 2;
				res.a = reg[rn];
				res.a += memory;
				res.a  = res.a & 0xffff;
				res.val = ((b_or_w == 1) & (rn < 6))? b_read(res.a) : w_read(res.a);
				if (N == 7)
					printf("%o", res.a);
				else
					printf("%o(R%o) ", memory, rn);
				break;
		case 7:
				Z = w_read(pc);
				pc += 2;
				res.a = reg[rn];
				res.a += Z;
				res.a = w_read(res.a);
				res.a = res.a & 0xffff;
				res.val = b_or_w ? b_read(res.a) : w_read(res.a);
				printf(" @%d(R%d)", memory, rn);
				break;
		/*case 6:
				if (rn == 7)
						reg[rn] += 2;
				res.a = reg[rn] + nn;
				res.val = w_read(res.a);
					printf("%o(%o) ", nn, res.val);
				break;*/
		default:
				printf("MODE %d NOT IMPLEMENTED YET!\n", mode);
				exit(1);
	}
	return res;
}
void do_jsr() {
	sp -= 2;
	w_write(sp, reg[reg_number_sob]);
	reg[reg_number_sob] = pc;
	pc = dd.a;
}
void do_rts() {
	pc = reg[r1];
	reg[r1] = w_read(sp);
	sp += 2;
}
/*void do_jsr()
{
	sp -=2;
	w_write(sp, reg[jr]);
	reg[jr] = pc;
	pc = dd.a;
}
void do_rts()
{
	pc = reg[rr];
	reg[rr] = w_read(sp);
	sp += 2;
}*/

void do_halt()
{
	printf("THE END!!!!\n");
	reg_dump();
	exit(0);
}

void write_dd(struct Argument dd) {
	if (dd.space == 0) {
		if (b_or_w) {
			reg[dd.a] = (byte)dd.val;
		} 
		else {
			reg[dd.a] = dd.val;
		}
	} 
	else {
		if (b_or_w) {
			b_write(dd.a, (byte)dd.val);
		}
		 else {
			w_write(dd.a, dd.val);
		}	
	}
}

void do_add() {
	dd.val = ss.val + dd.val;
	write_dd(dd);
	setNZ(dd.val);
}

void do_clr()
{
	dd.val = 0;
	write_dd(dd);
	setNZ(dd.val);
}

void do_mov() {
	dd.val = ss.val;
	write_dd(dd);
	setNZ(dd.val);
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
void do_bpl()
{
	if(N == 0)
		do_br();
}
void do_bne() { 
	if (Z == 0)
		do_br();
}
void do_tst()
{
	setNZ(dd.val);
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
					reg_number_sob = get_reg_number_sob(w);
				}
				if(cmd.param & HAS_SS) {
					ss = get_dd(w>>6);
				}
				if(cmd.param & HAS_DD) {
					dd = get_dd(w);
				}
				if(cmd.param & HAS_XX) {
					xx = get_xx(w);
				}
				 if (cmd.param & HAS_R) { //1 сдвинутый влево на 3, а мой на 4
                    reg_number_sob = get_reg_number_sob(w);
                }
                if (cmd.param & HAS_R1) { //1 сдвинутый влево на 5
                    r1 = w & 7;
                }
				cmd.func();
				break;
			}
		}
		printf("\n");
	}
}
					
byte b_read(adr a) {
	if(a == ostat)
		return 0xFF;
    return mem[a];
}

void b_write(adr a, byte val) {
	if(a == odata)
		fprintf(stderr, "%c", val);
	else
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

