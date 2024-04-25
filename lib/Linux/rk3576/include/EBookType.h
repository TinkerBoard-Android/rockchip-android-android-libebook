/****************************************************************************
 *
 *    Copyright (c) 2023 by Rockchip Corp.  All rights reserved.
 *
 *    The material in this file is confidential and contains trade secrets
 *    of Rockchip Corporation. This is proprietary information owned by
 *    Rockchip Corporation. No part of this work may be disclosed,
 *    reproduced, copied, transmitted, or used in any way for any purpose,
 *    without the express written permission of Rockchip Corporation.
 *
 *****************************************************************************/
#pragma once

#include <cwchar>
#include "EBookVersion.h"
// Verison info
#define EBOOK_MAGIC 0x83991906

#ifdef __ANDROID__
// Android Property
#define EBOOK_VERSION_NAME "vendor.ebook.version"

// Debug Property.
#define EBOOK_DEBUG_NAME "vendor.ebook.log"
#endif

enum EBookError
{
    None = 0,     /* 无错误，正常 */
    BadVersion,   /* 版本错误     */
    BadStage,     /* 流程错误     */
    BadParameter, /* 参数错误     */
    BadInit,      /* 初始化错误   */
    BadOperate,   /* 操作错误     */
};

enum EBookDatapace
{
    EB_DATASPACE_UNKNOWN           = 0,
    EB_DATASPACE_SHIFT             = 16,
    EB_DATASPACE_UNSPECIFIED       = 0 << EB_DATASPACE_SHIFT,
    EB_DATASPACE_BT601             = 1 << EB_DATASPACE_SHIFT,
    EB_DATASPACE_BT709             = 2 << EB_DATASPACE_SHIFT,
    EB_DATASPACE_BT2020            = 3 << EB_DATASPACE_SHIFT,
    EB_DATASPACE_RANGE_SHIFT       = 27,
    EB_DATASPACE_RANGE_UNSPECIFIED = 0 << EB_DATASPACE_RANGE_SHIFT,
    EB_DATASPACE_LIMITED           = 1 << EB_DATASPACE_RANGE_SHIFT,
    EB_DATASPACE_FULL              = 2 << EB_DATASPACE_RANGE_SHIFT,
    EB_DATAPACE_BT601_LIMIT_RANGE = (EB_DATASPACE_BT601 | EB_DATASPACE_LIMITED),
    EB_DATAPACE_BT601_FULL_RANGE  = (EB_DATASPACE_BT601 | EB_DATASPACE_FULL),
    EB_DATAPACE_BT709_LIMIT_RANGE = (EB_DATASPACE_BT709 | EB_DATASPACE_LIMITED),
    EB_DATAPACE_BT709_FULL_RANGE  = (EB_DATASPACE_BT709 | EB_DATASPACE_FULL)
};

enum EBookBufferMask
{
    EB_BUFFER_NONE  = 0,     /* 无特殊标志 */
    EB_AFBC_FORMATE = 1 << 1 /* AFBC压缩格式 */
};

enum EBookMode
{
    EBOOK_NULL       = -1,
    EBOOK_AUTO       = 0,
    EBOOK_OVERLAY    = 1,
    EBOOK_FULL_GC16  = 2,
    EBOOK_FULL_GL16  = 3,
    EBOOK_FULL_GLR16 = 4,
    EBOOK_FULL_GLD16 = 5,
    EBOOK_FULL_GCC16 = 6,
    EBOOK_PART_GC16  = 7,
    EBOOK_PART_GL16  = 8,
    EBOOK_PART_GLR16 = 9,
    EBOOK_PART_GLD16 = 10,
    EBOOK_PART_GCC16 = 11,
    EBOOK_A2         = 12,
    EBOOK_A2_DITHER  = 13,
    EBOOK_DU         = 14,
    EBOOK_DU4        = 15,
    EBOOK_A2_ENTER   = 16,
    EBOOK_RESET      = 17,
    EBOOK_SUSPEND    = 18,
    EBOOK_RESUME     = 19,
    EBOOK_POWER_OFF  = 20,
    EBOOK_FORCE_FULL = 21,
    EBOOK_AUTO_DU    = 22,
    EBOOK_AUTO_DU4   = 23,
};

struct EBookVersion
{
    int iMajor_;      /* 主版本 */
    int iMinor_;      /* 副版本 */
    int iPatchLevel_; /* 补丁版本 */
};
