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

#pragma once

#include <cstdint>

namespace skyplay_render_view_plugin {

typedef void (*LoggerFunction)(int level,
                               const char* context,
                               const char* message);
typedef const void* (*GlLoaderFunction)(void* userdata, const char* procname);

typedef struct {
  void* dpy;
  void* context;
  uint32_t framebufferId;
  const char* access_token;
  int width;
  int height;
  const char* asset_path;
  const char* cache_folder;
  const char* misc_folder;
  LoggerFunction pfn_log;
  GlLoaderFunction pfn_gl_loader;
  const void* native_window;
#if HEADLESS_BACKEND_EGL
  void* contextHeadless;
  unsigned char* buffer;
#endif
} SkyplayRenderConfig;

struct skyplay_render_Context;

struct LibSkyplayRenderExports {
  LibSkyplayRenderExports() = default;
  explicit LibSkyplayRenderExports(void* lib);

  void  (*initialize)(void *, void*) = nullptr;
  void* (*getTerrainEglImage)() = nullptr;
  void* (*getMapEglImage)() = nullptr;
  void  (*renderFrame)() = nullptr;
};

class LibSkyplayRender {
 public:
  static bool IsPresent() { return loadExports() != nullptr; }

  LibSkyplayRenderExports* operator->() const;

 private:
  static LibSkyplayRenderExports* loadExports();
};

extern LibSkyplayRender LibSkyplayRender;

}  // namespace skyplay_render_view_plugin
