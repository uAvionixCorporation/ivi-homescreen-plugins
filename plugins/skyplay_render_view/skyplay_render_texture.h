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

private:
    std::unique_ptr<flutter::MethodChannel<>> channel_{};

    std::unique_ptr<flutter::GpuSurfaceTexture> gpuSurfaceTexture;
    int64_t flutterTextureId = 0;
    GLuint glTextureId = 0;
    FlutterDesktopGpuSurfaceDescriptor surfaceDescriptor = {};
    EGLImage eglImage;
    flutter::PluginRegistrar* _registrar = nullptr;
    FlutterDesktopEngineRef _engine;

    typedef void (*PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum target, void * image);

    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
};

}  // namespace skyplay_render_view_plugin
