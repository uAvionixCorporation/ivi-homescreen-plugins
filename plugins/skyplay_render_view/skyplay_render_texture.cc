
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
    auto display = _engine->view_controller->view->GetDisplay()->GetDisplay();
    auto surface = _engine->view_controller->view->GetWindow()->GetBaseSurface();

    printf("Launch OGRE Renderer\n");
    LibSkyplayRender->initialize(display, surface);
    LibSkyplayRender->renderFrame();

    eglImage[0] = LibSkyplayRender->getMapEglImage();
    eglImage[1] = LibSkyplayRender->getTerrainEglImage();

    initializeInstance(0);
    initializeInstance(1);

    return std::nullopt;
}

void SkyplayRenderTexture::initializeInstance(uint8_t instance)
{
    flutter::TextureRegistrar* textureRegistrar =
        _registrar->texture_registrar();

    textureRegistrar->TextureMakeCurrent();

    glGenTextures(1, &glTextureId[instance]);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, glTextureId[instance]);
    printf("GL Texture Initialized: ID %u\n", glTextureId[instance]);

    glEGLImageTargetTexture2DOES =
        (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (glEGLImageTargetTexture2DOES) {
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, eglImage[instance]);
    }
    else
    {
        printf(">>>>>>>>>>>>>>>>>>>> Error <<<<<<<<<<<<<<<<<<<<\n");
    }

    textureRegistrar->TextureClearCurrent();

    surfaceDescriptor[instance] = {
        .struct_size = sizeof(FlutterDesktopGpuSurfaceDescriptor),
        .handle = &glTextureId[instance],
        .width = static_cast<size_t>(800),
        .height = static_cast<size_t>(480),
        .visible_width = static_cast<size_t>(800),
        .visible_height = static_cast<size_t>(480),
        .format = kFlutterDesktopPixelFormatRGBA8888,
        .release_callback = [](void* /* release_context */) {},
        .release_context = this
    };

    gpuSurfaceTexture[instance] =
        std::make_unique<flutter::GpuSurfaceTexture>(
            kFlutterDesktopGpuSurfaceTypeGlTexture2D,
            [&](size_t width, size_t height) -> const FlutterDesktopGpuSurfaceDescriptor*
            {
                (void)width;
                (void)height;
                return &surfaceDescriptor[instance];
            }
        );

    flutter::TextureVariant textureVariant = *gpuSurfaceTexture[instance];

    flutterTextureId[instance] = textureRegistrar->RegisterTexture(&textureVariant);
    textureRegistrar->MarkTextureFrameAvailable(glTextureId[instance]);

    //printf("flutter texture ID: %ld\n", flutterTextureId);
}

skyplay_render_view::ErrorOr<int64_t> SkyplayRenderTexture::GetMapTextureHandle()
{
    return flutterTextureId[0];
}

skyplay_render_view::ErrorOr<int64_t> SkyplayRenderTexture::GetTerrainTextureHandle()
{
    return flutterTextureId[1];
}

std::optional<skyplay_render_view::FlutterError> SkyplayRenderTexture::RenderFrame()
{
    flutter::TextureRegistrar* textureRegistrar =
        _registrar->texture_registrar();

    LibSkyplayRender->renderFrame();

    textureRegistrar->MarkTextureFrameAvailable(glTextureId[0]);
    textureRegistrar->MarkTextureFrameAvailable(glTextureId[1]);

    return std::nullopt;
}

}  // namespace skyplay_render_view_plugin
