#include "../TFTApplication/TFTDisplay.h"

using namespace PiDisplays;

enum ErrCodes
{
    MainDisplay = 1,
    TFTScreen = 2,
    Resources = 4,
    Initilization = 8,
    Information = 16,
    NotAccessible = 32
};

TFTDisplay::TFTDisplay()
{
    fbfd_ = 0;
    fbp_ = 0;
}

bool TFTDisplay::IsInitialized()
{
    return display_ && screenResource_ && fbp_ > 0;
}

int TFTDisplay::Initialize(const char* framebuffer)
{
    int callValue;

    bcm_host_init();

    ReleaseResources();

    // # Main Display Initilization and Information
    // =============================================
    display_ = vc_dispmanx_display_open(0);
    if (!display_)
    {
        return MainDisplay | Initilization;
    }

    callValue = vc_dispmanx_display_get_info(display_, &displayInfo_);
    if (callValue)
    {
        return MainDisplay | Information;
    }

    // # Framebuffer File Descriptor for TFT Screen
    // =============================================
    fbfd_ = open(framebuffer, O_RDWR);
    if (fbfd_ == -1)
    {
        vc_dispmanx_display_close(display_);
        return TFTScreen | Initilization;
    }

    if (ioctl(fbfd_, FBIOGET_FSCREENINFO, &finfo_))
    {
        close(fbfd_);
        vc_dispmanx_display_close(display_);
        return TFTScreen | Information;
    }

    if (ioctl(fbfd_, FBIOGET_VSCREENINFO, &vinfo_))
    {
        close(fbfd_);
        vc_dispmanx_display_close(display_);
        return TFTScreen | Information;
    }

    // # Screen Resource
    // ==================
    screenResource_ = vc_dispmanx_resource_create(VC_IMAGE_RGB565, vinfo_.xres, vinfo_.yres, &imagePrt_);
    if (!screenResource_)
    {
        close(fbfd_);
        vc_dispmanx_display_close(display_);
        return Resources | Initilization;
    }

    // # Framebuffer Memory Mapping
    // =============================
    fbp_ = (char*)mmap(0, finfo_.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd_, 0);
    if (fbp_ <= 0)
    {
        close(fbfd_);
        vc_dispmanx_resource_delete(screenResource_);
        vc_dispmanx_display_close(display_);
        return Resources | Initilization;
    }

    vc_dispmanx_rect_set(&rect_, 0, 0, vinfo_.xres, vinfo_.yres);

    return -1;
}

int TFTDisplay::Render()
{
    if (display_ && screenResource_ && fbp_ > 0)
    {
        vc_dispmanx_snapshot(display_, screenResource_, (DISPMANX_TRANSFORM_T)0);
        vc_dispmanx_resource_read_data(screenResource_, &rect_, fbp_, vinfo_.xres * vinfo_.bits_per_pixel / 8);
    }
    else
    {
        return Resources | NotAccessible;
    }

    return -1;
}

int TFTDisplay::ReleaseResources()
{
    if (fbp_ > 0)
        munmap(fbp_, finfo_.smem_len);
    if (fbfd_ >= 0)
        close(fbfd_);
    if (screenResource_)
        vc_dispmanx_resource_delete(screenResource_);
    if (display_)
        vc_dispmanx_display_close(display_);

    return -1;
}

TFTDisplay::~TFTDisplay()
{
    ReleaseResources();
}
