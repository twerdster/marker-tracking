# marker-tracking: maintenance context

Read this file first, then `SKILL.md`, then the relevant README and code. Current update: 2026-11-14.

## Project and preserved original

Alex Kemarsky's project trained classification trees to recognize a planar image marker and fitted a projective transform for camera tracking. Aaron Wetzler supervised it. The intended iPod port was not completed.

The 83 original text files in `source/` are unchanged. Historical entrypoints are `training-2010/Tracking.sln` and `tracking-2008/WebCamCSharp.sln`; original instructions remain in `source/README - usage.txt`. Windows/MFC, old .NET/camera dependencies, omitted FAST/RANSAC files and `C:\Temp` assumptions prevent an as-provided modern build. The recorded Mac compile attempt stops at missing `afxwin.h`; this is not a successful historical application build.

The root README remains the historical project reference, with only an authorized top `New!` badge/link and historical-snapshot qualifier added. Its old build/validation status describes the recovered Windows snapshot, not `updated/`.

## Updated implementation

`updated/` contains an adapted copy of the student's `SearchTree.cpp/h`, a new C++17/OpenCV frontend, portable model metadata, tests, real-photo results and explicitly synthetic video evidence. The ternary ±10 pixel-pair classifier, depth-eight partitioning and quantized leaf voting remain substantive student-derived code. Fixes include no-valid-split fallback, integer-overflow prevention, portable headers, and bounded validation before tree deserialization.

This is more than a wrapper: OpenCV's FAST-based ORB keypoint detector supplies scale/orientation-normalized patches; no ORB descriptors or SIFT matcher are used. OpenCV replaces missing RANSAC fitting, and the new frontend supplies augmentation, correspondence filtering, image/video/camera IO and overlays. The original `FastStable`, `Matcher`, C# UI and iPod application are not compiled. See `updated/PORTING.md` for exact retained/replaced components and parameter changes.

## Commands and recorded evidence

Build with CMake and OpenCV; the configured minimum is 4.5 and only 4.12.0 was tested in task-local macOS/Linux builds; the suggested Ubuntu/Debian system-package installation is untested; see `updated/README.md` and `updated/BUILD.md`. `marker_tracker` commands: `train`, `image`, `video`, `camera`, `demo`, `selftest`. The optional standard-library Python downloader is `updated/tools/run_demo.py`; `updated/tests/check_results.py` checks its saved results.

Mac Release and address/undefined-behavior sanitizer runs passed the tree regressions and full short demo: saved model, real-photo matching, unrelated-image rejection, 60 synthetic frames and decoded-video replay. The real separate photo produced 16 inliers and 1.546px inlier fit RMSE. Both Mac runs detected all 48 marker-present synthetic frames and no markers in 12 absent frames; known-transform corner RMSE was 5.920px. These are development fixtures used in preprocessing/threshold selection, not held-out benchmarks. Inlier fit error is not external ground-truth pose error.

The installed Homebrew OpenCV could compile but failed to load removed FFmpeg/OpenEXR dependency names. Validation used isolated OpenCV4.12 instead, without changing global packages. Exact dependency/build and scratch-data paths are retained in the private agent handoff, rather than the published validation record. Forest training was actively supervised and completed; no job was left running. The independent Linux Release build also passed CTest and the full image/video pipeline: 13/33 real-photo inliers, 1.075px inlier fit, 48/48 visible synthetic detections, 0/12 absent false positives and 7.443px known-corner RMSE. The raw records and platform differences are in `updated/VALIDATION.json` and `updated/validation/linux/`. No Linux sanitizer, live-camera or mobile run is claimed.

## Limits and maintenance

This is planar marker detection per frame, without temporal tracking or calibrated 3D pose. A local camera viewer is implemented, but saved-data proof does not validate a live camera session. Feature-poor/repetitive targets, severe viewpoint changes, blur and occlusion remain limitations. Old model directories are not directly compatible. A single unrelated photo and generated absence frames do not establish a general false-positive rate. One CTest entry groups several tree regression assertions, not independent tracking benchmarks.

Preserve original source hashes, attribution, notices and historical assets. Keep implementation changes and all modern details under `updated/`; the root historical body must remain unchanged except the top navigation and its historical-snapshot qualifier. Before every commit, review/synchronize this file and `SKILL.md`, run affected checks, inspect relative links/images and confirm source preservation. No package bundles these docs, so no released archive rebuild is required.

## Permission and prior editorial records

On 2026-11-12 Alex Kemarsky gave Aaron permission to use the recovered project code. This is his response only, not a new blanket license or rights to withheld third-party dependencies. Private correspondence remains outside the repository. This port does not copy the withheld camera, FAST or Peter Kovesi RANSAC files and does not relicense original code.

The historical 2026-11-12 and 2026-11-13 documentation revisions each received two actual Claude editorial rounds. Those reviews added no runtime coverage. On 2026-11-14 two actual Claude review rounds covered the revival. Useful wording, build-command compatibility and evidence qualifications were checked against source and saved stdout and applied. These reviews added no runtime coverage.
