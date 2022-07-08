/******************************************************************************
* Copyright (c) 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xloader_plat_secure.c
*
* This file contains the versal_net specific secure code related to PDI image
* loading.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- --------   -------------------------------------------------------
* 1.00  bm   07/06/2022 Initial release
*       kpt  07/05/2022 Added support to update KAT status
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xplmi_config.h"
#ifndef PLM_SECURE_EXCLUDE
#include "xloader_auth_enc.h"

/************************** Constant Definitions *****************************/
#define XLOADER_EFUSE_OBFUS_KEY		(0xA5C3C5A7U) /* eFuse Obfuscated Key */
#define XLOADER_BBRAM_OBFUS_KEY		(0x3A5C3C57U) /* BBRAM Obfuscated Key */
#define XLOADER_BH_OBFUS_KEY		(0xA35C7CA5U) /*Boot Header Obfuscated Key */

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
 * @brief	This function updates KEK red key availability status from
 * boot header.
 *
 * @param	PdiPtr is pointer to the XilPdi instance.
 *
 * @return	None.
 *
 ******************************************************************************/
void XLoader_UpdateKekSrc(XilPdi *PdiPtr)
{
	PdiPtr->KekStatus = 0x0U;

	XPlmi_Printf(DEBUG_INFO, "Identifying KEK's corresponding RED "
			"key availability status\n\r");
	switch(PdiPtr->MetaHdr.BootHdrPtr->EncStatus) {
		case XLOADER_BH_BLK_KEY:
		case XLOADER_BH_OBFUS_KEY:
			PdiPtr->KekStatus = XLOADER_BHDR_RED_KEY;
			break;
		case XLOADER_BBRAM_BLK_KEY:
		case XLOADER_BBRAM_OBFUS_KEY:
			PdiPtr->KekStatus = XLOADER_BBRAM_RED_KEY;
			break;
		case XLOADER_EFUSE_BLK_KEY:
		case XLOADER_EFUSE_OBFUS_KEY:
			PdiPtr->KekStatus = XLOADER_EFUSE_RED_KEY;
			break;
		default:
			/* No KEK is available for PLM */
			break;
	}
	XPlmi_Printf(DEBUG_DETAILED, "KEK red key available after "
			"for PLM %x\n\r", PdiPtr->KekStatus);
}

/*****************************************************************************/
/**
 * @brief	This function provides Obfuscated Aes Key source
 *
 * @param	PdiKeySrc is the Key source given in Pdi
 * @param	KekStatus is the current KekStatus
 * @param	KeySrcPtr is the pointer to the calculated KeySrc
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 ******************************************************************************/
int XLoader_AesObfusKeySelect(u32 PdiKeySrc, u32 KekStatus, void *KeySrcPtr)
{
	int Status = XST_FAILURE;
	XSecure_AesKeySrc *KeySrc = (XSecure_AesKeySrc *)KeySrcPtr;

	switch (PdiKeySrc) {
		case XLOADER_EFUSE_OBFUS_KEY:
			if (((KekStatus) & XLOADER_EFUSE_RED_KEY) == XLOADER_EFUSE_RED_KEY) {
				Status = XST_SUCCESS;
				*KeySrc = XSECURE_AES_EFUSE_RED_KEY;
			}
			break;
		case XLOADER_BBRAM_OBFUS_KEY:
			if (((KekStatus) & XLOADER_BBRAM_RED_KEY) == XLOADER_BBRAM_RED_KEY) {
				*KeySrc = XSECURE_AES_BBRAM_RED_KEY;
				Status = XST_SUCCESS;
			}
			break;
		case XLOADER_BH_OBFUS_KEY:
			if (((KekStatus) & XLOADER_BHDR_RED_KEY) == XLOADER_BHDR_RED_KEY) {
				*KeySrc = XSECURE_AES_BH_RED_KEY;
				Status = XST_SUCCESS;
			}
			break;
		default:
			Status  = XLoader_UpdateMinorErr(XLOADER_SEC_AES_KEK_DEC, Status);
			break;
	}

	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function return FIPS mode status
 *
 * @return
 * 		TRUE  if FIPS mode is enabled
 * 		FALSE if FIPS mode is not enabled
 *
 * @note
 *     Fips_Mode[24:23] is used to control scan clear execution as a part of
 *     secure lockdown
 *
 *****************************************************************************/
u8 XLoader_IsFipsModeEn(void) {
	u8 FipsModeEn = (u8)(XPlmi_In32(XLOADER_EFUSE_CACHE_FIPS) >>
					XLOADER_EFUSE_FIPS_MODE_SHIFT);

	return (FipsModeEn != 0U) ? (u8)TRUE: (u8)FALSE;
}

/*****************************************************************************/
/**
 * @brief	This function updates the KAT status
 *
 * @param	SecurePtr is pointer to the XLoader_SecureParams instance
 * @param	PlmKatMask is the mask of the KAT that is going to run
 *
 * @return	None
 *
 *****************************************************************************/
void XLoader_UpdateKatStatus(XLoader_SecureParams *SecurePtr, u32 PlmKatMask) {
	u8 FipsModeEn = XLoader_IsFipsModeEn();

	if (FipsModeEn == TRUE) {
		if (PlmKatMask != 0U) {
			/* Rerun KAT for every image */
			XLoader_ClearKatStatus(&SecurePtr->PdiPtr->PlmKatStatus, PlmKatMask);
		}
		else {
			SecurePtr->PdiPtr->PlmKatStatus = 0U;
		}
	}
	else {
		if (SecurePtr->PdiPtr->PdiType == XLOADER_PDI_TYPE_PARTIAL) {
			XLoader_UpdatePpdiKatStatus(SecurePtr, PlmKatMask);
		}
	}
}

#endif /* END OF PLM_SECURE_EXCLUDE */
