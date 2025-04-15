// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef XSTREAM2DDR_H
#define XSTREAM2DDR_H

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
#include "xstream2ddr_hw.h"

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
} XStream2ddr_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XStream2ddr;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XStream2ddr_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XStream2ddr_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XStream2ddr_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XStream2ddr_ReadReg(BaseAddress, RegOffset) \
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
int XStream2ddr_Initialize(XStream2ddr *InstancePtr, UINTPTR BaseAddress);
XStream2ddr_Config* XStream2ddr_LookupConfig(UINTPTR BaseAddress);
#else
int XStream2ddr_Initialize(XStream2ddr *InstancePtr, u16 DeviceId);
XStream2ddr_Config* XStream2ddr_LookupConfig(u16 DeviceId);
#endif
int XStream2ddr_CfgInitialize(XStream2ddr *InstancePtr, XStream2ddr_Config *ConfigPtr);
#else
int XStream2ddr_Initialize(XStream2ddr *InstancePtr, const char* InstanceName);
int XStream2ddr_Release(XStream2ddr *InstancePtr);
#endif

void XStream2ddr_Start(XStream2ddr *InstancePtr);
u32 XStream2ddr_IsDone(XStream2ddr *InstancePtr);
u32 XStream2ddr_IsIdle(XStream2ddr *InstancePtr);
u32 XStream2ddr_IsReady(XStream2ddr *InstancePtr);
void XStream2ddr_EnableAutoRestart(XStream2ddr *InstancePtr);
void XStream2ddr_DisableAutoRestart(XStream2ddr *InstancePtr);

void XStream2ddr_Set_out_data(XStream2ddr *InstancePtr, u64 Data);
u64 XStream2ddr_Get_out_data(XStream2ddr *InstancePtr);
void XStream2ddr_Set_n_elements(XStream2ddr *InstancePtr, u32 Data);
u32 XStream2ddr_Get_n_elements(XStream2ddr *InstancePtr);

void XStream2ddr_InterruptGlobalEnable(XStream2ddr *InstancePtr);
void XStream2ddr_InterruptGlobalDisable(XStream2ddr *InstancePtr);
void XStream2ddr_InterruptEnable(XStream2ddr *InstancePtr, u32 Mask);
void XStream2ddr_InterruptDisable(XStream2ddr *InstancePtr, u32 Mask);
void XStream2ddr_InterruptClear(XStream2ddr *InstancePtr, u32 Mask);
u32 XStream2ddr_InterruptGetEnabled(XStream2ddr *InstancePtr);
u32 XStream2ddr_InterruptGetStatus(XStream2ddr *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
