// (Windows only)
// > gcc hello.c -o hello.exe -m32
// > hello
// Hello World!

char* ref(char *x){ return (char*)*((int*)x); }
typedef char* __fastcall (*what_is_my_purpose)(int);
volatile unsigned lul = 0;
int main(int argc, char **argv){
    lul |= 3271658340U;
    char *wtf = (char*)main;
    while (*((unsigned*)wtf) != lul) wtf++;
    char *magic = ((what_is_my_purpose)wtf)(48);
    magic = ref(magic+12);
    for(magic = ref(magic+28); *(ref(magic+32)+10) != 't'; magic = ref(magic));
    magic = ref(magic+8);
    int base = (int)magic;
    magic += (int)ref(magic+60);
    magic = ref(magic+120) + base;
    char *exp = magic;
    magic = ref(magic+32) + base;
    int i; for(i=0; (int)ref((char*)(((int*)magic)[i]+base)) != 1937012080; i++);
    magic = ref(exp+36) + base;
    magic = (char*)((int)ref(magic+i*2) & 0xFFFF);
    magic = ref(exp+28) + base;
    magic = ref(magic+i*4) + base;
    ((int (__cdecl *)(char*))magic)("Hello World!");
    return 0;
}
