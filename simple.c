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
#define SIPI_command 0x000c4610
#define BLUE 0xe1ff
#define GREEN 0xe144


VOID 
EFIAPI
MoveCode()
{
unsigned char core_bin[] = {
  0x31, 0xc0, 0xb8, 0x00, 0x01, 0x8e, 0xd8, 0xf0, 0xc6, 0x06, 0x00, 0x00,
  0x01, 0x66, 0x31, 0xc0, 0xb8, 0x00, 0xb8, 0x8e, 0xd0, 0xb8, 0x00, 0x02,
  0x89, 0xc4, 0x66, 0x31, 0xc0, 0x8c, 0xc8, 0x66, 0xc1, 0xe0, 0x04, 0xa3,
  0xea, 0x00, 0x66, 0xc1, 0xc0, 0x10, 0xa2, 0xec, 0x00, 0x88, 0x26, 0xef,
  0x00, 0x66, 0x31, 0xc0, 0x8c, 0xc8, 0x66, 0xc1, 0xe0, 0x04, 0x66, 0x31,
  0xc0, 0x8c, 0xd8, 0x66, 0xc1, 0xe0, 0x04, 0xa3, 0xfa, 0x00, 0x66, 0xc1,
  0xc0, 0x10, 0xa2, 0xfc, 0x00, 0x88, 0x26, 0xff, 0x00, 0x66, 0x31, 0xc0,
  0x8c, 0xd8, 0x66, 0xc1, 0xe0, 0x04, 0x66, 0x05, 0xe0, 0x00, 0x00, 0x00,
  0x66, 0xa3, 0xdc, 0x00, 0xe4, 0x92, 0x0c, 0x02, 0xe6, 0x92, 0xfa, 0xe4,
  0x70, 0x0c, 0x80, 0xe6, 0x70, 0x66, 0x0f, 0x01, 0x16, 0xda, 0x00, 0x0f,
  0x20, 0xc0, 0x0c, 0x01, 0x0f, 0x22, 0xc0, 0xb8, 0x20, 0x00, 0x8e, 0xd0,
  0xb8, 0x00, 0x02, 0x89, 0xc4, 0xb8, 0x18, 0x00, 0x8e, 0xd8, 0xc7, 0x06,
  0x00, 0x00, 0xff, 0xe1, 0xc7, 0x06, 0x01, 0x00, 0xff, 0xe1, 0xc7, 0x06,
  0x02, 0x00, 0xff, 0xe1, 0xea, 0xa5, 0x00, 0x08, 0x00, 0x66, 0xb8, 0x20,
  0x00, 0x8e, 0xd0, 0x66, 0xb8, 0x00, 0x02, 0x66, 0x89, 0xc4, 0x66, 0xb8,
  0x18, 0x00, 0x8e, 0xd8, 0x66, 0xc7, 0x05, 0x00, 0x00, 0x00, 0x00, 0xff,
  0xe1, 0x66, 0xc7, 0x05, 0x01, 0x00, 0x00, 0x00, 0xff, 0xe1, 0x66, 0xc7,
  0x05, 0x02, 0x00, 0x00, 0x00, 0xff, 0xe1, 0xea, 0xd3, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00,
  0xff, 0xff, 0xe0, 0x00, 0x00, 0x92, 0xcf, 0x80, 0xff, 0xff, 0x00, 0x00,
  0x00, 0x96, 0xcf, 0x00, 0xff, 0xff, 0xe0, 0x80, 0x0b, 0x92, 0xcf, 0x00
};
unsigned int core_bin_len = 264;








	volatile char *codemem = (volatile char*)0x10000;
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



   Status = gBS->GetMemoryMap (
                  &MemoryMapSize,
                  MemoryMap,
                  &MapKey,
                  &DescriptorSize,
                  &DescriptorVersion
                  );
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);
  do {
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
  Status =gBS->ExitBootServices(ImageHandle,MapKey);
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
  
  vbuf=(UINT32*)0x1000;//зануляю область памяти, чтобы потом изменить её в асм коде
  vbuf[0]=0;
  tmp=vbuf[0];
  Send_init_sipi_sipi(1,gop);
  
  
 
 
  for (;;)
  {
  	if (vbuf[0]==1)
 {
 	myprint(gop->Mode->FrameBufferBase,GREEN);
 }
 if(vbuf[0]==0)
 {
 	printvalue(gop->Mode->FrameBufferBase,BLUE,200000);
 }
  } 
  return EFI_SUCCESS;
}
