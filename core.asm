use16

xor ax,ax;
mov ax,0x100;
mov ds,ax;
lock mov byte[ds:00],1; меняю область памяти для проверки передачи управления


xor eax,eax;
mov ax,cs;
shl eax,4;
add eax,gdt;
mov  [gdt_reg.adresss],eax;

xor eax,eax;
mov ax,1000h
mov ds, ax;
xor eax,eax;
mov esi,eax;

mov ax,moy_cod;
mov si,ax;

xor eax,eax;
mov ax,2000h;
mov es,ax;
xor ax,ax;
mov di,ax;

xor cx,cx;
mov cl,byte[code_length];

rep movsb;



cli;
xor ax,ax
mov ds,ax;

lgdt fword[gdt_reg];


mov eax,cr0;
or al,1;
mov cr0,eax; enable protected mode

jmp  fword 8:20000h;

moy_cod:
use32
jmp $;
moy_cod_end:

code_length db moy_cod_end-moy_cod; 

struc gdt_registr razmer,adresss
{
  .razmer dw razmer
  .adresss dd adresss
}

gdt_reg gdt_registr 23,0
align 8
gdt: 
zero db 0,0,0,0,0,0,0,0;
code db 0xff,0xff,0,0,0,10011010b,11001111b,0; pamyat dlya coda v 32 mode
data_seg db 0xff, 0xff, 0, 0, 0,10010110b,11001111b,0; pamyat dlya dannyh v 32 mode 


