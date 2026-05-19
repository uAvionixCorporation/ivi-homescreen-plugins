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

namespace maplibre_render_view_plugin {

struct MapLibreRenderExports
{
    MapLibreRenderExports() = default;
    explicit MapLibreRenderExports(void* lib);

    void  (*initialize)(void*) = nullptr;
    void* (*getEglImage)() = nullptr;
    void  (*renderFrame)() = nullptr;
};

class MapLibreRender
{
public:
    static bool IsPresent() { return loadExports() != nullptr; }

    MapLibreRenderExports* operator->() const;

private:
    static MapLibreRenderExports* loadExports();
};

extern MapLibreRender MapLibreRender;

}  // namespace maplibre_render_view_plugin
