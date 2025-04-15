// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2023.2 (64-bit)
// Tool Version Limit: 2023.10
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
/***************************** Include Files *********************************/
#include "xddr2stream.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XDdr2stream_CfgInitialize(XDdr2stream *InstancePtr, XDdr2stream_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XDdr2stream_Start(XDdr2stream *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_AP_CTRL) & 0x80;
    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XDdr2stream_IsDone(XDdr2stream *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XDdr2stream_IsIdle(XDdr2stream *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XDdr2stream_IsReady(XDdr2stream *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XDdr2stream_EnableAutoRestart(XDdr2stream *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XDdr2stream_DisableAutoRestart(XDdr2stream *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_AP_CTRL, 0);
}

void XDdr2stream_Set_in_data(XDdr2stream *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_IN_DATA_DATA, (u32)(Data));
    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_IN_DATA_DATA + 4, (u32)(Data >> 32));
}

u64 XDdr2stream_Get_in_data(XDdr2stream *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_IN_DATA_DATA);
    Data += (u64)XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_IN_DATA_DATA + 4) << 32;
    return Data;
}

void XDdr2stream_Set_n_elements(XDdr2stream *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_N_ELEMENTS_DATA, Data);
}

u32 XDdr2stream_Get_n_elements(XDdr2stream *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_N_ELEMENTS_DATA);
    return Data;
}

void XDdr2stream_InterruptGlobalEnable(XDdr2stream *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_GIE, 1);
}

void XDdr2stream_InterruptGlobalDisable(XDdr2stream *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_GIE, 0);
}

void XDdr2stream_InterruptEnable(XDdr2stream *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_IER);
    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_IER, Register | Mask);
}

void XDdr2stream_InterruptDisable(XDdr2stream *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_IER);
    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_IER, Register & (~Mask));
}

void XDdr2stream_InterruptClear(XDdr2stream *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XDdr2stream_WriteReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_ISR, Mask);
}

u32 XDdr2stream_InterruptGetEnabled(XDdr2stream *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_IER);
}

u32 XDdr2stream_InterruptGetStatus(XDdr2stream *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XDdr2stream_ReadReg(InstancePtr->Control_BaseAddress, XDDR2STREAM_CONTROL_ADDR_ISR);
}

