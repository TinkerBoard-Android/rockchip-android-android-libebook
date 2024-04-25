/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstring>
#include <inttypes.h>
#include <stdint.h>
#include <sys/time.h>

#include "buffer/drm_fourcc.h"
#include <sw-sync/sw_sync.h>
#include <sw-sync/sync/sync.h>
#include <unistd.h>

#include "EBookApi.h"
#include "buffer/EBookBuffer.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static char optstr[] = "i:m:f:c:H:";

static void usage(char *name) {
  fprintf(stderr, "usage: %s [-imfHc]\n", name);
  fprintf(stderr,
          "usage: %s -i 1280x720+0+0:1280x720@NV12 -m "
          "+mode=1 -f /data/1280x720-nv12.yuv -c 100\n",
          name);
  fprintf(stderr, "\n Query options:\n\n");
  fprintf(stderr, "\t-i\t<crop_w>x<crop_h>+<x>+<y>:<stride_w>x<stride_h>@"
                  "<format>[#afbc]\n");
  fprintf(stderr, "\t-m\t[+mode=1]\n");
  fprintf(stderr, "\t-f\t<input_image_path>\n");
  fprintf(stderr, "\t-c\t<run_cnt> default cnt=1\n");
  fprintf(stderr, "\t-H\thelp\n");
  exit(0);
}

/*-------------------------------------------
                  Functions
-------------------------------------------*/
// static inline int64_t getCurrentTimeUs() {
//   struct timeval tv;
//   gettimeofday(&tv, NULL);
//   return tv.tv_sec * 1000000 + tv.tv_usec;
// }

struct image_arg {
  int x;
  int y;
  int crop_w;
  int crop_h;
  int stride_w;
  int stride_h;
  char format_str[5]; /* need to leave room for terminating \0 */
  uint32_t fourcc_format;
  bool afbc;
  bool has_path = false;
  char image_path[80];
};
struct mode_arg {
  EBookMode ebook_mode;
  int run_cnt;
};

struct util_format_info {
  uint32_t format;
  const char *name;
};

static const struct util_format_info format_info[] = {
    /* Indexed */
    {DRM_FORMAT_C8, "C8"},
    /* YUV packed */
    {DRM_FORMAT_UYVY, "UYVY"},
    {DRM_FORMAT_VYUY, "VYUY"},
    {DRM_FORMAT_YUYV, "YUYV"},
    {DRM_FORMAT_YVYU, "YVYU"},
    /* YUV semi-planar */
    {DRM_FORMAT_NV12, "NV12"},
    {DRM_FORMAT_NV21, "NV21"},
    {DRM_FORMAT_NV16, "NV16"},
    {DRM_FORMAT_NV61, "NV61"},
    // { DRM_FORMAT_NV15, "NV15"},
    /* YUV planar */
    {DRM_FORMAT_YUV420, "YU12"},
    {DRM_FORMAT_YVU420, "YV12"},
    /* RGB16 */
    {DRM_FORMAT_ARGB4444, "AR12"},
    {DRM_FORMAT_XRGB4444, "XR12"},
    {DRM_FORMAT_ABGR4444, "AB12"},
    {DRM_FORMAT_XBGR4444, "XB12"},
    {DRM_FORMAT_RGBA4444, "RA12"},
    {DRM_FORMAT_RGBX4444, "RX12"},
    {DRM_FORMAT_BGRA4444, "BA12"},
    {DRM_FORMAT_BGRX4444, "BX12"},
    {DRM_FORMAT_ARGB1555, "AR15"},
    {DRM_FORMAT_XRGB1555, "XR15"},
    {DRM_FORMAT_ABGR1555, "AB15"},
    {DRM_FORMAT_XBGR1555, "XB15"},
    {DRM_FORMAT_RGBA5551, "RA15"},
    {DRM_FORMAT_RGBX5551, "RX15"},
    {DRM_FORMAT_BGRA5551, "BA15"},
    {DRM_FORMAT_BGRX5551, "BX15"},
    {DRM_FORMAT_RGB565, "RG16"},
    {DRM_FORMAT_BGR565, "BG16"},
    /* RGB24 */
    {DRM_FORMAT_BGR888, "BG24"},
    {DRM_FORMAT_RGB888, "RG24"},
    /* RGB32 */
    {DRM_FORMAT_ARGB8888, "AR24"},
    {DRM_FORMAT_XRGB8888, "XR24"},
    {DRM_FORMAT_ABGR8888, "AB24"},
    {DRM_FORMAT_XBGR8888, "XB24"},
    {DRM_FORMAT_RGBA8888, "RA24"},
    {DRM_FORMAT_RGBX8888, "RX24"},
    {DRM_FORMAT_BGRA8888, "BA24"},
    {DRM_FORMAT_BGRX8888, "BX24"},
    {DRM_FORMAT_ARGB2101010, "AR30"},
    {DRM_FORMAT_XRGB2101010, "XR30"},
    {DRM_FORMAT_ABGR2101010, "AB30"},
    {DRM_FORMAT_XBGR2101010, "XB30"},
    {DRM_FORMAT_RGBA1010102, "RA30"},
    {DRM_FORMAT_RGBX1010102, "RX30"},
    {DRM_FORMAT_BGRA1010102, "BA30"},
    {DRM_FORMAT_BGRX1010102, "BX30"},
    // { DRM_FORMAT_XRGB16161616F, "XR4H"},
    // { DRM_FORMAT_XBGR16161616F, "XB4H"},
    // { DRM_FORMAT_ARGB16161616F, "AR4H"},
    // { DRM_FORMAT_ABGR16161616F, "AB4H"},
};

