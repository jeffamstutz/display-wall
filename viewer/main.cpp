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

#include "common/commandline/Utility.h"

#include "client/Client.h"

#include "DisplayWaldViewer.h"

using namespace commandline;

void parseForDisplayWall(int ac, const char **&av, ospray::DisplayWaldViewer &v)
{
#if 0
  bool initialized = false;
  for (int i = 1; i < ac; i++) {
    const std::string arg = av[i];
    if (arg == "--display-wall-host" || arg == "-dwh") {
      ospray::DisplayWaldViewer::DisplayWall displayWall;
      std::cout << "#osp.dwviewer: using displayWald-style display wall module"
                << std::endl;
      const std::string hostName = av[++i];

      /* todo: extract that from hostname if required */
      const int portNo = atoi(av[++i]);

      std::cout << "#osp.dwviewer: trying to get display wall config from "
                << hostName << ":" << portNo << std::endl;
      ospray::dw::ServiceInfo service;
      service.getFrom(hostName,portNo);
      const ospcommon::vec2i size = service.totalPixelsInWall;
      std::cout << "#osp.dwviewer: found display wald with "
                << size.x << "x" << size.y << " pixels "
                << "(" << ospcommon::prettyNumber(size.product()) << "pixels)" 
                << std::endl;
      displayWall.hostname   = service.mpiPortName;
      displayWall.streamName = service.mpiPortName;
      displayWall.size = size;
      displayWall.stereo = service.stereo;
#if 0// NOTE(jda) - old displaywall code
      displayWall.hostname   = av[++i];
      displayWall.streamName = av[++i];
      displayWall.size.x     = atof(av[++i]);
      displayWall.size.y     = atof(av[++i]);
#endif
      v.setDisplayWall(displayWall);
      initialized = true;
    }
  }

  if (!initialized) {
    std::cerr << "ERROR: You did not provide a hostname and port! Use -dwh "
              << "<hostname> <port> as part of the arguments to ospDwViewer!"
              << std::endl;
    std::exit(1);
  }
  #endif
}

int main(int ac, const char **av)
{
#if 0
  ospInit(&ac,av);
  ospray::imgui3D::init(&ac,av);
  ospLoadModule("displayWald");

  auto ospObjs = parseWithDefaultParsers(ac, av);

  std::deque<ospcommon::box3f> bbox;
  std::deque<ospray::cpp::Model> model;
  ospray::cpp::Renderer renderer;
  ospray::cpp::Camera camera;

  std::tie(bbox, model, renderer, camera) = ospObjs;

  ospray::DisplayWaldViewer window(bbox, model, renderer, camera);
  window.create("ospDwViewer: OSPRay Display Wall Test Viewer");

  parseForDisplayWall(ac, av, window);

  ospray::imgui3D::run();
  #endif
}
