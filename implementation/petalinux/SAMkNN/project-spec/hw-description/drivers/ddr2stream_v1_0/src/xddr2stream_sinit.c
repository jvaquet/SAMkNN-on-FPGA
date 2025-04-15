// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#ifdef SDT
#include "xparameters.h"
#endif
#include "xddr2stream.h"

extern XDdr2stream_Config XDdr2stream_ConfigTable[];

#ifdef SDT
XDdr2stream_Config *XDdr2stream_LookupConfig(UINTPTR BaseAddress) {
	XDdr2stream_Config *ConfigPtr = NULL;

	int Index;

	for (Index = (u32)0x0; XDdr2stream_ConfigTable[Index].Name != NULL; Index++) {
		if (!BaseAddress || XDdr2stream_ConfigTable[Index].Control_BaseAddress == BaseAddress) {
			ConfigPtr = &XDdr2stream_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XDdr2stream_Initialize(XDdr2stream *InstancePtr, UINTPTR BaseAddress) {
	XDdr2stream_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XDdr2stream_LookupConfig(BaseAddress);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XDdr2stream_CfgInitialize(InstancePtr, ConfigPtr);
}
#else
XDdr2stream_Config *XDdr2stream_LookupConfig(u16 DeviceId) {
	XDdr2stream_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XDDR2STREAM_NUM_INSTANCES; Index++) {
		if (XDdr2stream_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XDdr2stream_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XDdr2stream_Initialize(XDdr2stream *InstancePtr, u16 DeviceId) {
	XDdr2stream_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XDdr2stream_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XDdr2stream_CfgInitialize(InstancePtr, ConfigPtr);
}
#endif

#endif

