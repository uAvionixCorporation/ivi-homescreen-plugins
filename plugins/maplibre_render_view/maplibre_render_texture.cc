
#include "maplibre_render_texture.h"

#include "maplibre_render.h"

namespace maplibre_render_view_plugin {

void MapLibreRenderTexture::RegisterWithRegistrar(
    flutter::PluginRegistrar* registrar,
    FlutterDesktopEngineRef engine)
{
    if (!MapLibreRender::IsPresent()) {
        spdlog::error("[MapLibreRenderViewPlugin] libmaplibre-native-ivi.so missing");
    }

    auto plugin = std::make_unique<MapLibreRenderTexture>(registrar, engine);

    MapLibreApi::SetUp(registrar->messenger(), plugin.get());

    registrar->AddPlugin(std::move(plugin));
}

MapLibreRenderTexture::MapLibreRenderTexture(
    flutter::PluginRegistrar* registrar,
    FlutterDesktopEngineRef engine) :
    _registrar(registrar),
    _engine(engine)
{
}

MapLibreRenderTexture::~MapLibreRenderTexture() = default;

std::optional<maplibre_render_view::FlutterError> MapLibreRenderTexture::Initialize()
{
    auto display = _engine->view_controller->view->GetDisplay()->GetDisplay();

    printf("Launch MapLibre Renderer\n");
    MapLibreRender->initialize(display);
    MapLibreRender->renderFrame();

    eglImage = MapLibreRender->getEglImage();

    flutter::TextureRegistrar* textureRegistrar =
        _registrar->texture_registrar();

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
        .width = static_cast<size_t>(1920),
        .height = static_cast<size_t>(1080),
        .visible_width = static_cast<size_t>(1920),
        .visible_height = static_cast<size_t>(1080),
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

maplibre_render_view::ErrorOr<int64_t> MapLibreRenderTexture::GetTextureHandle()
{
    return flutterTextureId;
}

std::optional<maplibre_render_view::FlutterError> MapLibreRenderTexture::RenderFrame()
{
    flutter::TextureRegistrar* textureRegistrar =
        _registrar->texture_registrar();

    MapLibreRender->renderFrame();

    textureRegistrar->MarkTextureFrameAvailable(glTextureId);

    return std::nullopt;
}

std::optional<maplibre_render_view::FlutterError> MapLibreRenderTexture::AdjustZoom(double steps, int64_t x, int64_t y)
{
    MapLibreRender->adjustZoom(steps, x, y);

    return std::nullopt;
}

}  // namespace maplibre_render_view_plugin
