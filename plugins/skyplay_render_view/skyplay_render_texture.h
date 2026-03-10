#pragma once

#include <flutter/event_channel.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include "flutter_desktop_engine_state.h"

#include "wayland/display.h"

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "messages.g.h"

namespace skyplay_render_view_plugin {

class SkyplayRenderTexture final : public flutter::Plugin, public skyplay_render_view::SkyplayApi {
public:
    static void RegisterWithRegistrar(
        flutter::PluginRegistrar* registrar,
        FlutterDesktopEngineRef engine);

    explicit SkyplayRenderTexture(
        flutter::PluginRegistrar* registrar,
        FlutterDesktopEngineRef engine);

    ~SkyplayRenderTexture() override;

    // Disallow copy and assign.
    SkyplayRenderTexture(const SkyplayRenderTexture&) = delete;
    SkyplayRenderTexture& operator=(const SkyplayRenderTexture&) = delete;

    std::optional<skyplay_render_view::FlutterError> Initialize() override;
    skyplay_render_view::ErrorOr<int64_t> GetMapTextureHandle() override;
    skyplay_render_view::ErrorOr<int64_t> GetTerrainTextureHandle() override;
    std::optional<skyplay_render_view::FlutterError> RenderFrame() override;

private:
    std::unique_ptr<flutter::GpuSurfaceTexture> gpuSurfaceTexture[2];
    int64_t flutterTextureId[2];
    GLuint glTextureId[2];
    FlutterDesktopGpuSurfaceDescriptor surfaceDescriptor[2];
    EGLImage eglImage[2];
    flutter::PluginRegistrar* _registrar = nullptr;
    FlutterDesktopEngineRef _engine = nullptr;

    void initializeInstance(uint8_t instance);

    typedef void (*PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum target, void * image);

    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
};

}  // namespace skyplay_render_view_plugin
