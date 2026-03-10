
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

    auto texture =
        flutter::GpuSurfaceTexture(
            kFlutterDesktopGpuSurfaceTypeGlTexture2D,
            [this](size_t width, size_t height)
            {
                // This is the callback. It only runs when Flutter wants to DRAW.
                return ObtainDescriptor(width, height);
            }
        );

    // Create the texture proxy
    texture_variant = std::make_unique<flutter::TextureVariant>(texture);

    flutter_texture_id = registrar->texture_registrar()->RegisterTexture(texture_variant.get());

    //FlutterDesktopEngineState* state;
    auto display = engine->view_controller->view->GetDisplay()->GetDisplay();
    auto surface = engine->view_controller->view->GetWindow()->GetBaseSurface();

    printf("Launch OGRE Renderer\n");
    LibSkyplayRender->initialize(display, surface);
}

void SkyplayRenderTexture::HandleMethodCall(
    const flutter::MethodCall<>& method_call,
    std::unique_ptr<flutter::MethodResult<>> result)
{
    if (method_call.method_name() == "create") {

#if 0
        auto texture_registrar = registrar->texture_registrar();

        auto texture_variant = std::make_unique<flutter::TextureVariant>(
            flutter::GpuSurfaceTexture(
                kFlutterDesktopGpuSurfaceTypeGlTexture2d,
                [this](size_t width, size_t height) -> const FlutterDesktopGpuSurfaceDescriptor*
                {
                    // Your logic to return the EGL/GL texture handle
                    return this->GetSurfaceDescriptor(width, height);
                }
            )
       );

        int64_t texture_id = texture_registrar->RegisterTexture(texture_variant.get());
#endif
        int64_t texture_id = 0;
        flutter::EncodableMap response;
        response[flutter::EncodableValue("textureId")] = flutter::EncodableValue(texture_id);
        result->Success(flutter::EncodableValue(response));
    }
}

SkyplayRenderTexture::~SkyplayRenderTexture() = default;

const FlutterDesktopGpuSurfaceDescriptor* SkyplayRenderTexture::ObtainDescriptor(size_t width, size_t height)
{
    surface_descriptor_.struct_size = sizeof(FlutterDesktopGpuSurfaceDescriptor);
    surface_descriptor_.handle = nullptr;
    surface_descriptor_.width = width;
    surface_descriptor_.height = height;
    surface_descriptor_.format = kFlutterDesktopPixelFormatBGRA8888; // Standard for D3D11
    return &surface_descriptor_;
}

}  // namespace skyplay_render_view_plugin
