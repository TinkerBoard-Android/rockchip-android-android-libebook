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
#include "allocator/EBookAllocator.h"
#include "utils/autofd.h"
#include "buffer/drm_utils.h"
#include <mutex>

class EBookBuffer
{
public:
    EBookBuffer(int w, int h, int stride, uint32_t fourcc_format,
                const char* name);
    EBookBuffer(int w, int h, int stride, uint32_t fourcc_format,
                uint64_t usage, const char* name);
    EBookBuffer(int w, int h, int stride, int h_stride, uint32_t fourcc_format,
                uint64_t usage, const char* name);
    ~EBookBuffer();
    int Init();
    bool initCheck();
    uint64_t GetId();
    int GetFd();
    std::string GetName();
    int GetWidth();
    int GetHeight();
    int GetFormat();
    int GetStride();
    int GetHeightStride();
    int GetByteStride();
    int GetSize();
    int GetUsage();
    uint32_t GetFourccFormat();
    uint64_t GetModifier();
    uint64_t GetBufferId();
    EBookUniqueFd GetFinishFence();
    int SetFinishFence(int fence);
    int WaitFinishFence();
    EBookUniqueFd GetReleaseFence();
    EBookOutputFd ReleaseFenceOutput();
    int WaitReleaseFence();
    int Lock(void** vaddr);
    int UnLock(void* vaddr);
    int DumpData();
    int FillFromFile(const char* file_name);

private:
    uint64_t uId;
    // BufferInfo
    int iFd_;
    int iWidth_;
    int iHeight_;
    int iStride_;
    int iHeightStride_;
    uint32_t uFourccFormat_;
    uint32_t iByteStride_;
    int iSize_;
    uint64_t iUsage_;
    uint64_t uBufferId_;
    // Fence info
    EBookUniqueFd iFinishFence_;
    EBookUniqueFd iReleaseFence_;
    // Init flags
    bool bInit_;
    std::string sName_;
    EBookAllocator* mAllocator_;
    mutable std::mutex mtx_;
};
