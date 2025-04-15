// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
/***************************** Include Files *********************************/
#include "xstream2ddr.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XStream2ddr_CfgInitialize(XStream2ddr *InstancePtr, XStream2ddr_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XStream2ddr_Start(XStream2ddr *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_AP_CTRL) & 0x80;
    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XStream2ddr_IsDone(XStream2ddr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XStream2ddr_IsIdle(XStream2ddr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XStream2ddr_IsReady(XStream2ddr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XStream2ddr_EnableAutoRestart(XStream2ddr *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XStream2ddr_DisableAutoRestart(XStream2ddr *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_AP_CTRL, 0);
}

void XStream2ddr_Set_out_data(XStream2ddr *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_OUT_DATA_DATA, (u32)(Data));
    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_OUT_DATA_DATA + 4, (u32)(Data >> 32));
}

u64 XStream2ddr_Get_out_data(XStream2ddr *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_OUT_DATA_DATA);
    Data += (u64)XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_OUT_DATA_DATA + 4) << 32;
    return Data;
}

void XStream2ddr_Set_n_elements(XStream2ddr *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_N_ELEMENTS_DATA, Data);
}

u32 XStream2ddr_Get_n_elements(XStream2ddr *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_N_ELEMENTS_DATA);
    return Data;
}

void XStream2ddr_InterruptGlobalEnable(XStream2ddr *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_GIE, 1);
}

void XStream2ddr_InterruptGlobalDisable(XStream2ddr *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_GIE, 0);
}

void XStream2ddr_InterruptEnable(XStream2ddr *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_IER);
    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_IER, Register | Mask);
}

void XStream2ddr_InterruptDisable(XStream2ddr *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_IER);
    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_IER, Register & (~Mask));
}

void XStream2ddr_InterruptClear(XStream2ddr *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XStream2ddr_WriteReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_ISR, Mask);
}

u32 XStream2ddr_InterruptGetEnabled(XStream2ddr *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_IER);
}

u32 XStream2ddr_InterruptGetStatus(XStream2ddr *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XStream2ddr_ReadReg(InstancePtr->Control_BaseAddress, XSTREAM2DDR_CONTROL_ADDR_ISR);
}

