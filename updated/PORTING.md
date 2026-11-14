# Implementation and preservation

Original student: Alex Kemarsky; supervision: Aaron Wetzler. New work in this directory is a 2026 port and new frontend, distinct from the original Windows programs. The 83 files in [`../source/`](../source/) are unchanged; their hashes are recorded in [the source manifest](validation/original-source-sha256.json).

## What is retained

`src/SearchTree.cpp` and `src/SearchTree.h` are adapted copies of `source/training-2010/TrackingDll/SearchTree.cpp` and `SearchTree.h`. The classifier still learns ternary pixel-pair intensity comparisons on 64×64 grayscale patches, uses thresholds −10 and +10, chooses splits by weighted histogram impurity, stops at depth 8 or partitions smaller than 50 samples, and sums leaf histograms normalized to 1,024 integer votes. This is the student's classification-tree algorithm, compiled as C++17 rather than replaced by descriptor matching.

The updated tracker uses 32 newly trained trees instead of the historical configuration of 100. It trains on 96 transformed patches per selected marker point and evaluates all trees for each candidate patch. The original staged inference loop skipped one tree and reused its maximum/runner-up state; the frontend instead computes a fresh maximum and runner-up after all tree votes. It keeps the best correspondence per marker class before fitting the projective transform. Each candidate starts with a fresh 300-bin vote histogram, matching the original classifier’s maximum class count; the new frontend selects at most 120 marker points.

## What changed

- Replaced Windows/MFC precompiled headers and the `byte` typedef with standard C++ headers and a name that does not conflict with C++17 `std::byte`.
- Fixed a training case that left split coordinates uninitialized when no candidate rule created three nonempty partitions: that node now becomes a histogram leaf. Prevented integer overflow in the impurity calculation and split nonemptiness check.
- Retained the original tree text writer and node reconstruction, but added grammar, integer range, class count, depth and file-size validation before parsing. Replaced the Windows-specific reader, its `char`/EOF loop and leaked temporary buffer. Model metadata is separate YAML; old Windows model directories are not directly compatible.
- Replaced omitted FAST and RANSAC implementations with OpenCV. The detector uses ORB's FAST image pyramid and intensity-centroid orientation, **without computing or matching ORB descriptors**. Patches are normalized by the candidate's orientation and pyramid scale, and sampled at five relative scales; this is a changed feature-input pipeline. Random patch rotation, scaling, shifts, brightness and contrast are generated with OpenCV rather than the original Windows training code.
- Added a new image/video/camera frontend, portable model directories, JSON results, independently generated replay transforms, and rejection of unsupported/implausible homographies. It requires at least eight inliers and an inlier fraction of at least 0.35; RANSAC's threshold is 3 image pixels. An overlay is drawn only after these checks pass.

The C#/Windows Forms camera application, original stability-selection procedure, `FastStable`, `Matcher`, `ProjTransform`, and omitted dependencies are not compiled by this port. It is a partial modernization with an adapted original classifier and new surrounding software, not an unchanged build of the full historical application. No iPod/iPhone port is included.

## Dependencies and terms

OpenCV is an external dependency. Version 4.12.0 is distributed under Apache-2.0; its bundled dependencies have their own notices. See [the upstream license](https://github.com/opencv/opencv/blob/4.12.0/LICENSE). No files from the withheld Touchless, EasyWebCam, bundled FAST, or Peter Kovesi RANSAC material were copied into the port. The original repository's author permission and absence of a blanket license remain unchanged; this directory does not relicense the original algorithm or repository.

The reproducible demo fetches `box.png`, `box_in_scene.png`, and `fruits.jpg` from OpenCV's pinned 4.12.0 sample tree, verifies their SHA256 hashes, and records download URLs. The real scene pair is also used in [OpenCV's homography tutorial](https://docs.opencv.org/4.12.0/d1/de0/tutorial_py_feature_homography.html). That tutorial uses SIFT; this port does not use its matching implementation. Demo overlays and replay images are newly generated, not historical project results. The replay is a synthetic geometric transformation of a real photograph, not a camera recording.
