#pragma once

#include <flutter/event_channel.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include "flutter_desktop_engine_state.h"

#include "skyplay_render_error.h"
#include "wayland/display.h"

#include <GLES2/gl2.h>
#include <EGL/egl.h>

namespace skyplay_render_view_plugin {

class SkyplayRenderTexture final : public flutter::Plugin {
public:
    static void RegisterWithRegistrar(
        flutter::PluginRegistrar* registrar,
        FlutterDesktopEngineRef engine);

    explicit SkyplayRenderTexture(
        flutter::PluginRegistrar* registrar,
        FlutterDesktopEngineRef engine);

    const FlutterDesktopGpuSurfaceDescriptor* ObtainDescriptor(size_t width, size_t height);

    ~SkyplayRenderTexture() override;

    // Disallow copy and assign.
    SkyplayRenderTexture(const SkyplayRenderTexture&) = delete;
    SkyplayRenderTexture& operator=(const SkyplayRenderTexture&) = delete;

private:
    std::unique_ptr<flutter::MethodChannel<>> channel_{};

    static void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue>& method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    std::unique_ptr<flutter::TextureVariant> texture_variant;
    int64_t flutter_texture_id = 0;
    GLuint gl_texture_id = 0;
    FlutterDesktopGpuSurfaceDescriptor surface_descriptor_ = {};
};

}  // namespace skyplay_render_view_plugin
