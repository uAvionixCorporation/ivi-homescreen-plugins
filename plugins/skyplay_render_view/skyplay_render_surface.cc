
#include "skyplay_render_surface.h"

#include <flutter/standard_message_codec.h>
#include <plugins/common/common.h>

#include <utility>

#include "libskyplay_render.h"

namespace skyplay_render_view_plugin {

// static
void SkyplayRenderSurface::RegisterWithRegistrar(
    flutter::PluginRegistrar* registrar,
    int32_t id,
    std::string viewType,
    int32_t direction,
    double top,
    double left,
    double width,
    double height,
    const std::vector<uint8_t>& params,
    std::string assetDirectory,
    FlutterDesktopEngineRef engine,
    PlatformViewAddListener addListener,
    PlatformViewRemoveListener removeListener,
    void* platform_view_context) {
  auto plugin = std::make_unique<SkyplayRenderSurface>(
      id, std::move(viewType), direction, top, left, width, height, params,
      std::move(assetDirectory), engine, addListener, removeListener,
      platform_view_context);
  registrar->AddPlugin(std::move(plugin));
}

SkyplayRenderSurface::SkyplayRenderSurface(int32_t id,
                                   std::string viewType,
                                   int32_t direction,
                                   double top,
                                   double left,
                                   double width,
                                   double height,
                                   const std::vector<uint8_t>& params,
                                   std::string assetDirectory,
                                   FlutterDesktopEngineState* state,
                                   PlatformViewAddListener addListener,
                                   PlatformViewRemoveListener removeListener,
                                   void* platform_view_context)
    : PlatformView(id,
                   std::move(viewType),
                   direction,
                   top,
                   left,
                   width,
                   height),
      id_(id),
      view_(state->view_controller->view),
      width_(static_cast<int>(width)),
      height_(static_cast<int>(height)),
      callback_(nullptr),
      platformViewsContext_(platform_view_context),
      removeListener_(removeListener),
      flutterAssetsPath_(std::move(assetDirectory))
{
#if 0
  SPDLOG_TRACE("++SkyplayRenderSurface::SkyplayRenderSurface");
  auto& codec = flutter::StandardMessageCodec::GetInstance();
  const auto decoded = codec.DecodeMessage(params.data(), params.size());
  const auto& creationParams =
      std::get_if<flutter::EncodableMap>(decoded.get());

  std::string access_token;
  std::string module;
  bool map_flutter_assets{};
  std::string asset_path;
  std::string cache_folder;
  std::string misc_folder;

  for (const auto& [fst, snd] : *creationParams) {
    if (auto key = std::get<std::string>(fst); key == "access_token") {
      if (std::holds_alternative<std::string>(snd)) {
        access_token = std::get<std::string>(snd);
      }
    } else if (key == "map_flutter_assets") {
      if (std::holds_alternative<bool>(snd)) {
        map_flutter_assets = std::get<bool>(snd);
      }
    } else if (key == "asset_path") {
      if (std::holds_alternative<std::string>(snd)) {
        asset_path = std::get<std::string>(snd);
      }
    } else if (key == "cache_folder") {
      if (std::holds_alternative<std::string>(snd)) {
        cache_folder = std::get<std::string>(snd);
      }
    } else if (key == "misc_folder") {
      if (std::holds_alternative<std::string>(snd)) {
        misc_folder = std::get<std::string>(snd);
      }
    }
  }

  if (map_flutter_assets) {
    asset_path = flutterAssetsPath_;
  }

  if (!LibSkyplayRender::IsPresent()) {
    spdlog::error("[SkyplayRenderViewPlugin] libskyplay_render.so missing");
    return;
  }
  if (LibSkyplayRender::kExpectedSurfaceApiVersion !=
      LibSkyplayRender->SurfaceGetInterfaceVersion()) {
    spdlog::error("[SkyplayRenderViewPlugin] unexpected interface version: {}",
                  LibSkyplayRender->SurfaceGetInterfaceVersion());
    return;
  }

  /// Setup Native Window pointers
  display_ = view_->GetDisplay()->GetDisplay();
  auto compositor = view_->GetDisplay()->GetCompositor();
  surface_ = wl_compositor_create_surface(compositor);

  egl_display_ = eglGetDisplay(display_);
  assert(egl_display_);
  egl_window_ = wl_egl_window_create(surface_, width_, height_);
  assert(egl_window_);

  native_window_ = {
      .wl_display = display_,
      .wl_surface = surface_,
      .egl_display = egl_display_,
      .egl_window = egl_window_,
      .width = static_cast<uint32_t>(width_),
      .height = static_cast<uint32_t>(height_),
  };

  context_ = LibSkyplayRender->SurfaceInitialize(
      access_token.c_str(), static_cast<int>(width), static_cast<int>(height),
      &native_window_, asset_path.c_str(), cache_folder.c_str(),
      misc_folder.c_str());
  assert(context_);
  SPDLOG_DEBUG("Context: {}", fmt::ptr(context_));

  // Sub Surface
  auto sub_compositor = view_->GetDisplay()->GetSubCompositor();
  parent_surface_ = view_->GetWindow()->GetBaseSurface();
  subsurface_ = wl_subcompositor_get_subsurface(sub_compositor, surface_,
                                                parent_surface_);

  wl_subsurface_set_desync(subsurface_);

  addListener(platformViewsContext_, id, &platform_view_listener_, this);
  SPDLOG_TRACE("--SkyplayRenderSurface::SkyplayRenderSurface");
#endif
}

SkyplayRenderSurface::~SkyplayRenderSurface() {
  SPDLOG_TRACE("++SkyplayRenderSurface::~SkyplayRenderSurface");
  SPDLOG_TRACE("--SkyplayRenderSurface::~SkyplayRenderSurface");
}

void SkyplayRenderSurface::on_frame(void* data,
                                wl_callback* callback,
                                const uint32_t /* time */) {
  const auto obj = static_cast<SkyplayRenderSurface*>(data);

  obj->callback_ = nullptr;

  if (callback) {
    wl_callback_destroy(callback);
  }

  obj->DrawFrame();

  if (obj->subsurface_ == nullptr)
    return;

  // Z-Order
  /// Place below or we miss mouse/touch
  wl_subsurface_place_below(obj->subsurface_, obj->parent_surface_);

  obj->callback_ = wl_surface_frame(obj->surface_);
  wl_callback_add_listener(obj->callback_, &SkyplayRenderSurface::frame_listener,
                           data);

  wl_subsurface_set_position(obj->subsurface_, obj->left_, obj->top_);

  wl_surface_commit(obj->surface_);
}

const wl_callback_listener SkyplayRenderSurface::frame_listener = {.done =
                                                                   on_frame};

void SkyplayRenderSurface::Resize(int32_t width, int32_t height) {
  SPDLOG_TRACE("[SkyplayRenderView] Resize: {} {}", width, height);
  width_ = width;
  height_ = height;
}

void SkyplayRenderSurface::Dispose() {
  // TODO
  //LibSkyplayRender->SurfaceDeInitialize(context_);
  context_ = nullptr;

  if (callback_) {
    wl_callback_destroy(callback_);
    callback_ = nullptr;
  }

  if (subsurface_) {
    wl_subsurface_destroy(subsurface_);
    subsurface_ = nullptr;
  }

  if (egl_window_) {
    wl_egl_window_destroy(egl_window_);
    egl_window_ = nullptr;
  }

  if (surface_) {
    wl_surface_destroy(surface_);
    surface_ = nullptr;
  }
  removeListener_(platformViewsContext_, id_);
}

void SkyplayRenderSurface::DrawFrame() {
  if (!context_)
    return;

  if (dispose_pending_) {
    dispose_pending_ = false;
    Dispose();
    return;
  }

  // TODO
  //LibSkyplayRender->SurfaceDrawFrame(context_);
}

void SkyplayRenderSurface::SetOffset(int32_t left, int32_t top) {
  SPDLOG_DEBUG("[SkyplayRenderSurface] SetOffset: left: {}, top: {}", left, top);
  wl_subsurface_set_position(subsurface_, left, top);
  if (!callback_) {
    on_frame(this, callback_, 0);
  }
}

void SkyplayRenderSurface::on_resize(double width, double height, void* data) {
  SPDLOG_TRACE("[SkyplayRenderSurface] on_resize: {} {}", width, height);
  const auto plugin = static_cast<SkyplayRenderSurface*>(data);
  plugin->Resize(static_cast<int32_t>(width), static_cast<int32_t>(height));
}

void SkyplayRenderSurface::on_set_direction(int32_t direction, void* data) {
  SPDLOG_TRACE("[SkyplayRenderSurface] on_set_direction: {}", direction);
  if (!data) {
    return;
  }
  static_cast<SkyplayRenderSurface*>(data)->direction_ = direction;
}

void SkyplayRenderSurface::on_set_offset(const double left,
                                     const double top,
                                     void* data) {
  (void)left;
  (void)top;
  SPDLOG_TRACE("[SkyplayRenderSurface] on_set_offset: left: {}, top: {}", left,
               top);
  if (!data) {
    return;
  }
  static_cast<SkyplayRenderSurface*>(data)->SetOffset(static_cast<int32_t>(left),
                                                  static_cast<int32_t>(top));
}

void SkyplayRenderSurface::on_touch(const int32_t action,
                                const int32_t point_count,
                                const size_t point_data_size,
                                const double* /* point_data */,
                                void* /* data */) {
  (void)action;
  (void)point_count;
  (void)point_data_size;
  SPDLOG_TRACE(
      "[SkyplayRenderSurface] on_touch: action: {}, point_count: {}, "
      "point_data_size: {}",
      action, point_count, point_data_size);
}

void SkyplayRenderSurface::on_dispose(bool /* hybrid */, void* data) {
  const auto obj = static_cast<SkyplayRenderSurface*>(data);

  /// Dispose on the next frame callback
  obj->dispose_pending_ = true;
}

const platform_view_listener SkyplayRenderSurface::platform_view_listener_ = {
    .resize = on_resize,
    .set_direction = on_set_direction,
    .set_offset = on_set_offset,
    .on_touch = on_touch,
    .dispose = on_dispose,
    .accept_gesture = nullptr,
    .reject_gesture = nullptr,
};

}  // namespace skyplay_render_view_plugin
