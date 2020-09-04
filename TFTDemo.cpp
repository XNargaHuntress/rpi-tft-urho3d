// DotV - Game
// @author: Laerin Anderson

// NOTE: ALWAYS INCLUDE THIS FIRST!!!!!
// Include the Urho3D Core
#include <Urho3D/Urho3D.h>

// Core
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>

// Input
#include <Urho3D/Input/Input.h>

// ResourceCache
#include <Urho3D/Resource/ResourceCache.h>

// UI
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

// Scene
#include <Urho3D/Scene/Scene.h>

// Audio
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource.h>

#include "TFTDemo.h"
#include "TFTApplication/TFTDisplay.h"
#include <Urho3D/IO/Log.h>

URHO3D_DEFINE_APPLICATION_MAIN(TFTDemo)

using namespace Urho3D;

TFTDemo::TFTDemo(Context* context)
  : Application(context)
{
  // Create the application based on the Urho3D context
  tftDisplay_ = new PiDisplays::TFTDisplay();
  logOnce = false;
  logTwice = false;
}

void TFTDemo::Setup(){
  // Setup the application
  engineParameters_["WindowHeight"] = 240;
  engineParameters_["WindowWidth"] = 320;
  engineParameters_["FullScreen"] = true;
}

void TFTDemo::Start(){
  // Initialize the game stuff and game state manager
  tftDisplay_->Initialize();
  SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(TFTDemo, HandleKeyDown));

  ResourceCache* cache = GetSubsystem<ResourceCache>();

  SharedPtr<Text> helloText(new Text(context_));
  helloText->SetText("Hello World");
  helloText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 8);
  helloText->SetColor(Color(0.0f, 1.0f, 0.0f));
  helloText->SetHorizontalAlignment(HA_CENTER);
  helloText->SetVerticalAlignment(VA_CENTER);

  GetSubsystem<UI>()->GetRoot()->AddChild(helloText);
}

void TFTDemo::Stop(){
  // Free them! Let them roam in the wilds, where they beong!
  engine_->DumpResources(true);
  tftDisplay_->ReleaseResources();
  delete tftDisplay_;
}

int TFTDemo::Run()
{
    // Emscripten-specific: C++ exceptions are turned off by default in -O1 (and above), unless '-s DISABLE_EXCEPTION_CATCHING=0' flag is set
    // Urho3D build configuration uses -O3 (Release), -O2 (RelWithDebInfo), and -O0 (Debug)
    // Thus, the try-catch block below should be optimised out except in Debug build configuration
    try
    {
        Setup();
        if (exitCode_)
            return exitCode_;

        if (!engine_->Initialize(engineParameters_))
        {
            ErrorExit();
            return exitCode_;
        }

        if (!tftDisplay_->IsInitialized())
        {
            if (!logOnce || !logTwice)
            {
                logTwice = logOnce;
                logOnce = true;
                Log::WriteRaw("TFT Not Initialized");
            }
            tftDisplay_->Initialize();
        }

        Start();
        if (exitCode_)
            return exitCode_;

        while (!engine_->IsExiting())
        {
            engine_->RunFrame();
            tftDisplay_->Render();
        }

        Stop();

        return exitCode_;
    }
    catch (std::bad_alloc&)
    {
        ErrorDialog(GetTypeName(), "An out-of-memory error occurred. The application will now exit.");
        return EXIT_FAILURE;
    }
}


void TFTDemo::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    if (key == KEY_ESC)
    {
        engine_->Exit();
    }
}
