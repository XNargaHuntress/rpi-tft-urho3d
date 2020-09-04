// @author: Laerin Anderson
// License: MIT

#pragma once

#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <bcm_host.h>

namespace PiDisplays
{

class TFTDisplay
{
public:
    TFTDisplay();
    ~TFTDisplay();

    // Returns -1 if successful, ErrCodes if not
    // ==========================================
    int Initialize(const char* framebuffer = "/dev/fb1");
    int ReleaseResources();
    int Render();

    bool IsInitialized();
private:
    DISPMANX_DISPLAY_HANDLE_T display_;
    DISPMANX_MODEINFO_T displayInfo_;
    DISPMANX_RESOURCE_HANDLE_T screenResource_;
    VC_IMAGE_TRANSFORM_T transform_;
    uint32_t imagePrt_;
    VC_RECT_T rect_;
    int fbfd_;
    char* fbp_;

    struct fb_var_screeninfo vinfo_;
    struct fb_fix_screeninfo finfo_;
};

}
