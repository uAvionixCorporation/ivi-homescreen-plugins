import 'package:pigeon/pigeon.dart';

@ConfigurePigeon(PigeonOptions(
    dartOut: 'lib/src/messages.g.dart',
    cppOptions: CppOptions(
        headerOut: 'messages.g.h',
        sourceOut: 'messages.g.cc',
        namespace: 'maplibre_render_view'
    ),
))

@HostApi()
abstract class MapLibreApi {
  void initialize();
  int getTextureHandle();
  void renderFrame();
}
