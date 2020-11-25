use16
org 1000h


xor ax,ax;
mov ax,0x10;
mov ds,ax;
;mov byte[ds:00],1; меняю область памяти для проверки передачи управления



xor eax,eax;
mov ax,100h
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

xor ecx,ecx;
mov cl,byte[code_length];

rep movsb;

cli;
xor ax,ax
mov ds,ax;

lgdt [gdt_reg];


mov eax,cr0;
or al,1;
mov cr0,eax; enable protected mode

jmp  8:moy_cod;

moy_cod:
use32

mov ax,16;
mov es,ax;
mov ds,ax;
mov byte[ds:0x100],1;

jmp $;

moy_cod_end:

code_length db moy_cod_end-moy_cod; 


align 8
gdt: 
zero db 0,0,0,0,0,0,0,0;
code db 0xff,0xff,0,0,0,10011010b,11001111b,0; pamyat dlya coda v 32 mode
data_seg db 0xff, 0xff, 0, 0, 0,10010110b,11001111b,0; pamyat dlya dannyh v 32 mode 
gdtend:
gdt_reg:
	.size dw gdtend-gdt-1;
	.address dd gdt;

