// TFT Display - Demo
// @author: Laerin Anderson

#pragma once

#include "TFTApplication/TFTDisplay.h"
#include <Urho3D/Engine/Application.h>

using namespace Urho3D;

class TFTDemo : public Application
{
  // Enable Type Information
  URHO3D_OBJECT(TFTDemo, Application);

public:
    TFTDemo(Context* context);

    // Application lifetime functions
    virtual void Setup();
    virtual void Start();
    virtual void Stop();

    int Run();

protected:
    PiDisplays::TFTDisplay* tftDisplay_;

private:
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
    bool logOnce;
    bool logTwice;
};
