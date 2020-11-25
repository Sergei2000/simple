#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
//#include <Protocol/MpService.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
 
#define ICR_SHIFT  0x300
#define LOCAL_APIC_REG  0xFEE00000
#define VIDEO_BUFER 0x80000000
#define INIT_IPI_command 0x000c4500 //пока для всех процессоров 
#define SIPI_command 0x000c4601
#define BLUE 0xe1ff
#define GREEN 0xe144
struct gdtr
{
    unsigned short _limit;
    unsigned char* _base;
} __attribute__((packed));

struct gdt
{
	unsigned char* _descriptor ;
};

void store_gdt_desc(struct gdtr *location) {
   asm volatile ("sgdt %0" : : "m"(*location) : "memory");
}





void gdt_print(struct gdtr *location)
{
	UINTN i,j;
	struct gdt gdtobj;
	
	gdtobj._descriptor=(unsigned char*)location->_base;


	for (i=0;i<=location->_limit;i+=8)
	{
		for(j=0;j<8;++j)
		{
			Print(L" %x",*(gdtobj._descriptor+i+j));
		}
		Print(L"\n");
	}
	 
}

VOID 
EFIAPI
MoveCode()
{
unsigned char core_bin[] = {
  0x31, 0xc0, 0xb8, 0x10, 0x00, 0x8e, 0xd8, 0x66, 0x31, 0xc0, 0xb8, 0x00,
  0x01, 0x8e, 0xd8, 0x66, 0x31, 0xc0, 0x66, 0x89, 0xc6, 0xb8, 0x46, 0x10,
  0x89, 0xc6, 0x66, 0x31, 0xc0, 0xb8, 0x00, 0x20, 0x8e, 0xc0, 0x31, 0xc0,
  0x89, 0xc7, 0x66, 0x31, 0xc9, 0x8a, 0x0e, 0x57, 0x10, 0xf3, 0xa4, 0xfa,
  0x31, 0xc0, 0x8e, 0xd8, 0x0f, 0x01, 0x16, 0x70, 0x10, 0x0f, 0x20, 0xc0,
  0x0c, 0x01, 0x0f, 0x22, 0xc0, 0xea, 0x46, 0x10, 0x08, 0x00, 0x66, 0xb8,
  0x10, 0x00, 0x8e, 0xc0, 0x8e, 0xd8, 0xc6, 0x05, 0x00, 0x01, 0x00, 0x00,
  0x01, 0xeb, 0xfe, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00, 0xff, 0xff, 0x00, 0x00,
  0x00, 0x96, 0xcf, 0x00, 0x17, 0x00, 0x58, 0x10, 0x00, 0x00
};
unsigned int core_bin_len = 118;






	volatile char *codemem = (volatile char*)0x1000;
	UINTN i;
	for (i=0;i<core_bin_len;++i)
	{
		codemem[i]=core_bin[i];
	}

}

VOID
EFIAPI
myprint(EFI_PHYSICAL_ADDRESS lfb_base_addr,UINT32 color)
{
    UINT32 *video = (UINT32*)lfb_base_addr;//(UINT32*)VIDEO_BUFER;
	UINTN i;
	for (i=0;i<200000;++i)
		video[i]=color;
}


VOID 
EFIAPI
printvalue(EFI_PHYSICAL_ADDRESS lfb_base_addr,UINT32 color,UINT32 position)
{
	UINT32 *video= (UINT32*)lfb_base_addr;
	UINT32 i;
	for (i=200000;i<200000+position;++i)
	{
		video[i]=color;
	}
}

VOID
EFIAPI
Send_init_sipi_sipi(UINT32 Apic_Id,EFI_GRAPHICS_OUTPUT_PROTOCOL* gop)
{
	UINT32* icr_low, *icr_high,i;
	icr_high = (UINT32*)(LOCAL_APIC_REG+ICR_SHIFT+0x10);
	icr_low =(UINT32*)(LOCAL_APIC_REG+ICR_SHIFT);
	icr_high[0]=(Apic_Id<<24);
	icr_low[0]=INIT_IPI_command;
	while((icr_low[0]&(1<<12)))
	{
	}
    
	for (i=0;i<1000;++i)
	{}
    icr_low[0]=SIPI_command;
    while((icr_low[0]&(1<<12)))
	{}
    for (i=0;i<2000;++i)
    {}// thinking of normal deleay
    icr_low[0]=SIPI_command;
    while((icr_low[0]&(1<<12)))
	{}
	for (i=0;i<5000000;++i)
	{}
	//myprint(gop->Mode->FrameBufferBase,GREEN);
}



