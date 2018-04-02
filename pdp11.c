//pdp11.c_kmv
#include <stdio.h>
#include <assert.h>

typedef unsigned char byte;
typedef unsigned short int word;
typedef word adr;

byte mem[64*1024];

#define L0(x) ((x)& 0xFF)
#define HI(x) (((x)>>8) & 0xFF)

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
                                 
int main()
{
	printf("Normal\n");
	return 0;
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
    assert(a%2 == 0);
    mem[a] = (byte)L0(val);
    mem[a+1] = (byte)HI(val);
}

void load_file() 
{
    unsigned int a, n;
    FILE * f;
    f = stdin;//fopen("c:/in.txt", "r");
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
