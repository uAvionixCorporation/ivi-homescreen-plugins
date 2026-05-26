#pragma once

#include <flutter/event_channel.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>
#include "flutter_desktop_engine_state.h"

#include "wayland/display.h"

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "messages.g.h"

namespace egl_image_texture_plugin {

class EglImageTexture final : public flutter::Plugin, public egl_image_texture::EglImageTextureApi {
public:
    static void RegisterWithRegistrar(
        flutter::PluginRegistrar* registrar,
        FlutterDesktopEngineRef engine);

    explicit EglImageTexture(
        flutter::PluginRegistrar* registrar,
        FlutterDesktopEngineRef engine);

    ~EglImageTexture() override;

    // Disallow copy and assign.
    EglImageTexture(const EglImageTexture&) = delete;
    EglImageTexture& operator=(const EglImageTexture&) = delete;

    egl_image_texture::ErrorOr<int64_t> GetNativeDisplay() override;
    egl_image_texture::ErrorOr<int64_t> GetNativeSurface() override;
    egl_image_texture::ErrorOr<int64_t> RegisterEglImage(int64_t egl_image) override;
    egl_image_texture::ErrorOr<int64_t> GetFlutterTextureId(int64_t handle) override;
    std::optional<egl_image_texture::FlutterError> MarkTextureAvailable(int64_t handle) override;

private:
    class Handle
    {
    public:
        std::unique_ptr<flutter::GpuSurfaceTexture> gpuSurfaceTexture;
        int64_t flutterTextureId;
        GLuint glTextureId;
        FlutterDesktopGpuSurfaceDescriptor surfaceDescriptor;
        EGLImage eglImage;
    };

    static constexpr uint32_t MAX_ITEMS = 2;

    std::unique_ptr<Handle> registry[MAX_ITEMS];

    flutter::PluginRegistrar* _registrar = nullptr;
    FlutterDesktopEngineRef _engine = nullptr;

    int index;

    typedef void (*PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum target, void * image);

    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
};

}  // namespace egl_image_texture_plugin
