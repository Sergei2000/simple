use16
xor ax,ax;
mov ax,0x100;
mov ds,ax;
lock mov byte[ds:00],1; меняю область памяти для проверки передачи управления


xor eax,eax;
mov ax,0xb800; так как не является видео буфером я решил его сделать стеком (пока временно)
mov ss,ax;
mov ax,200h;
mov sp,ax;


xor eax,eax;
mov ax,cs;
shl eax,4;
mov word[code+2],ax;
rol eax,16;
mov byte[code+4],al;
mov byte[code+7],ah;

xor eax,eax;
mov ax,cs;

shl eax,4;

xor eax,eax;
mov ax,ds;
shl eax,4;
mov word[data_seg+2],ax;
rol eax,16;
mov byte[data_seg+4],al;
mov byte[data_seg+7],ah;

xor eax,eax;
mov ax,ds;
shl eax,4;
add eax,gdt;
mov  [gdt_reg.adresss],eax;

in al, 0x92
or al, 2
out 0x92, al


cli;
in al,0x70;
or al,10000000b;
out 0x70,al; disable interrupts


lgdt fword[gdt_reg];

jmp $; debug

mov eax,cr0;
or al,1;
mov cr0,eax; enable protected mode

mov ax,32;
mov ss,ax;
mov ax, 200h;
mov sp,ax; initialize stack


mov ax,24;
mov ds,ax;
mov word[ds:0],0xe1ff
mov word[ds:1],0xe1ff
mov word[ds:2],0xe1ff
jmp 8:moy_cod


use32
moy_cod:

mov ax,32;
mov ss,ax;
mov ax, 200h;
mov sp,ax; initialize stack


mov ax,24;
mov ds,ax;
mov word[ds:0],0xe1ff
mov word[ds:1],0xe1ff
mov word[ds:2],0xe1ff
current:
jmp 8:current;


struc gdt_registr razmer,adresss
{
  .razmer dw razmer
  .adresss dd adresss
}

gdt_reg gdt_registr 39,0
align 8
gdt: 
zero db 0,0,0,0,0,0,0,0;
code db 0xff,0xff,0,0,0,10011010b,11001111b,0; pamyat dlya coda v 32 mode
video_adapter db 0xff,0xff,0xe0,0x00,0x00,10010010b,11001111b,0x80;pamyat video adaptera
data_seg db 0xff, 0xff, 0, 0, 0,10010110b,11001111b,0
stack_seg db 0xff,0xff,0xe0,0x80,0x0b,10010010b,11001111b,0x00;stack memeory 
