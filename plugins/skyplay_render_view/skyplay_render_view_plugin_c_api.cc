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

#include "include/skyplay_render_view/skyplay_render_view_plugin_c_api.h"

#include <flutter/plugin_registrar.h>

#include "skyplay_render_surface.h"
#include "skyplay_render_texture.h"

void SkyplayRenderViewPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar,
    const int32_t id,
    std::string viewType,
    const int32_t direction,
    const double top,
    const double left,
    const double width,
    const double height,
    const std::vector<uint8_t>& params,
    std::string assetDirectory,
    FlutterDesktopEngineRef engine,
    const PlatformViewAddListener add_listener,
    const PlatformViewRemoveListener remove_listener,
    void* platform_views_context)
{
  skyplay_render_view_plugin::SkyplayRenderSurface::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrar>(registrar),
      id, std::move(viewType), direction, top, left, width, height, params,
      std::move(assetDirectory), engine, add_listener, remove_listener,
      platform_views_context);
}

void SkyplayRenderViewPluginTextureCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrar* registrar,
    FlutterDesktopEngineRef engine)
{
    skyplay_render_view_plugin::SkyplayRenderTexture::RegisterWithRegistrar(
        flutter::PluginRegistrarManager::GetInstance()
            ->GetRegistrar<flutter::PluginRegistrar>(registrar),
        engine);
}
