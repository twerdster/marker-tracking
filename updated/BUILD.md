# Build notes

The normal build uses an installed OpenCV development package, as shown in [README.md](README.md). The configured minimum is 4.5; only 4.12.0 was tested on macOS and Linux. Both recorded platform runs used task-local OpenCV 4.12.0 builds; the suggested Ubuntu/Debian system-package installation was not tested. A successful CMake configuration alone does not prove its shared libraries can load: run CTest before training.

## macOS dependency mismatch observed

The Mac's installed Homebrew OpenCV 4.12.0 linked older FFmpeg and OpenEXR library names that were no longer installed. The first compiled executable therefore failed at dynamic loading. The validation used a task-local OpenCV 4.12.0 build instead; it did not change global Homebrew packages or create compatibility symlinks.

For an isolated build, extract the [OpenCV 4.12.0 source archive](https://github.com/opencv/opencv/archive/refs/tags/4.12.0.tar.gz), whose SHA256 is `44c106d5bb47efec04e531fd93008b3fcd1d27138985c5baf4eafac0e1ec9e9d`. Choose paths on a native filesystem for the source, build and install prefix. For example, in a separate work directory:

```sh
cmake -S opencv-4.12.0 -B opencv-build \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PWD/opencv-install" \
  -DBUILD_LIST=core,imgproc,imgcodecs,features2d,calib3d,videoio,highgui \
  -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_EXAMPLES=OFF \
  -DBUILD_opencv_apps=OFF -DWITH_FFMPEG=OFF -DWITH_OPENEXR=OFF \
  -DWITH_QT=OFF -DWITH_GSTREAMER=OFF -DWITH_VTK=OFF \
  -DWITH_TIFF=OFF -DWITH_WEBP=OFF -DWITH_AVIF=OFF \
  -DWITH_JASPER=OFF -DWITH_OPENJPEG=OFF -DWITH_1394=OFF \
  -DWITH_PROTOBUF=OFF -DWITH_OBSENSOR=OFF -DWITH_LAPACK=OFF \
  -DWITH_OPENCL=OFF -DWITH_IPP=OFF \
  -DBUILD_JPEG=ON -DBUILD_PNG=ON -DBUILD_ZLIB=ON
cmake --build opencv-build -j4
cmake --install opencv-build
```

Then build this repository with `-DOpenCV_DIR=/absolute/path/to/opencv-install/lib/cmake/opencv4`. This macOS build uses Cocoa/AVFoundation and OpenCV's Motion JPEG AVI codec; arbitrary MP4 inputs may require a different video backend. It is a dependency workaround for the recorded machine, not a universal requirement.

## Sanitizers

```sh
cmake -S updated -B updated/build-asan -DCMAKE_BUILD_TYPE=RelWithDebInfo -DMARKER_SANITIZE=ON
cmake --build updated/build-asan -j4
(cd updated/build-asan && ctest --output-on-failure)
python3 updated/tools/run_demo.py --exe updated/build-asan/marker_tracker --output /tmp/marker-demo-asan
```

Pass the same `OpenCV_DIR` when using a private dependency build. The tracker and adapted tree source are instrumented for address/undefined-behavior checking; externally built OpenCV libraries are not. Recorded results cover the entire short demonstration pipeline, not arbitrary model files or videos.

## Output and storage

Training produces `model.yml`, `marker.png`, and 32 tree text files. The demo refuses to reuse its output directory, and model creation refuses to overwrite an existing directory. Keep generated data outside the repository. Use a scratch directory for inputs, trained models, AVIs and temporary results; keep dependency builds on a native filesystem that supports ordinary compiler and build-tool operations. The tracker uses the CPU and does not require a GPU.
