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

#include "Server.h"
#include "GlutWindow.h"
#include <thread>

#include <stdlib.h>

namespace ospray {
  namespace dw {

    using std::cout; 
    using std::endl;
    using std::flush;
    
    void usage(const std::string &err)
    {
      if (!err.empty()) {
        cout << "Error: " << err << endl << endl;
      }
      cout << "usage: ./ospDisplayWald [args]*" << endl << endl;
      cout << "w/ args: " << endl;
      cout << "--width|-w <numDisplays.x>        - num displays in x direction" << endl;
      cout << "--height|-h <numDisplays.y>       - num displays in y direction" << endl;
      cout << "--window-size|-ws <res_x> <res_y> - window size (in pixels)" << endl;
      cout << "--[no-]head-node | -[n]hn         - use / do not use dedicated head node" << endl;
      exit(!err.empty());
    }

    /*! the display callback */
    void displayNewFrame(const uint32_t *left, 
                         const uint32_t *right, 
                         void *object)
    {
      GlutWindow *window = (GlutWindow*)object;
      window->setFrameBuffer(left,right);
    }

    extern "C" int main(int ac, char **av)
    {
      
      MPI::init(ac,av);
      MPI::Group world(MPI_COMM_WORLD);

      // default settings
      bool hasHeadNode = false;
      bool doStereo    = false;
      WallConfig::DisplayArrangement arrangement = WallConfig::Arrangement_xy;
      vec2f relativeBezelWidth(0.f);
      vec2i windowSize(320,240);
      vec2i windowPosition(0,0);
      vec2i numDisplays(0,0);

      for (int i=1;i<ac;i++) {
        const std::string arg = av[i];
        if (arg == "--head-node" || arg == "-hn") {
          hasHeadNode = true;
        } else if (arg == "--stereo" || arg == "-s") {
          doStereo = true;
        } else if (arg == "--no-head-node" || arg == "-nhn") {
          hasHeadNode = false;
        } else if (arg == "--width" || arg == "-w") {
          assert(i+1<ac);
          numDisplays.x = atoi(av[++i]);
        } else if (arg == "--height" || arg == "-h") {
          assert(i+1<ac);
          numDisplays.y = atoi(av[++i]);
        } else if (arg == "--window-size" || arg == "-ws") {
          assert(i+1<ac);
          windowSize.x = atoi(av[++i]);
          assert(i+1<ac);
          windowSize.y = atoi(av[++i]);
        } else if (arg == "--bezel" || arg == "-b") {
          if (i+2 >= ac) {
            printf("format for --bezel|-b argument is '-b <x> <y>'\n");
            exit(1);
          }
          assert(i+1<ac);
          relativeBezelWidth.x = atof(av[++i]);
          assert(i+1<ac);
          relativeBezelWidth.y = atof(av[++i]);
        } else {
          usage("unkonwn arg "+arg);
        } 
      }

      if (numDisplays.x < 1) 
        usage("no display wall width specified (--width <w>)");
      if (numDisplays.y < 1) 
        usage("no display wall height specified (--heigh <h>)");
      if (world.size != numDisplays.x*numDisplays.y+hasHeadNode)
        throw std::runtime_error("invalid number of ranks for given display/head node config");

      const int displayNo = hasHeadNode ? world.rank-1 : world.rank;
      const vec2i displayID(displayNo % numDisplays.x, displayNo / numDisplays.x);

      char title[1000];
      sprintf(title,"display (%i,%i)",displayID.x,displayID.y);
      
      /*
      if (world.rank == 2) {
        setenv("DISPLAY", ":0.0", 1);
      }
      
      else if (world.rank == 3) {
        setenv("DISPLAY", ":0.0", 1);
      }
      
      else if (world.rank == 0) {
        setenv("DISPLAY", ":0.1", 1);
      }
      
      else if (world.rank == 1) {
        setenv("DISPLAY", ":0.1", 1);
      }
      
      else {
        setenv("DISPLAY", ":0", 1);
      }
      
      
      printf("Rank %d, display (%d, %d), host %s\n", world.rank, displayID.x, displayID.y, world.name);
      
      //exit(1);
      */
      
      glutInit(&ac, (char **) av);
      
      bool doFullScreen = false;
      
      GlutWindow glutWindow(windowSize,windowPosition,title,doFullScreen,doStereo);
      
      WallConfig wallConfig(numDisplays,windowSize,
                            relativeBezelWidth,
                            arrangement,doStereo);
      
      std::string configFileName = "configuration.xml";
      
      if (world.rank == 0) {
        cout << "#osp:dw: display wall config is" << endl;
        wallConfig.print();
      }
      
      
      if (hasHeadNode && world.rank == 0) {
        cout << "#osp:dw: running a dedicated headnode on rank 0; "
             << "not creating a window there" << endl;
      } else {
        glutWindow.create();
      }

      startDisplayWallService(world.comm,wallConfig,hasHeadNode,
                              displayNewFrame,&glutWindow);
      
      if (hasHeadNode && world.rank == 0) {
        /* no window on head node */
        throw std::runtime_error("should never reach this ...");
      } else {
        glutWindow.run();
      }
      // commThread.join();
      return 0;
    }
    
  } // ::ospray::dw
} // ::ospray
