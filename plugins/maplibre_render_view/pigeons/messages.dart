import 'package:pigeon/pigeon.dart';

@ConfigurePigeon(PigeonOptions(
    dartOut: 'lib/src/maplibre_messages.g.dart',
    cppOptions: CppOptions(
        headerOut: 'messages.g.h',
        sourceOut: 'messages.g.cc',
        namespace: 'maplibre_render_view'
    ),
))

@HostApi()
abstract class MapLibreApi {
  int getNativeDisplay();
  int registerEglImage(int eglImage);
  void markTextureAvailable();
}