uint32_t util_format_fourcc(const char *name) {
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE(format_info); i++)
    if (!strcmp(format_info[i].name, name))
      return format_info[i].format;

  return 0;
}

int parse_input_image_info(struct image_arg *pipe, const char *arg) {
  /* Parse the input image info. */
  char *end;
  pipe->crop_w = strtoul(arg, &end, 10);
  if (*end != 'x')
    return -EINVAL;

  arg = end + 1;
  pipe->crop_h = strtoul(arg, &end, 10);
  if (*end != '+')
    return -EINVAL;

  arg = end + 1;
  pipe->x = strtoul(arg, &end, 10);
  if (*end != '+')
    return -EINVAL;

  arg = end + 1;
  pipe->y = strtoul(arg, &end, 10);
  if (*end != ':')
    return -EINVAL;

  arg = end + 1;
  pipe->stride_w = strtoul(arg, &end, 10);
  if (*end != 'x')
    return -EINVAL;

  arg = end + 1;
  pipe->stride_h = strtoul(arg, &end, 10);
  if (*end != '@')
    return -EINVAL;

  if (*end == '@') {
    strncpy(pipe->format_str, end + 1, 4);
    pipe->format_str[4] = '\0';
  } else {
    strcpy(pipe->format_str, "NV12");
  }

  pipe->fourcc_format = util_format_fourcc(pipe->format_str);
  if (pipe->fourcc_format == 0) {
    fprintf(stderr, "unknown format %s\n", pipe->format_str);
    return -EINVAL;
  }

  arg = end + 5;
  if (*arg == '#') {
    if (!strcmp(arg, "#afbc")) {
      pipe->afbc = true;
    }
  }

  return 0;
}

int parse_input_image_path(struct image_arg *pipe, const char *arg) {
  if (arg == NULL)
    return -EINVAL;

  if (strlen(arg) > sizeof(pipe->image_path)) {
    fprintf(stderr, "%s is too long, max is %zu\n", arg,
            strnlen(pipe->image_path, 80));
    return -EINVAL;
  }
  strncpy(pipe->image_path, arg, strlen(arg));
  pipe->has_path = true;
  return 0;
}

int parse_ebook_mode(struct mode_arg *pipe, const char *arg) {
  char *end;

  if (*arg != '+')
    return -EINVAL;

  arg++;

  if (*arg == 'm') {
    if (!strncmp(arg, "mode=", 5)) {
      arg = arg + 5;
      pipe->ebook_mode = static_cast<EBookMode>(strtoul(arg, &end, 10));
    }
  }

  return 0;
}

// 解析输入参数
int parse_argv(int argc, char **argv, image_arg *input_image, mode_arg *mode) {
  int c;
  unsigned int args = 0;
  opterr = 0;
  bool exit = false;
  mode->run_cnt = 1;
  while ((c = getopt(argc, argv, optstr)) != -1) {
    args++;
    switch (c) {
    case 'i':
      if (parse_input_image_info(input_image, optarg) < 0) {
        fprintf(stderr, "parse_input_image_info fail!\n");
        exit = true;
      }
      break;
    case 'm':
      if (parse_ebook_mode(mode, optarg) < 0) {
        fprintf(stderr, "parse_sr_mode fail!\n");
        exit = true;
      }
      break;
    case 'f':
      if (parse_input_image_path(input_image, optarg) < 0) {
        fprintf(stderr, "parse_input_image_path fail!\n");
        exit = true;
      }
      break;
    case 'c':
      mode->run_cnt = atoi(optarg);
      break;
    case 'H':
      exit = true;
      break;
    default:
      exit = true;
      break;
    }
  }
  if (args == 0 || exit) {
    fprintf(stderr,
            "cmd_parse: crop[%d,%d,%d,%d] image[%d,%d,%s] afbc=%d path=%s "
            "mode=%d\n",
            input_image->x, input_image->y, input_image->crop_w,
            input_image->crop_h, input_image->stride_w, input_image->stride_h,
            input_image->format_str, input_image->afbc, input_image->image_path,
            mode->ebook_mode);
    usage(argv[0]);
    return -1;
  }

  fprintf(stderr,
          "cmd_parse: crop[%d,%d,%d,%d] image[%d,%d,%s] afbc=%d path=%s "
          "mode=%d \n",
          input_image->x, input_image->y, input_image->crop_w,
          input_image->crop_h, input_image->stride_w, input_image->stride_h,
          input_image->format_str, input_image->afbc, input_image->image_path,
          mode->ebook_mode);
  return 0;
}

