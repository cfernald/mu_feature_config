/** @file
  Initialize DFCI unsigned list.

  Copyright (c) Microsoft Corporation.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>

#include <Guid/ZeroGuid.h>

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/ConfigSystemModeLib.h>

/**
  This PEIM will update the PCD content based on the system operation mode,
  before the DFCI has a chance to look at the file guid PCD. If this is MFG
  mode, that means the unsigned settings will be accepted, thus the file GUID
  PCD contains a legit value. Otherwise, it will be supplied with a bogus/null
  value so that DFCI will not be able to locate any xml and eventually not
  taking any unsigned settings.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval  EFI_SUCCESS   The PEIM executed normally.
  @retval  !EFI_SUCCESS  The PEIM failed to update dynamic PCD.
**/
EFI_STATUS
EFIAPI
ConfDfciUnsignedListInitEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINTN       Size;
  EFI_STATUS  Status;

  if (!IsSystemInManufacturingMode ()) {
    // Invalidate the PCD if system operation does not allow it.
    Size   = sizeof (EFI_GUID);
    Status = PcdSetPtrS (PcdUnsignedPermissionsFile, &Size, &gZeroGuid);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a Setting dynamic PCD failed %r\n", __FUNCTION__, Status));
      ASSERT (FALSE);
      return Status;
    }

    if (Size != sizeof (EFI_GUID)) {
      DEBUG ((DEBUG_ERROR, "%a Setting dynamic PCD returned with unexpected size 0x%x\n", __FUNCTION__, Size));
      ASSERT (FALSE);
      return EFI_ABORTED;
    }
  }

  return EFI_SUCCESS;
}
