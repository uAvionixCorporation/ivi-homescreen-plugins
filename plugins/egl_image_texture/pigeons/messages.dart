import 'package:pigeon/pigeon.dart';

@ConfigurePigeon(PigeonOptions(
    dartOut: 'lib/src/egl_image_texture_messages.g.dart',
    cppOptions: CppOptions(
        headerOut: 'messages.g.h',
        sourceOut: 'messages.g.cc',
        namespace: 'egl_image_texture'
    ),
))

@HostApi()
abstract class EglImageTextureApi {
  int getNativeDisplay();
  int registerEglImage(int eglImage);
  void markTextureAvailable();
}
