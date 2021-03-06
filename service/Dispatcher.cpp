/* 
   Copyright (c) 2016 Ingo Wald

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include "../common/MPI.h"
#include "../common/CompressedTile.h"
#include "../common/WallConfig.h"

namespace ospray {
  namespace dw {

    using std::cout; 
    using std::endl;
    using std::flush;

    /*! the dispatcher that receives tiles on the head node, and then
      dispatches them to the actual tile receivers */
    void runDispatcher(const MPI::Group &outsideClients,
                       const MPI::Group &displayGroup,
                       const WallConfig &wallConfig)
    {
      // std::thread *dispatcherThread = new std::thread([=]() {
      std::cout << "#osp:dw(hn): running dispatcher on rank 0" << std::endl;

      size_t numWrittenThisFrame = 0;
      size_t numExpectedThisFrame = wallConfig.totalPixelCount();

      while (1) {
        CompressedTile encoded;
        DW_DBG(printf("dispatcher trying to receive...\n"));
        encoded.receiveOne(outsideClients);

        const box2i region = encoded.getRegion();
        
        // -------------------------------------------------------
        // compute displays affected by this tile
        // -------------------------------------------------------
        const box2i affectedDisplays = wallConfig.affectedDisplays(region);
        
        DW_DBG(printf("region %i %i - %i %i displays %i %i - %i %i\n",
                      region.lower.x,
                      region.lower.y,
                      region.upper.x,
                      region.upper.y,
                      affectedDisplays.lower.x,
                      affectedDisplays.lower.y,
                      affectedDisplays.upper.x,
                      affectedDisplays.upper.y));

        // -------------------------------------------------------
        // now, send to all affected displays ...
        // -------------------------------------------------------
        for (int dy=affectedDisplays.lower.y;dy<affectedDisplays.upper.y;dy++)
          for (int dx=affectedDisplays.lower.x;dx<affectedDisplays.upper.x;dx++) {
            DW_DBG(printf("sending to %i/%i -> %i\n",dx,dy,wallConfig.rankOfDisplay(vec2i(dx,dy))));
            encoded.sendTo(displayGroup,wallConfig.rankOfDisplay(vec2i(dx,dy)));
          }

        numWrittenThisFrame += region.size().product();
        DW_DBG(printf("dispatch %i/%i\n",numWrittenThisFrame,numExpectedThisFrame));
        if (numWrittenThisFrame == numExpectedThisFrame) {
          DW_DBG(printf("#osp:dw(hn): head node has a full frame\n"));
          outsideClients.barrier();
          displayGroup.barrier();

          numWrittenThisFrame = 0;
        }
        
      };
      // });
    }

    
  } // ::ospray::dw
} // ::ospray
