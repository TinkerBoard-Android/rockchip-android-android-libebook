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

#include <memory>

#include "EBookType.h"
#include "EBookImage.h"

class EBook;

class EBookApi
{
public:
    /**
     * @brief 获取 EBook 上下文
     */
    EBookApi();

    /**
     * @brief EBook 上下文析构函数
     */
    ~EBookApi();
    /**
     * @brief 禁用引用构造函数
     */
    EBookApi(const EBookApi &) = delete;
    /**
     * @brief 禁用拷贝构造函数
     */
    EBookApi &operator=(const EBookApi &) = delete;

    /**
     * @brief 外部传入的版本信息，用于版本适配，
     *        通常为宏定义EB_VERSION，定义位于 EBookType.h 头文件；
     *
     * @param version_str [IN] 版本号，用于版本校验
     *
     * @return EBookError::
     *         - None, success
     *         - Other, fail
     */
    EBookError Init(const char *version_str);

    /**
     * @brief 获取EBook屏幕信息
     *
     * @param width     [OUT] 屏幕像素宽度
     * @param height    [OUT] 屏幕像素高度
     * @param width_mm  [OUT] 屏幕物理宽度，单位mm
     * @param height_mm [OUT] 屏幕物理高度，单位mm
     *
     * @return EBookError::
     *         - None, success
     *         - Other, fail
     */
    EBookError GetEBookInfo(int *width, int *height, int *width_mm,
                            int *height_mm);

    /**
     * @brief 同步处理模式，EB算法完全执行完成后返回
     *
     * @param int_src   [IN] 输入图像信息
     * @param mode      [IN] 输入的显示模式信息
     * @param out_fence [OUT] 返回完成输入图像转换的Fence标志
     * @return EBookError:
     *         - None, success
     *         - Other, fail
     */
    EBookError Commit(const EBookImageInfo *next_frame, EBookMode mode,
                      int *out_fence);

private:
    std::shared_ptr<EBook> mEBook_;
};