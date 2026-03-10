
#include "skyplay_render_texture.h"

#include "libskyplay_render.h"

namespace skyplay_render_view_plugin {

[[maybe_unused]] static constexpr int kExpectedRenderApiVersion = 0x00010002;

void SkyplayRenderTexture::RegisterWithRegistrar(
    flutter::PluginRegistrar* registrar,
    FlutterDesktopEngineRef engine)
{
    if (!LibSkyplayRender::IsPresent()) {
        spdlog::error("[SkyplayRenderViewPlugin] libskyplay_render.so missing");
    }

    auto plugin = std::make_unique<SkyplayRenderTexture>(registrar, engine);
    registrar->AddPlugin(std::move(plugin));
}

SkyplayRenderTexture::SkyplayRenderTexture(
    flutter::PluginRegistrar* registrar,
    FlutterDesktopEngineRef engine)
{
    channel_ = std::make_unique<flutter::MethodChannel<>>(
        registrar->messenger(), "skyplay_render_view",
        &flutter::StandardMethodCodec::GetInstance());

    channel_->SetMethodCallHandler(
        [](const flutter::MethodCall<>& call, std::unique_ptr<flutter::MethodResult<>> result)
        {
            HandleMethodCall(call, std::move(result));
        }
    );

    flutter::TextureRegistrar* texture_registrar_ =
        registrar->texture_registrar();

    texture_registrar_->TextureMakeCurrent();

    EGLContext current_context = eglGetCurrentContext();
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Context State: ");
    if (current_context != EGL_NO_CONTEXT) {
        printf("VALID\n");
    } else {
        printf("INVALID\n");
    }

    glGenTextures(1, &glTextureId);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, glTextureId);
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Texture Initialized: ID %u\n", glTextureId);

    glEGLImageTargetTexture2DOES =
        (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (glEGLImageTargetTexture2DOES) {
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, eglImage);
    }
    else
    {
        printf(">>>>>>>>>>>>>>>>>>>> Error <<<<<<<<<<<<<<<<<<<<\n");
    }

    surfaceDescriptor = {
        .struct_size = sizeof(FlutterDesktopGpuSurfaceDescriptor),
        .handle = &glTextureId,
        .width = 1920,
        .height = 1080,
        .visible_width = 1920,
        .visible_height = 1080,
        .format = kFlutterDesktopPixelFormatRGBA8888,
        .release_callback = [](void* /* release_context */) {},
        .release_context = this
    };

    gpuSurfaceTexture =
        std::make_unique<flutter::GpuSurfaceTexture>(
            kFlutterDesktopGpuSurfaceTypeGlTexture2D,
            [&](size_t width, size_t height) -> const FlutterDesktopGpuSurfaceDescriptor*
            {
                (void)width;
                (void)height;
                return &surfaceDescriptor;
            }
        );

    flutter::TextureVariant textureVariant = *gpuSurfaceTexture;

    flutterTextureId = registrar->texture_registrar()->RegisterTexture(&textureVariant);

    auto display = engine->view_controller->view->GetDisplay()->GetDisplay();
    auto surface = engine->view_controller->view->GetWindow()->GetBaseSurface();

    printf("Launch OGRE Renderer\n");
    LibSkyplayRender->initialize(display, surface);

    printf("flutter texture ID: %ld\n", flutterTextureId);
}

void SkyplayRenderTexture::HandleMethodCall(
    const flutter::MethodCall<>& method_call,
    std::unique_ptr<flutter::MethodResult<>> result)
{
    if (method_call.method_name() == "create") {
        int64_t texture_id = 0;
        flutter::EncodableMap response;
        response[flutter::EncodableValue("textureId")] = flutter::EncodableValue(texture_id);
        result->Success(flutter::EncodableValue(response));
    }
}

SkyplayRenderTexture::~SkyplayRenderTexture() = default;

}  // namespace skyplay_render_view_plugin
