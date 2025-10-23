#include "efi.h"
#include "efilib.h"

extern void (*EnableCompatibilityMode)();

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    Print(L"Entering compatibility mode...\n");

    EnableCompatibilityMode();

    while (1);

    return EFI_SUCCESS;
}
