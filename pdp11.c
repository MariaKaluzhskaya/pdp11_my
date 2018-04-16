//pdp11.c_kmv
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned short int word;
typedef word adr;

byte mem[64*1024];
word reg[8];
word nn;
int reg_number_sob;
int reg_number = 100;

struct Argument {
	adr a;
	word val;
};
struct Argument ss, dd;

#define L0(x) ((x)& 0xFF)
#define HI(x) (((x)>>8) & 0xFF)
#define pc reg[7]
#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD 2 // (1<<1)
#define HAS_XX 4 // (1<<2)
#define HAS_NN 8 // (1<<3)

byte b_read  (adr a);            // читает из "старой памяти" mem байт с "адресом" a.
void b_write (adr a, byte val);  // пишет значение val в "старую память" mem в байт с "адресом" a.
word w_read  (adr a);            // читает из "старой памяти" mem слово с "адресом" a.
void w_write (adr a, word val);  // пишет значение val в "старую память" mem в слово с "адресом" a.
void load_file();                //читает из стандартного потока ввода данные исполняемого файла
                                 //в формате s-rec и записывает их в "память" старой машины
                                 //с помощью функции b_wtite.
void mem_dump(adr start, word n);//печатает на стандартный поток вывода n байт, начиная с адреса start в формате:
                                 //адрес слова в виде восьмеричного числа с ведущими нулями, двоеточие,
                                 //содержимое слова в восьмеричном виде с ведущими нулями через пробел.
void test_mem();                                
void do_add();
void do_mov();
void do_unknown();
void do_halt();
void do_sob();
void run(adr pc0);

word get_nn(word w) {
	return w & 077;
}
int get_reg_number_sob(word w) {
	return (w >> 6) &7;
}
struct Argument get_dd(word w) {
	struct Argument res;
	int rn = w & 7;
	int mode = (w >> 3) & 7;
	switch(mode) {
		case 0:
				res.a = rn;
				res.val = reg[rn];
				printf("r%d ", rn);
				break;
		case 1:
				res.a = reg[rn];
				res.val = w_read(res.a);	// TODO: byte varant
				printf("(r%d) ", rn);
				break;
		case 2:
				res.a = reg[rn];
				res.val = w_read(res.a);	// TODO: byte variant
				reg[rn] += 2; 				// TODO: +1 if 
				//printf("rn = %d res.a=%o res.val=%o, reg[%d]=%o\n", rn, res.a, res.val, rn, reg[rn]);
				if (rn == 7)
					printf("#%o ", res.val);
				else
					printf("(r%d) ", rn);
				break;
		default:
				printf("MODE %d NOT IMPLEMENTED YET!\n", mode);
				exit(1);
	}
	return res;
}

struct Command {
	word opcode;
	word mask;
	char* name;
	void(*func)();
	byte param;
	
} commands[] = {
	{0,       0177777, "halt", do_halt, NO_PARAM}, //0xFFF
	{0010000, 0170000, "mov",  do_mov, HAS_SS | HAS_DD},
	{0060000, 0170000, "add",  do_add, HAS_SS | HAS_DD}, 
	{0000000, 0000000, "unknown", do_unknown, HAS_NN},
	{0077000, 0177000, "sob", do_sob, HAS_NN}
};

                          
int main(int argc, char * argv[])
{
	char * filename = NULL;
	if (argc > 1)
		filename = argv[1];
	
	test_mem();
	load_file(filename);
	run(01000);
	printf("Normal\n");
	return 0;
}

void do_halt()
{
	printf("THE END!!!!\n");
	exit(0);
}

void do_add() {
	w_write(dd.a, ss.val + dd.val);
}

void do_mov() {
	//w_write(dd.a, ss.val);
	//printf("%d\n", dd.a);
	reg[dd.a] = ss.val; 
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
		for(i = 0; i < 4; ++i)
		{
			struct Command cmd = commands[i];
			if((w & cmd.mask) == cmd.opcode)
			{
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
				/*if(cmd.param & )
				{
					reg_number_sob = get_reg_number_sob(w);
				}*/
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
    //printf("mem[a+1] = %x\n", w);
    w = w << 8;
    //printf("w<<8 = %x\n", w);
    w  = w | (word)mem[a];
    //printf("%x\n", w);
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
void do_sob() {
	printf("r%d 0%6o", reg_number_sob, pc - 2*nn);
	reg[reg_number_sob] --;
	if(reg[reg_number] != 0)
	pc = pc - 2*nn;
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
