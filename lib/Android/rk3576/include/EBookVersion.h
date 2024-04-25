/*
 * Copyright (c) 2021 by Rockchip Electronics Co., Ltd. All Rights Reserved.
 *
 *
 * @Author: Randall Zhuo
 * @Date: 2022-05-09 20:26:55
 * @LastEditors: Randall
 * @LastEditTime: 2022-05-09 20:30:26
 * @Description: TODO
 */

#pragma once

#include "EBookGitVersion.h"

#define STR_HELPER(x)          #x
#define STR(x)                 STR_HELPER(x)

#define EBOOK_VERSION_MAJOR    1
#define EBOOK_VERSION_MINOR    0
#define EBOOK_VERSION_REVISION 0
#define EBOOK_VERSION_SUFFIX   ""

#define EBOOK_CORE_VERSION                                   \
    "libebook version: " STR(EBOOK_VERSION_MAJOR) "." STR(   \
        EBOOK_VERSION_MINOR) "." STR(EBOOK_VERSION_REVISION) \
        EBOOK_VERSION_SUFFIX " (" GIT_REVISION "@" BUILD_TIMESTAMP ")"

#define EBOOK_VERSION                                                       \
    "EBook-" STR(EBOOK_VERSION_MAJOR) "." STR(EBOOK_VERSION_MINOR) "." STR( \
        EBOOK_VERSION_REVISION)
