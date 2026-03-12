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
#include <cstdio>

namespace skyplay_render_view_plugin {

constexpr char kSkyplayRenderSoName[] = "libSkyplay3DEngine.so";

LibSkyplayRenderExports::LibSkyplayRenderExports(void* lib)
{
    if (lib != nullptr) {
        PluginGetFuncAddress(lib, "initialize", &initialize);
        PluginGetFuncAddress(lib, "getTerrainEglImage", &getTerrainEglImage);
        PluginGetFuncAddress(lib, "getMapEglImage", &getMapEglImage);
        PluginGetFuncAddress(lib, "renderFrame", &renderFrame);
    }
}

LibSkyplayRenderExports* LibSkyplayRender::operator->() const
{
    return loadExports();
}

LibSkyplayRenderExports* LibSkyplayRender::loadExports() {
    static LibSkyplayRenderExports* exports_ptr = []() -> LibSkyplayRenderExports* {

        void* lib = dlopen(kSkyplayRenderSoName, RTLD_LAZY | RTLD_LOCAL);
        if (!lib) {
            spdlog::error("[LibSkyplayRender] Error: Failed to load {}: {}", kSkyplayRenderSoName, dlerror());
            return nullptr;
        }

        spdlog::error("[LibSkyplayRender] Successfully loaded {}", kSkyplayRenderSoName);

        static LibSkyplayRenderExports exports(lib);
        return &exports;
    }();

    return exports_ptr;
}

class LibSkyplayRender LibSkyplayRender;

}  // namespace skyplay_render_view_plugin
