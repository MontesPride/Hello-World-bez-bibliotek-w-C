    // (Windows only)
    // > gcc hello.c -o hello.exe -m32
    // > hello
    // Hello World!
    
    
    // This returns 32 bits as char* from a given address
    char* ref(char *x){ return (char*)*((int*)x); }
    
    // Type definition - Function pointer that takes registers
    // as arguments
    typedef char* __fastcall (*what_is_my_purpose)(int);
    
    // A global variable. Volatile is used so that the compiler
    // will not optimise access to that variable (i.e. I want the compiler
    // to emit code related to this variable whereas it would put the result
    // immediately into that variable otherwise).
    volatile unsigned lul = 0;
    int main(int argc, char **argv){
        
        // This with the volatile above forces compiler to emit this instruction
        // OR DWORD PTR [lul], 3271658340
        // (or something familiar)
        // Instruction itself is not important but that number there - 
        // 0xc3018b64 so in memory it will be 64 8b 01 c3 (little endian)
        // If we look at these numbers as assembly instructions rather then bytes
        // then we get following code
        // 64 8b 01     MOV     EAX, DWORD PTR FS:[ECX]
        // c3           RET
        // This is a function that returns 32 bits from FS segment and ECX offset (function argument)
        lul |= 3271658340U;
        
        // I assign the address of main() to wtf i.e. address in memory
        // where machine code of main() is placed.
        char *wtf = (char*)main;
        
        // This loop is searching for this 3271658340 value.
        // At the end we get the address of lul in wtf.
        while (*((unsigned*)wtf) != lul) wtf++;
        
        // This is where the magic begins (hence the name)
        // I convert the address of the lul variable (that function
        // that returns 32-bits from FS segment) to function pointer
        // and immediately call it with argument 48 (0x30).
        // FS segment points onto TEB (thread environment block)
        // which is a small object containing all the system information
        // about a thread
[TED](http://terminus.rewolf.pl/terminus/structures/ntdll/_TEB_combined.html)

        // On the other hand FS:0x30 is a field that contain PEB (process
        // environment block, which is analogous to TEB but contains information
        // about the process)
[PEB](http://terminus.rewolf.pl/terminus/structures/ntdll/_PEB_combined.html)

        char *magic = ((what_is_my_purpose)wtf)(48);
        
        // PEB+12 contains address of an object with information about DLL
        // module loader i.e. LDR_DATA
[LDR_DATA](http://terminus.rewolf.pl/terminus/structures/ntdll/_PEB_LDR_DATA_combined.html)

        magic = ref(magic+12);
        
        // LDR_DATA+28 contains the address of the beginning of a list of loaded DLLs.
        // In each iteration of this loop, magic points onto 3rd field of this:
[LDR_DATA_TABLE_ENTRY](http://www.nirsoft.net/kernel_struct/vista/LDR_DATA_TABLE_ENTRY.html)

        // Whereas magic+32 is BaseDLLName.Buffer which is the name of a current DLL
        // as unicode (1 char = 2 bytes)
        // That condition there says: '5th character cannot be t' or in other words
        // if '5th character is t' then I have found what we were looking for.
        // Sounds like looking for msvcrt.dll  (Note that we are indexing from 0)
        // which is a standard C library from Microsoft.
        for(magic = ref(magic+28); *(ref(magic+32)+10) != 't'; magic = ref(magic));
        
        // Here magic+8 is DLLBase which is the address of that DLL (msvcrt.dll)
        magic = ref(magic+8);
        int base = (int)magic;
        
        // Code below is a parsing of the EXE header and portable executable (PE)
        // (DLL libraries have got a PE structure btw)
[PE structure](https://raw.githubusercontent.com/corkami/pics/master/binary/pe101/pe1011pl.png)

        // First we need to find the address of that PE structure - it is located on an offset
        // EXE+60 (e_lfanew field)
        magic += (int)ref(magic+60);
        
        // PE+120 is the address of a table of exported functions
        // IMAGE_OPTIONAL_HEADER.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]
        // .VirtualAddress
[IMAGE_OPTIONAL_HEADER](https://docs.microsoft.com/en-us/windows/desktop/api/winnt/ns-winnt-_image_optional_header)

[VirtualAddress](https://docs.microsoft.com/en-us/windows/desktop/api/winnt/ns-winnt-_image_data_directory)

        magic = ref(magic+120) + base;
        char *exp = magic;
        
        // Code below browses functions searching for something
        // It searches for 32 bits that are equal to:
        //    1937012080
        // If we look at it in hex we get:
        //    0x73747570
        // If we write it down as bytes in memory (little endian) we get:
        //    70 75 74 73
        // And if we look at it as ASCII characters we get:
        //    0x70 -> p
        //    0x75 -> u
        //    0x74 -> t
        //    0x73 -> s
        // Sounds familiar? ;)
        magic = ref(magic+32) + base;
        int i; for(i=0; (int)ref((char*)(((int*)magic)[i]+base)) != 1937012080; i++);
        
        // After we have found a name, we to find the address of that function
        // It can be found in this structure:
[IMPORT TABLE](http://win32assembly.programminghorizon.com/pe-tut6.html)

        magic = ref(exp+36) + base;
        magic = (char*)((int)ref(magic+i*2) & 0xFFFF);
        magic = ref(exp+28) + base;
        magic = ref(magic+i*4) + base;
        
        // Here magic contains the address of the put function
        // All there is to do now is to cast it onto a function pointer
        // of the correct type and call it :)
        ((int (__cdecl *)(char*))magic)("Hello World!");
        return 0;
    }
