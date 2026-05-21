#pragma once

#include <flutter/event_channel.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include "flutter_desktop_engine_state.h"

#include "wayland/display.h"

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "messages.g.h"

namespace maplibre_render_view_plugin {

class MapLibreRenderTexture final : public flutter::Plugin, public maplibre_render_view::MapLibreApi {
public:
    static void RegisterWithRegistrar(
        flutter::PluginRegistrar* registrar,
        FlutterDesktopEngineRef engine);

    explicit MapLibreRenderTexture(
        flutter::PluginRegistrar* registrar,
        FlutterDesktopEngineRef engine);

    ~MapLibreRenderTexture() override;

    // Disallow copy and assign.
    MapLibreRenderTexture(const MapLibreRenderTexture&) = delete;
    MapLibreRenderTexture& operator=(const MapLibreRenderTexture&) = delete;

    maplibre_render_view::ErrorOr<int64_t> GetNativeDisplay() override;
    maplibre_render_view::ErrorOr<int64_t> RegisterEglImage(int64_t egl_image) override;
    std::optional<maplibre_render_view::FlutterError> MarkTextureAvailable() override;

private:
    std::unique_ptr<flutter::GpuSurfaceTexture> gpuSurfaceTexture;
    int64_t flutterTextureId;
    GLuint glTextureId;
    FlutterDesktopGpuSurfaceDescriptor surfaceDescriptor;
    EGLImage eglImage;
    flutter::PluginRegistrar* _registrar = nullptr;
    FlutterDesktopEngineRef _engine = nullptr;

    typedef void (*PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum target, void * image);

    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
};

}  // namespace maplibre_render_view_plugin