EFI_STATUS
EFIAPI
test (
    IN EFI_HANDLE         ImageHandle,
    IN EFI_SYSTEM_TABLE   *SystemTable
    )
{
 // EFI_PROCESSOR_INFORMATION info;
  //EFI_MP_SERVICES_PROTOCOL* mpProto= NULL;
  // UINT32 Apic_Id;
  // UINTN apic_adr;
  EFI_STATUS Status;
 // UINTN mynum;
  //UINTN number,enabled;
  
  EFI_HANDLE* handle_buffer;
 // struct gdtr desc;
  EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
  EFI_MEMORY_DESCRIPTOR *MemoryMap;
  UINTN MemoryMapSize;
  UINT32* vbuf,tmp;
  UINTN  MapKey,handle_count;
  UINTN DescriptorSize;
  UINT32 DescriptorVersion;
  MemoryMapSize = 0;
  MemoryMap     = NULL;
  
  Status = gBS->LocateHandleBuffer( ByProtocol,
                                      &gEfiGraphicsOutputProtocolGuid,
                                      NULL,
                                      &handle_count,
                                      &handle_buffer );
   

  Status = gBS->HandleProtocol( handle_buffer[0],
                                  &gEfiGraphicsOutputProtocolGuid,
                                  (VOID **)&gop );



 // Status = SystemTable->BootServices->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID **)&mpProto);
  //Status = mpProto->GetNumberOfProcessors(mpProto,&number,&enabled);
  //Print(L"number is %d %d\n",number,enabled);
  //Status = mpProto->WhoAmI(mpProto,&mynum);
  //Print(L"I am working with %d processor\n And here is my info:",mynum);
  //Status = mpProto->GetProcessorInfo(mpProto,mynum,&info);
  //Print(L"%r \n Package:%d Core:%d Thread:%d \n",Status,info.Location.Package,info.Location.Core,info.Location.Thread);
  

  //exit boot services

//  store_gdt_desc(&desc);
 // Print(L"%x %x\n",desc._limit,desc._base);
 // gdt_print(&desc);
  

   Status = gBS->GetMemoryMap (
                  &MemoryMapSize,
                  MemoryMap,
                  &MapKey,
                  &DescriptorSize,
                  &DescriptorVersion
                  );
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);
  do 
  {
  	gBS->AllocatePool(EfiBootServicesData,MemoryMapSize,(VOID **)&MemoryMap);
    ASSERT (MemoryMap != NULL);
    Status = gBS->GetMemoryMap (
                    &MemoryMapSize,
                    MemoryMap,
                    &MapKey,
                    &DescriptorSize,
                    &DescriptorVersion
                    );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (MemoryMap);
    }
  } while (Status == EFI_BUFFER_TOO_SMALL);

  Status =gBS->ExitBootServices(ImageHandle,MapKey);
  if (EFI_ERROR (Status))
  {
  	Status = gBS->GetMemoryMap (
                    &MemoryMapSize,
                    MemoryMap,
                    &MapKey,
                    &DescriptorSize,
                    &DescriptorVersion
                    );
    Status = gBS->ExitBootServices(ImageHandle,MapKey);
  }
  

 
  if (EFI_ERROR(Status))  
  {
  	Print(L"couldn't exit BootServices\n");
  	return EFI_SUCCESS;
  }
 
  
  MoveCode();
     //|
    // |
   //__|
  
  vbuf=(UINT32*)0x100;//зануляю область памяти, чтобы потом изменить её в асм коде
  vbuf[0]=0;
  tmp=vbuf[0];
  Send_init_sipi_sipi(1,gop);
  
  
 
 
  for (;;)
  {
  	if (vbuf[0]==1)
    {
 	    myprint(gop->Mode->FrameBufferBase,GREEN);
      for (;;)
      {
        /* code */
      }
    }
    if(vbuf[0]==0)
    {
 	    printvalue(gop->Mode->FrameBufferBase,BLUE,200000);
    }
  } 
  return EFI_SUCCESS;
}
