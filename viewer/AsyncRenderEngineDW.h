// ======================================================================== //
// Copyright 2009-2016 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

// std
#include <atomic>
#include <thread>
#include <vector>

// ospcommon
#include <ospcommon/box.h>

// ospray::cpp
#include "ospray/ospray_cpp/Renderer.h"

// ospImGui util
#include "common/util/FPSCounter.h"
#include "common/util/transactional_value.h"

#include "common/util/AsyncRenderEngine.h"

namespace ospray {

  class AsyncRenderEngineDW : public AsyncRenderEngine
  {
  public:

    AsyncRenderEngineDW() = default;
    ~AsyncRenderEngineDW();

    // Properties //

    void setRenderer(cpp::Renderer    renderer,
                     cpp::Renderer    rendererDW,
                     cpp::FrameBuffer frameBufferDW);

    // Method to say that an objects needs to be comitted before next frame //

    void scheduleObjectCommit(const cpp::ManagedObject &obj);

    // Output queries //

    bool hasNewFrame() const;

    const std::vector<uint32_t> &mapFramebuffer();
    void                         unmapFramebuffer();

  private:

    // Helper functions //

    void validate() override;
    bool checkForObjCommits();
    bool checkForFbResize();
    void run() override;

    // Data //

    cpp::FrameBuffer frameBuffer;
    cpp::FrameBuffer frameBufferDW;

    transactional_value<cpp::Renderer>    renderer;
    transactional_value<cpp::Renderer>    rendererDW;
    transactional_value<ospcommon::vec2i> fbSize;

    int nPixels {0};

    int currentPB {0};
    int mappedPB  {1};
    std::mutex fbMutex;
    std::vector<uint32_t> pixelBuffer[2];

    std::mutex objMutex;
    std::vector<OSPObject> objsToCommit;

    std::atomic<bool> newPixels {false};
  };

}// namespace ospray
