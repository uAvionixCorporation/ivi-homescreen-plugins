#pragma once

#include <flutter/event_channel.h>
#include <flutter/method_channel.h>
#include <flutter/plugin_registrar.h>

#include "skyplay_render_error.h"
#include "wayland/display.h"

namespace skyplay_render_view_plugin {
class SkyplayRenderTexture final : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrar* registrar);

  explicit SkyplayRenderTexture(flutter::PluginRegistrar* registrar);

  ~SkyplayRenderTexture() override;

  static ErrorOr<flutter::EncodableMap> Create(const std::string& access_token,
                                               bool map_flutter_assets,
                                               const std::string& asset_path,
                                               const std::string& cache_folder,
                                               const std::string& misc_folder,
                                               int interface_version);

  // Disallow copy and assign.
  SkyplayRenderTexture(const SkyplayRenderTexture&) = delete;
  SkyplayRenderTexture& operator=(const SkyplayRenderTexture&) = delete;

 private:
  std::unique_ptr<flutter::MethodChannel<>> channel_{};

  static void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue>& method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};
}  // namespace skyplay_render_view_plugin
