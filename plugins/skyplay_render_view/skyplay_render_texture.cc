
#include "skyplay_render_texture.h"

#include "libskyplay_render.h"

namespace skyplay_render_view_plugin {

void SkyplayRenderTexture::RegisterWithRegistrar(
    flutter::PluginRegistrar* registrar,
    FlutterDesktopEngineRef engine)
{
    if (!LibSkyplayRender::IsPresent()) {
        spdlog::error("[SkyplayRenderViewPlugin] libskyplay_render.so missing");
    }

    auto plugin = std::make_unique<SkyplayRenderTexture>(registrar, engine);

    SkyplayApi::SetUp(registrar->messenger(), plugin.get());

    registrar->AddPlugin(std::move(plugin));
}

SkyplayRenderTexture::SkyplayRenderTexture(
    flutter::PluginRegistrar* registrar,
    FlutterDesktopEngineRef engine) :
    _registrar(registrar),
    _engine(engine)
{
}

SkyplayRenderTexture::~SkyplayRenderTexture() = default;

std::optional<skyplay_render_view::FlutterError> SkyplayRenderTexture::Initialize()
{
    flutter::TextureRegistrar* textureRegistrar =
        _registrar->texture_registrar();

    auto display = _engine->view_controller->view->GetDisplay()->GetDisplay();
    auto surface = _engine->view_controller->view->GetWindow()->GetBaseSurface();

    printf("Launch OGRE Renderer\n");
    LibSkyplayRender->initialize(display, surface);
    LibSkyplayRender->renderFrame();

    eglImage = LibSkyplayRender->getMapEglImage();
    //printf("EGL Image: %d\n", eglImage);

    textureRegistrar->TextureMakeCurrent();

    glGenTextures(1, &glTextureId);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, glTextureId);
    printf("GL Texture Initialized: ID %u\n", glTextureId);

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

    textureRegistrar->TextureClearCurrent();

    surfaceDescriptor = {
        .struct_size = sizeof(FlutterDesktopGpuSurfaceDescriptor),
        .handle = &glTextureId,
        .width = static_cast<size_t>(800),
        .height = static_cast<size_t>(480),
        .visible_width = static_cast<size_t>(800),
        .visible_height = static_cast<size_t>(480),
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

    flutterTextureId = textureRegistrar->RegisterTexture(&textureVariant);
    textureRegistrar->MarkTextureFrameAvailable(glTextureId);

    //printf("flutter texture ID: %ld\n", flutterTextureId);

    return std::nullopt;
}

skyplay_render_view::ErrorOr<int64_t> SkyplayRenderTexture::GetTextureHandle()
{
    return flutterTextureId;
}

}  // namespace skyplay_render_view_plugin
