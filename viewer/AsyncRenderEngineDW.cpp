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

#include "AsyncRenderEngineDW.h"

namespace ospray {

  AsyncRenderEngineDW::~AsyncRenderEngineDW()
  {
    stop();
  }

  void AsyncRenderEngineDW::setRenderer(cpp::Renderer    renderer,
                                           cpp::Renderer    rendererDW,
                                           cpp::FrameBuffer frameBufferDW)
  {
    this->renderer      = renderer;
    this->rendererDW    = rendererDW;
    this->frameBufferDW = frameBufferDW;
  }

  void AsyncRenderEngineDW::scheduleObjectCommit(const cpp::ManagedObject &obj)
  {
    std::lock_guard<std::mutex> lock{objMutex};
    objsToCommit.push_back(obj.object()); 
  }

  bool AsyncRenderEngineDW::hasNewFrame() const
  {
    return newPixels;
  }

  const std::vector<uint32_t> &AsyncRenderEngineDW::mapFramebuffer()
  {
    fbMutex.lock();
    newPixels = false;
    return pixelBuffer[mappedPB];
  }

  void AsyncRenderEngineDW::unmapFramebuffer()
  {
    fbMutex.unlock();
  }

  void AsyncRenderEngineDW::validate()
  {
    if (state == ExecState::INVALID) {
      renderer.update();
      rendererDW.update();
      state = renderer.ref().handle() ? ExecState::STOPPED : ExecState::INVALID;
    }
  }

  bool AsyncRenderEngineDW::checkForObjCommits()
  {
    bool commitOccurred = false;

    if (!objsToCommit.empty()) {
      std::lock_guard<std::mutex> lock{objMutex};

      for (auto obj : objsToCommit)
        ospCommit(obj);

      objsToCommit.clear();
      commitOccurred = true;
    }

    return commitOccurred;
  }

  bool AsyncRenderEngineDW::checkForFbResize()
  {
    bool changed = fbSize.update();

    if (changed) {
      auto &size  = fbSize.ref();
      frameBuffer = cpp::FrameBuffer(osp::vec2i{size.x, size.y}, OSP_FB_SRGBA,
                                     OSP_FB_COLOR | OSP_FB_DEPTH | OSP_FB_ACCUM);

      nPixels = size.x * size.y;
      pixelBuffer[0].resize(nPixels);
      pixelBuffer[1].resize(nPixels);
    }

    return changed;
  }

  void AsyncRenderEngineDW::run()
  {
    auto device = ospGetCurrentDevice();
    if (numOsprayThreads > 0)
      ospDeviceSet1i(device, "numThreads", numOsprayThreads);
    ospDeviceCommit(device);

    while (state == ExecState::RUNNING) {
      bool resetAccum = false;
      resetAccum |= renderer.update();
      resetAccum |= checkForFbResize();
      resetAccum |= checkForObjCommits();

      if (resetAccum) {
        frameBuffer.clear(OSP_FB_ACCUM);
        if (frameBufferDW)
          frameBufferDW.clear(OSP_FB_ACCUM);
      }

      fps.startRender();
      renderer.ref().renderFrame(frameBuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
      if (rendererDW.ref())
        rendererDW.ref().renderFrame(frameBufferDW, OSP_FB_COLOR | OSP_FB_ACCUM);
      fps.doneRender();

      auto *srcPB = (uint32_t*)frameBuffer.map(OSP_FB_COLOR);
      auto *dstPB = (uint32_t*)pixelBuffer[currentPB].data();

      memcpy(dstPB, srcPB, nPixels*sizeof(uint32_t));

      frameBuffer.unmap(srcPB);

      if (fbMutex.try_lock()) {
        std::swap(currentPB, mappedPB);
        newPixels = true;
        fbMutex.unlock();
      }
    }
  }

}// namespace ospray
