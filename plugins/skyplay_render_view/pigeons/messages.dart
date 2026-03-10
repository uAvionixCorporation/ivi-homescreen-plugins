import 'package:pigeon/pigeon.dart';

@ConfigurePigeon(PigeonOptions(
    dartOut: 'lib/src/messages.g.dart',
    cppOptions: CppOptions(
        headerOut: 'messages.g.h',
        sourceOut: 'messages.g.cc',
        namespace: 'skyplay_render_view'
    ),
))

@HostApi()
abstract class SkyplayApi {
  void initialize();
  int getMapTextureHandle();
  int getTerrainTextureHandle();
}