int main(int argc, char **argv) {
  image_arg input_image;
  mode_arg mode;
  memset(&input_image, 0x00, sizeof(input_image));
  memset(&mode, 0x00, sizeof(mode));
  if (parse_argv(argc, argv, &input_image, &mode)) {
    return -1;
  }

  // 1. 获得SR实例
  std::shared_ptr<EBookApi> ebook_api =
      std::shared_ptr<EBookApi>(new EBookApi());
  if (ebook_api == NULL) {
    fprintf(stderr, "EBookApi init fail check\n");
    return -1;
  }

  EBookError error = EBookError::None;
  // 2. 初始化
  error = ebook_api->Init(EBOOK_VERSION);
  if (error != EBookError::None) {
    fprintf(stderr, "EBook Init fail.\n");
    return -1;
  }

  int resolution_width = 0;
  int resolution_height = 0;
  int physical_width_mm = 0;
  int physical_height_mm = 0;
  error = ebook_api->GetEBookInfo(&resolution_width, &resolution_height,
                                  &physical_width_mm, &physical_height_mm);
  if (error != EBookError::None) {
    fprintf(stderr, "EBook Init fail.\n");
    return -1;
  }

  fprintf(stderr,
          "EBookInfo: resolution : width=%d height=%d. physical: width=%d mm"
          "height=%d mm\n",
          resolution_width, resolution_height, physical_width_mm,
          physical_height_mm);

  // 3. 申请src内存
  EBookBuffer *src_buffer = new EBookBuffer(
      input_image.stride_w, input_image.stride_h, input_image.stride_w,
      input_image.fourcc_format, "EBookSrc");
  if (src_buffer->Init()) {
    fprintf(stderr, "Alloc src buffer fail,  check error : %s\n",
            strerror(errno));
    return -1;
  }

  // 4. 从外部获取源图像数据，并安装申请格式打印输出
  if (input_image.has_path == true) {
    src_buffer->FillFromFile(input_image.image_path);
    src_buffer->DumpData();
  }

  // 5. 设置源图像参数
  EBookImageInfo src;
  src.mBufferInfo_.iFd_ = src_buffer->GetFd();
  src.mBufferInfo_.iWidth_ = src_buffer->GetWidth();
  src.mBufferInfo_.iHeight_ = src_buffer->GetHeight();
  src.mBufferInfo_.iFormat_ = src_buffer->GetFourccFormat();
  src.mBufferInfo_.iStride_ = src_buffer->GetStride();
  src.mBufferInfo_.iHeightStride_ = src_buffer->GetHeightStride();
  src.mBufferInfo_.uBufferId_ = src_buffer->GetBufferId();
  src.mBufferInfo_.iSize_ = src_buffer->GetSize();

  src.mCrop_.iLeft_ = input_image.x;
  src.mCrop_.iTop_ = input_image.y;
  src.mCrop_.iRight_ = input_image.x + input_image.crop_w;
  src.mCrop_.iBottom_ = input_image.y + input_image.crop_h;

  if (input_image.afbc) {
    src.mBufferInfo_.uMask_ = EBookBufferMask::EB_AFBC_FORMATE;
  }

  // 6. Commit
  int finish_fence = -1;
  error = ebook_api->Commit(&src, mode.ebook_mode, &finish_fence);
  if (error != EBookError::None) {
    fprintf(stderr, "EBook RunAsync fail\n");
    return -1;
  }

  if (finish_fence > 0) {
    int ret = sync_wait(finish_fence, 1500);
    if (ret) {
      fprintf(stderr, "Failed to wait for RGA finish fence %d/%d 1500ms",
              finish_fence, ret);
      return -1;
    }
  }

  usleep(2000 * 1000);

  delete src_buffer;
  return 0;
}
