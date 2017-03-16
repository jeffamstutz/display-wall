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

#include <deque>
#include "widgets/imguiViewer.h"

namespace ospray {

  class DisplayWaldViewer : public ImGuiViewer
  {
  public:

    DisplayWaldViewer(const std::deque<ospcommon::box3f> &worldBounds,
                      std::deque<cpp::Model> model,
                      cpp::Renderer renderer,
                      cpp::Renderer rendererDW,
                      cpp::FrameBuffer frameBufferDW,
                      cpp::Camera camera);

    // Helper types //

    struct DisplayWall
    {
      std::string hostname;
      std::string streamName;
      ospcommon::vec2i size{-1};
      ospray::cpp::FrameBuffer fb;
      ospray::cpp::PixelOp     po;
      // display uses its own camera; it has a different aspect ratio!
      ospray::cpp::Camera      camera;
      int stereo;
    } displayWall;

    void setDisplayWall(const DisplayWall &dw);

  private:

    void display() override;

    // Data members //

    AsyncRenderEngineLocal renderEngine;
  };

}// namespace ospray
