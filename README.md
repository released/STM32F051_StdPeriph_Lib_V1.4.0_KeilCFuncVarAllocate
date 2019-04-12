# STM32F051_StdPeriph_Lib_V1.4.0_KeilCFuncVarAllocate

update @ 2019/04/12

Add function allocate in KEILC 

check Allocate_Test1 and Allocate_Test2 example for different assign method

a. with Allocate_Test1 , function between #pragma arm section , will allocate to specific address

#pragma arm section code="FuncLocation1"

#pragma arm section

b. with Allocate_Test2 , check declaration hw_config.h , to assign function in specific address

__attribute__((section(".ARM.__at_0x8002000")));

