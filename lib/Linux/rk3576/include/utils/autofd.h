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

#include <unistd.h>

class EBookUniqueFd
{
public:
    EBookUniqueFd() = default;
    EBookUniqueFd(int fd) : fd_(fd) {}
    EBookUniqueFd(const EBookUniqueFd &rhs) { fd_ = rhs.Dup(); }

    EBookUniqueFd(EBookUniqueFd &&rhs)
    {
        fd_     = rhs.fd_;
        rhs.fd_ = -1;
    }

    EBookUniqueFd &operator=(EBookUniqueFd &&rhs)
    {
        Set(rhs.Release());
        return *this;
    }

    ~EBookUniqueFd()
    {
        if (fd_ > 0) close(fd_);
    }

    int Release()
    {
        int old_fd = fd_;
        fd_        = -1;
        return old_fd;
    }

    int Set(int fd)
    {
        if (fd_ > 0) close(fd_);
        fd_ = fd;
        return fd_;
    }

    void Close()
    {
        if (fd_ > 0) close(fd_);
        fd_ = -1;
    }

    int get() const { return fd_; }

    int Dup() const { return dup(fd_); }

    int *get_ptr() { return &fd_; }

private:
    int fd_ = -1;
};

struct EBookOutputFd
{
    EBookOutputFd() = default;
    EBookOutputFd(int *fd) : fd_(fd) {}
    EBookOutputFd(EBookOutputFd &&rhs)
    {
        fd_     = rhs.fd_;
        rhs.fd_ = NULL;
    }

    EBookOutputFd &operator=(EBookOutputFd &&rhs)
    {
        fd_     = rhs.fd_;
        rhs.fd_ = NULL;
        return *this;
    }

    int Set(int fd)
    {
        if (*fd_ >= 0) close(*fd_);
        *fd_ = fd;
        return fd;
    }

    int get() { return *fd_; }

    operator bool() const { return fd_ != NULL; }

private:
    int *fd_ = NULL;
};
