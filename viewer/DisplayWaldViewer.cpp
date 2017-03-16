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

#include "DisplayWaldViewer.h"

namespace ospray {

  using namespace ospcommon;
  using DWV = DisplayWaldViewer;

  DWV::DisplayWaldViewer(const std::deque<box3f> &worldBounds,
                         std::deque<cpp::Model> model,
                         cpp::Renderer renderer,
                         cpp::Renderer rendererDW,
                         cpp::FrameBuffer frameBufferDW,
                         cpp::Camera camera)
    : ImGuiViewer(worldBounds, model, renderer,
                  rendererDW, frameBufferDW, camera)
  {
  }

  void DWV::setDisplayWall(const DWV::DisplayWall &dw)
  {
    displayWall = dw;

    displayWall.camera = cpp::Camera("perspective");
    displayWall.camera.set("aspect",
                           displayWall.size.x/float(displayWall.size.y));

    if (displayWall.stereo) {
      // in stereo mode, make sure to set
      displayWall.size.x *= 2;
    }

    displayWall.fb =
      ospray::cpp::FrameBuffer((const osp::vec2i&)displayWall.size,
                               OSP_FB_NONE,
                               OSP_FB_COLOR | OSP_FB_DEPTH | OSP_FB_ACCUM);

    displayWall.fb.clear(OSP_FB_ACCUM);

    if (displayWall.po.handle() == nullptr) {
      displayWall.po = ospray::cpp::PixelOp("display_wald@displayWald");
      displayWall.po.set("stereo",(int)displayWall.stereo);
      displayWall.po.set("hostname", displayWall.hostname);
      displayWall.po.set("streamName", displayWall.streamName);
      displayWall.po.commit();
    }

    displayWall.fb.setPixelOp(displayWall.po);
  }

  void DWV::display()
  {
  #if 0
    if (!frameBuffer.handle() || !renderer.handle()) return;

    if (viewPort.modified) {
      displayWall.camera.set("pos", viewPort.from);
      auto dir = viewPort.at - viewPort.from;
      displayWall.camera.set("dir", dir);
      displayWall.camera.set("up", viewPort.up);
      displayWall.camera.commit();
      displayWall.fb.clear(OSP_FB_ACCUM);
    }

    // when using displaywald we use two different camera (to adjust
    // for both different aspect ratios as well as for possibly
    // different stereo settings in viewer window vs display wall)
    renderer.set("camera",displayWall.camera);
    renderer.commit();
    renderer.renderFrame(displayWall.fb, OSP_FB_COLOR | OSP_FB_ACCUM);
    renderer.set("camera",camera);
    renderer.commit();

    OSPGlutViewer::display();
    #endif
  }

}// ::ospray
