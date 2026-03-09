/*
 * Copyright 2020-2024 Toyota Connected North America
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "libskyplay_render.h"

#include <plugins/common/common.h>

#include <dlfcn.h>

namespace skyplay_render_view_plugin {

constexpr char kSkyplayRenderSoName[] = "/home/kyle/Documents/skyplay-software/cmake-build-debug/libSkyplay3DEngine.so";

LibSkyplayRenderExports::LibSkyplayRenderExports(void* lib) {
  if (lib != nullptr) {
    PluginGetFuncAddress(lib, "initialize", &initialize);
    PluginGetFuncAddress(lib, "getTerrainEglImage", &getTerrainEglImage);
    PluginGetFuncAddress(lib, "getMapEglImage", &getMapEglImage);
    PluginGetFuncAddress(lib, "get_terrain_texture_id", &getTerrainTextureID);
    PluginGetFuncAddress(lib, "get_map_texture_id", &getMapTextureID);
    PluginGetFuncAddress(lib, "renderFrame", &renderFrame);
  }
}

LibSkyplayRenderExports* LibSkyplayRender::operator->() const {
  return loadExports();
}

LibSkyplayRenderExports* LibSkyplayRender::loadExports() {
  static LibSkyplayRenderExports exports = [] {
    void* lib;

//    if (PluginGetProcAddress(
//            RTLD_DEFAULT,
//            "comp_surf_initialize"))  // Search the global scope
//                                      // for pre-loaded library.
//    {
//      lib = RTLD_DEFAULT;
//    } else {
      lib = dlopen(kSkyplayRenderSoName, RTLD_LAZY | RTLD_LOCAL);
//    }

    return LibSkyplayRenderExports(lib);
  }();

  // TODO
  //return exports.SurfaceInitialize ? &exports : nullptr;
  return &exports;
}

class LibSkyplayRender LibSkyplayRender;

}  // namespace skyplay_render_view_plugin
