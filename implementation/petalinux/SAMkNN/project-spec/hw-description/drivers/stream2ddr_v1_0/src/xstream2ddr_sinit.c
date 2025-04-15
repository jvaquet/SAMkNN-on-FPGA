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
#include "xstream2ddr.h"

extern XStream2ddr_Config XStream2ddr_ConfigTable[];

#ifdef SDT
XStream2ddr_Config *XStream2ddr_LookupConfig(UINTPTR BaseAddress) {
	XStream2ddr_Config *ConfigPtr = NULL;

	int Index;

	for (Index = (u32)0x0; XStream2ddr_ConfigTable[Index].Name != NULL; Index++) {
		if (!BaseAddress || XStream2ddr_ConfigTable[Index].Control_BaseAddress == BaseAddress) {
			ConfigPtr = &XStream2ddr_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XStream2ddr_Initialize(XStream2ddr *InstancePtr, UINTPTR BaseAddress) {
	XStream2ddr_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XStream2ddr_LookupConfig(BaseAddress);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XStream2ddr_CfgInitialize(InstancePtr, ConfigPtr);
}
#else
XStream2ddr_Config *XStream2ddr_LookupConfig(u16 DeviceId) {
	XStream2ddr_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XSTREAM2DDR_NUM_INSTANCES; Index++) {
		if (XStream2ddr_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XStream2ddr_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XStream2ddr_Initialize(XStream2ddr *InstancePtr, u16 DeviceId) {
	XStream2ddr_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XStream2ddr_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XStream2ddr_CfgInitialize(InstancePtr, ConfigPtr);
}
#endif

#endif

