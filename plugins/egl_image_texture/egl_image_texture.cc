
#include "egl_image_texture.h"

namespace egl_image_texture_plugin {

void EglImageTexture::RegisterWithRegistrar(
    flutter::PluginRegistrar* registrar,
    FlutterDesktopEngineRef engine)
{
    auto plugin = std::make_unique<EglImageTexture>(registrar, engine);

    EglImageTextureApi::SetUp(registrar->messenger(), plugin.get());

    registrar->AddPlugin(std::move(plugin));
}

EglImageTexture::EglImageTexture(
    flutter::PluginRegistrar* registrar,
    FlutterDesktopEngineRef engine) :
    _registrar(registrar),
    _engine(engine),
    index(0)
{
}

EglImageTexture::~EglImageTexture() = default;

egl_image_texture::ErrorOr<int64_t> EglImageTexture::GetNativeDisplay()
{
    auto display = _engine->view_controller->view->GetDisplay()->GetDisplay();

    return (int64_t)display;
}

egl_image_texture::ErrorOr<int64_t> EglImageTexture::GetNativeSurface()
{
    auto surface = _engine->view_controller->view->GetWindow()->GetBaseSurface();

    return (int64_t)surface;
}

egl_image_texture::ErrorOr<int64_t> EglImageTexture::RegisterEglImage(int64_t egl_image)
{
    auto handle = std::make_unique<Handle>();

    handle->eglImage = (void*)egl_image;

    flutter::TextureRegistrar* textureRegistrar =
        _registrar->texture_registrar();

    textureRegistrar->TextureMakeCurrent();

    glGenTextures(1, &handle->glTextureId);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, handle->glTextureId);

    glEGLImageTargetTexture2DOES =
        (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (glEGLImageTargetTexture2DOES) {
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, handle->eglImage);
    }
    else
    {
        printf(">>>>>>>>>>>>>>>>>>>> Error <<<<<<<<<<<<<<<<<<<<\n");
    }

    textureRegistrar->TextureClearCurrent();

    handle->surfaceDescriptor = {
        .struct_size = sizeof(FlutterDesktopGpuSurfaceDescriptor),
        .handle = &handle->glTextureId,
        .width = static_cast<size_t>(1920),
        .height = static_cast<size_t>(1080),
        .visible_width = static_cast<size_t>(1920),
        .visible_height = static_cast<size_t>(1080),
        .format = kFlutterDesktopPixelFormatRGBA8888,
        .release_callback = [](void* /* release_context */) {},
        .release_context = this
    };

    handle->gpuSurfaceTexture =
        std::make_unique<flutter::GpuSurfaceTexture>(
            kFlutterDesktopGpuSurfaceTypeGlTexture2D,
            [&](size_t width, size_t height) -> const FlutterDesktopGpuSurfaceDescriptor*
            {
                (void)width;
                (void)height;
                return &handle->surfaceDescriptor;
            }
        );

    flutter::TextureVariant textureVariant = *handle->gpuSurfaceTexture;

    handle->flutterTextureId = textureRegistrar->RegisterTexture(&textureVariant);
    textureRegistrar->MarkTextureFrameAvailable(handle->glTextureId);

    registry[index] = std::move(handle);
    return index++;
}

egl_image_texture::ErrorOr<int64_t> EglImageTexture::GetFlutterTextureId(int64_t handle)
{
    return registry[handle]->flutterTextureId;
}

std::optional<egl_image_texture::FlutterError> EglImageTexture::MarkTextureAvailable(int64_t handle)
{
    _registrar->texture_registrar()->MarkTextureFrameAvailable(registry[handle]->glTextureId);

    return std::nullopt;
}

}  // namespace egl_image_texture_plugin
