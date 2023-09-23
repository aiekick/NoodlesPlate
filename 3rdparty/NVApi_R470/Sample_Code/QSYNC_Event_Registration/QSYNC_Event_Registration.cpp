/************************************************************************************************************************************\
|*                                                                                                                                    *|
|*     Copyright © 2020 NVIDIA Corporation.  All rights reserved.                                                                     *|
|*                                                                                                                                    *|
|*  NOTICE TO USER:                                                                                                                   *|
|*                                                                                                                                    *|
|*  This software is subject to NVIDIA ownership rights under U.S. and international Copyright laws.                                  *|
|*                                                                                                                                    *|
|*  This software and the information contained herein are PROPRIETARY and CONFIDENTIAL to NVIDIA                                     *|
|*  and are being provided solely under the terms and conditions of an NVIDIA software license agreement                              *|
|*  and / or non-disclosure agreement.  Otherwise, you have no rights to use or access this software in any manner.                   *|
|*                                                                                                                                    *|
|*  If not covered by the applicable NVIDIA software license agreement:                                                               *|
|*  NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.                                            *|
|*  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.                                                           *|
|*  NVIDIA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,                                                                     *|
|*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.                       *|
|*  IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,                               *|
|*  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT,                         *|
|*  NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.            *|
|*                                                                                                                                    *|
|*  U.S. Government End Users.                                                                                                        *|
|*  This software is a "commercial item" as that term is defined at 48 C.F.R. 2.101 (OCT 1995),                                       *|
|*  consisting  of "commercial computer  software"  and "commercial computer software documentation"                                  *|
|*  as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995) and is provided to the U.S. Government only as a commercial end item.     *|
|*  Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through 227.7202-4 (JUNE 1995),                                          *|
|*  all U.S. Government End Users acquire the software with only those rights set forth herein.                                       *|
|*                                                                                                                                    *|
|*  Any use of this software in individual and commercial software must include,                                                      *|
|*  in the user documentation and internal comments to the code,                                                                      *|
|*  the above Disclaimer (as applicable) and U.S. Government End Users Notice.                                                        *|
|*                                                                                                                                    *|
\************************************************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////
// @brief: This is the entry point for the console application.
//         This program registers QSYNC event to the system.
//         The received QSYNC event will be logged on the console of this application.
//
// @assumptions: This code is designed for Win10+. It assumes that the system has
//               atleast one GPU and a QSYNC board.
//
// @driver support: R460+
////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include "nvapi.h"


#define STATUS_SUCCESS  0
#define STATUS_FAIL     1


// forward declarations
void __cdecl HandleQSYNCEvent(NV_QSYNC_EVENT_DATA qsyncEventData, void *callbackParam);

int _tmain(int argc, _TCHAR* argv[])
{
    int iRet = STATUS_FAIL;  // default to fail
    NvEventHandle hEventQSYNC = NULL;
    NvAPI_Status status = NVAPI_ERROR;

    do
    {
        status = NvAPI_Initialize();
        if (NVAPI_OK != status)
        {
            // Initialization failed
            printf("NvAPI_Initialize() failed = 0x%x", status);
            break;
        }

        NvGSyncDeviceHandle nvGSyncHandles[NVAPI_MAX_GSYNC_DEVICES];
        NvU32 gsyncCount = 0;
        status = NvAPI_GSync_EnumSyncDevices(nvGSyncHandles, &gsyncCount);
        if (status == NVAPI_NVIDIA_DEVICE_NOT_FOUND || gsyncCount == 0)
        {
            printf("No Quadro Sync Devices found on this system\n");
            break;
        }

        NV_EVENT_REGISTER_CALLBACK nvQSYNCEventRegisterCallback;
        ZeroMemory(&nvQSYNCEventRegisterCallback, sizeof(nvQSYNCEventRegisterCallback));
        nvQSYNCEventRegisterCallback.nvCallBackFunc.nvQSYNCEventCallback = HandleQSYNCEvent;
        nvQSYNCEventRegisterCallback.eventId = NV_EVENT_TYPE_QSYNC;
        nvQSYNCEventRegisterCallback.version = NV_EVENT_REGISTER_CALLBACK_VERSION;

        // Register for Quadro sync events.
        // This is central callback for the events of all Quadro Sync devices in the system.
        status = NvAPI_Event_RegisterCallback(&nvQSYNCEventRegisterCallback, &hEventQSYNC);
        if (NVAPI_OK != status)
        {
            printf("NvAPI_Event_RegisterCallback() failed = 0x%x", status);
            break;
        }

        // Wait for user's input for exit. Log all events until then.
        printf("The console would log Quadro Sync (QSYNC) related events\n");
        printf("(Please wait for events or Press any key to exit...)\n");
        getchar();

        // we've succeeded
        iRet = STATUS_SUCCESS;
    } while (false);

    // Cleanup
    if (hEventQSYNC != NULL)
    {
        status = NvAPI_Event_UnregisterCallback(hEventQSYNC);
        if (status != NVAPI_OK)
        {
            iRet = STATUS_FAIL;
            printf("NvAPI_Event_UnregisterCallback() failed = 0x%x", status);
        }
        hEventQSYNC = NULL;
    }

    return iRet;
}


// This function is hit when a QSYNC event is broadcasted by the display driver.
void __cdecl HandleQSYNCEvent(NV_QSYNC_EVENT_DATA qsyncEventData, void *callbackParam)
{
    switch (qsyncEventData.qsyncEvent)
    {
        case NV_QSYNC_EVENT_SYNC_LOSS:
            printf("\n Received event NV_QSYNC_EVENT_SYNC_LOSS");
            break;
        case NV_QSYNC_EVENT_SYNC_GAIN:
            printf("\n Received event NV_QSYNC_EVENT_SYNC_GAIN");
            break;
        case NV_QSYNC_EVENT_HOUSESYNC_GAIN:
            printf("\n Received event NV_QSYNC_EVENT_HOUSESYNC_GAIN");
            break;
        case NV_QSYNC_EVENT_HOUSESYNC_LOSS:
            printf("\n Received event NV_QSYNC_EVENT_HOUSESYNC_LOSS");
            break;
        case NV_QSYNC_EVENT_RJ45_GAIN:
            printf("\n Received event NV_QSYNC_EVENT_RJ45_GAIN");
            break;
        case NV_QSYNC_EVENT_RJ45_LOSS:
            printf("\n Received event NV_QSYNC_EVENT_RJ45_LOSS");
            break;
        default:
            printf("\n Received Unknown event");
            break;
    }
}