# Maintaining marker-tracking

Read `SUMMARY.md`, then this file, then `README.md`, `updated/README.md`, `updated/PORTING.md`, `updated/VALIDATION.json`, relevant saved results and code before changing technical claims.

## Preservation

Keep all 83 `source/` files byte-exact. `updated/validation/original-source-sha256.json` records their hashes. Do not edit historical usage, project files, credits or withheld-dependency boundaries. The root README is a historical reference; its only 2026 revival changes are the top `New!` badge/link and the sentence identifying the sections below as the recovered Windows snapshot. Put modern implementation, setup, caveats and evidence in `updated/`.

The updated tree is student-derived code, not an independently replaced algorithm. Explain the new FAST pyramid/orientation preprocessing, augmentation, correspondence filters, OpenCV homography fitting and frontend. Never describe the old C#/MFC application as compiled, the old iPod port as restored, or the port as merely a wrapper. Do not add a blanket license based on one author's permission. Do not publish private correspondence or addresses.

## Build and verify

From repository root:

```sh
cmake -S updated -B updated/build -DCMAKE_BUILD_TYPE=Release
cmake --build updated/build -j4
(cd updated/build && ctest --output-on-failure)
python3 updated/tools/run_demo.py --exe updated/build/marker_tracker --output /tmp/marker-demo
python3 updated/tests/check_results.py /tmp/marker-demo
```

Use a new output directory. OpenCV development files and a C++17 compiler are required; CMake accepts version 4.5+, but validation covers 4.12.0 only, using task-local dependency builds on both macOS and Linux; the suggested Ubuntu/Debian system-package setup is untested. See `updated/BUILD.md` for `OpenCV_DIR`, isolated dependency configuration and `MARKER_SANITIZE=ON`. A configure/build pass is insufficient if shared libraries fail to load; run CTest and the relevant image/video path. No documentation packaging/released archive rebuild is involved.

`selftest` covers the constant-patch split failure, eight class bins, normalized histogram sum, save/load vote equivalence and five malformed-tree cases. One CTest entry groups those checks. The downloaded real photo pair, unrelated photo and generated perspective/occlusion/absence/reacquisition replay exercise the classifier and IO. `check_results.py` has fixture-specific regression bounds, not a general correctness guarantee. Use raw JSON to report actual counts, fit RMSE, independent known-corner RMSE and runtime scope. A rejected frame's inlier RMSE is `-1` (not measured), not perfect fit.

For substantive changes, run the affected tree/pipeline checks and sanitizer configuration. Preserve actual failures, recording the host, dependency version, commands, fixture hashes, source hashes and limits. Do not silently relax gates or add descriptor-matching fallbacks and still call the result the original tree tracker. Real pair and synthetic replay were development fixtures; new accuracy claims require separate data. C-library random generators and numeric libraries can change independently trained forests across platforms.

## Media and handoff

Keep current output clearly distinct from historical screenshots. `updated/assets/real-photo.png` is a real photograph with a newly computed overlay. The GIF/MP4 show synthetic motion of a real image; retain that label and show failed/absent frames without a stale boundary. Derive displayed counts from saved JSON. Target GIFs below2MB and MP4s below10MB; record transformation/provenance and file hashes in the media manifest.

The Mac used a private native OpenCV prefix because Homebrew's installed libraries had missing transitive dependencies; do not repair this by changing unrelated global packages or creating incompatible symlinks. Keep builds/venvs on native filesystems. Recheck the SSD's actual mount, identity, free space and task-scoped write access before large data writes. Keep exact local build and data paths in the private agent handoff; published validation records describe environments without local scratch paths.

Do not claim a live camera or mobile run from saved-data results. No GPU is needed. Supervise short forest fits through verified completion; do not leave an unmonitored training job.

Before committing, reread and synchronize `SUMMARY.md` and this file; check diff scope, source hashes, root historical body, Markdown links, image rendering and media sizes. Record actual external reviews accurately and do not infer additional runtime coverage from editorial review.
