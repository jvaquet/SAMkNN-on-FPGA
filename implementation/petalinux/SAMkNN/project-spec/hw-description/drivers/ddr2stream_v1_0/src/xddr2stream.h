// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef XDDR2STREAM_H
#define XDDR2STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xddr2stream_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
#ifdef SDT
    char *Name;
#else
    u16 DeviceId;
#endif
    u64 Control_BaseAddress;
} XDdr2stream_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XDdr2stream;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XDdr2stream_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XDdr2stream_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XDdr2stream_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XDdr2stream_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
#ifdef SDT
int XDdr2stream_Initialize(XDdr2stream *InstancePtr, UINTPTR BaseAddress);
XDdr2stream_Config* XDdr2stream_LookupConfig(UINTPTR BaseAddress);
#else
int XDdr2stream_Initialize(XDdr2stream *InstancePtr, u16 DeviceId);
XDdr2stream_Config* XDdr2stream_LookupConfig(u16 DeviceId);
#endif
int XDdr2stream_CfgInitialize(XDdr2stream *InstancePtr, XDdr2stream_Config *ConfigPtr);
#else
int XDdr2stream_Initialize(XDdr2stream *InstancePtr, const char* InstanceName);
int XDdr2stream_Release(XDdr2stream *InstancePtr);
#endif

void XDdr2stream_Start(XDdr2stream *InstancePtr);
u32 XDdr2stream_IsDone(XDdr2stream *InstancePtr);
u32 XDdr2stream_IsIdle(XDdr2stream *InstancePtr);
u32 XDdr2stream_IsReady(XDdr2stream *InstancePtr);
void XDdr2stream_EnableAutoRestart(XDdr2stream *InstancePtr);
void XDdr2stream_DisableAutoRestart(XDdr2stream *InstancePtr);

void XDdr2stream_Set_in_data(XDdr2stream *InstancePtr, u64 Data);
u64 XDdr2stream_Get_in_data(XDdr2stream *InstancePtr);
void XDdr2stream_Set_n_elements(XDdr2stream *InstancePtr, u32 Data);
u32 XDdr2stream_Get_n_elements(XDdr2stream *InstancePtr);

void XDdr2stream_InterruptGlobalEnable(XDdr2stream *InstancePtr);
void XDdr2stream_InterruptGlobalDisable(XDdr2stream *InstancePtr);
void XDdr2stream_InterruptEnable(XDdr2stream *InstancePtr, u32 Mask);
void XDdr2stream_InterruptDisable(XDdr2stream *InstancePtr, u32 Mask);
void XDdr2stream_InterruptClear(XDdr2stream *InstancePtr, u32 Mask);
u32 XDdr2stream_InterruptGetEnabled(XDdr2stream *InstancePtr);
u32 XDdr2stream_InterruptGetStatus(XDdr2stream *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
