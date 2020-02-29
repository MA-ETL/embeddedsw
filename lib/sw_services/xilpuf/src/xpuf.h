/******************************************************************************
*
* Copyright (C) 2019 - 2020 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMANGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*
*
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpuf.h
* @addtogroup xpuf_apis XilPuf APIs
* @{
* @cond xpuf_internal
* This file contains PUF interface APIs
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- ---------- -------------------------------------------------------
* 1.0   kal  08/01/2019 Initial release
* 1.1   har  01/27/2020 Updated XPufData structure to support helper data
*			programming
*			Added macros for supporting ID only regeneration and
*			black key programming
*
* </pre>
*
* @note
*
* @endcond
*
*******************************************************************************/

#ifndef XPUF_H
#define XPUF_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************** Include Files *********************************/
#include "xil_types.h"
#include "xil_io.h"
#include "xstatus.h"
#include "xpuf_hw.h"
#include "xil_printf.h"

/*************************** Constant Definitions *****************************/
/** @cond xpuf_internal
@{
*/
#if defined XPUF_DEBUG
#define XPUF_DEBUG_GENERAL (1U)
#else
#define XPUF_DEBUG_GENERAL (0U)
#endif

#define xPuf_printf(type,...)	if ((type) == (1U)) {xil_printf (__VA_ARGS__);}

#define XPUF_MAX_SYNDROME_DATA_LEN_IN_WORDS		(350U)
#define XPUF_AES_KEY_LEN_IN_BYTES			(32U)
#define XPUF_AES_KEY_IV_LEN_IN_BYTES			(12U)
#define XPUF_4K_PUF_SYN_LEN_IN_WORDS			(140U)
#define XPUF_4K_PUF_SYN_LEN_IN_BYTES			(560U)
#define XPUF_EFUSE_TRIM_SYN_DATA_IN_WORDS		(127U)
#define XPUF_12K_PUF_SYN_LEN_IN_WORDS			(350U)
#define XPUF_12K_PUF_SYN_LEN_IN_BYTES			(1400U)
#define XPUF_SHUTTER_VALUE				(0x1000040U)
#define XPUF_ID_LENGTH					(0x8U)
#define XPUF_WORD_LENGTH				(0x4U)

#define XPUF_SYNDROME_MODE_4K				(0x0U)
#define XPUF_SYNDROME_MODE_12K				(0x1U)

#define XPUF_READ_FROM_EFUSE_CACHE			(0x1U)
#define XPUF_READ_FROM_RAM				(0x2U)
#define XPUF_READ_FROM_BBRAM				(0x3U)

#define XPUF_WRITE_INTO_EFUSE				(0x1U)
#define XPUF_WRITE_INTO_RAM				(0x2U)
#define XPUF_WRITE_INTO_BBRAM				(0x3U)

#define XPUF_REGISTRATION				(0X1U)
#define XPUF_REGEN_ON_DEMAND				(0x2U)
#define XPUF_REGEN_ID_ONLY				(0x3U)

#define XPUF_EFUSE_TRIM_MASK				(0xFFFFF000U)
#define XPUF_LAST_WORD_OFFSET				(126U)
#define XPUF_LAST_WORD_MASK				(0xFFFFFFF0U)

/* Key registration time error codes */
#define XPUF_ERROR_INVALID_PARAM			((u32)0x02)
#define XPUF_ERROR_INVALID_SYNDROME_MODE		((u32)0x03)
#define XPUF_ERROR_SYNDROME_WORD_WAIT_TIMEOUT		((u32)0x04)
#define XPUF_ERROR_SYNDROME_DATA_OVERFLOW		((u32)0x05)
#define XPUF_ERROR_SYNDROME_DATA_UNDERFLOW		((u32)0x06)
#define XPUF_ERROR_PUF_DONE_WAIT_TIMEOUT		((u32)0x07)
#define XPUF_ERROR_REGISTRATION_INVALID			((u32)0x08)

/* Key regeneration time error codes */
#define XPUF_ERROR_CHASH_NOT_PROGRAMMED			((u32)0x10)
#define XPUF_ERROR_PUF_STATUS_DONE_TIMEOUT		((u32)0x11)
#define XPUF_ERROR_INVALID_REGENERATION_TYPE		((u32)0x12)
#define XPUF_ERROR_INVALID_PUF_OPERATION		((u32)0x13)
#define XPUF_ERROR_REGENERATION_INVALID			((u32)0x14)
#define XPUF_ERROR_REGEN_PUF_HD_INVALID			((u32)0x15)
/***************************** Type Definitions *******************************/

typedef struct {
	u8 RegMode;		/* PUF Registration Mode 4K/12K*/
	u8 PufOperation;	/* PUF Registration/ Regeneration On Demand/
				 or ID only regeneration) */
	u8 ReadOption;		/* Read Syndrome data from eFuse Cache/DDR */
	u8 WriteOption;	/*Write into eFuse/BBRAM/DDR */
	u32 ShutterValue;
	u32 SyndromeData[XPUF_MAX_SYNDROME_DATA_LEN_IN_WORDS];
	u32 Chash;
	u32 Aux;
	u32 PufID[XPUF_ID_LENGTH];
	u32 SyndromeAddr;
	u32 EfuseSynData[XPUF_EFUSE_TRIM_SYN_DATA_IN_WORDS];
				 /*< Trimmed data to be	written in efuse */
}XPuf_Data;

/** @}
@endcond */

/*************************** Function Prototypes ******************************/

u32 XPuf_Registration(XPuf_Data *PufData);
u32 XPuf_Regeneration(XPuf_Data *PufData);
void XPuf_GenerateFuseFormat(XPuf_Data *PufData);

#ifdef __cplusplus
}
#endif

#endif  /* XPUF_H */
/**@}*/
