# Marker tracking

<p align="center">
  <img src="assets/project-preview.jpg" alt="Top: a marker photograph and the same photograph held in front of a camera with detected feature points and a red tracked boundary. Bottom: a grid of rotated and warped grayscale image patches" width="420">
</p>

![Platform: Windows desktop](https://img.shields.io/badge/platform-Windows%20desktop-lightgrey)
![Code: C# + C++](https://img.shields.io/badge/code-C%23%20%2B%20C%2B%2B-blue)
![Source: partial snapshot](https://img.shields.io/badge/source-partial%20snapshot-orange)

This project developed desktop marker-training and camera-tracking code using image features, classification trees and a projective transform. It was intended as a step toward an iPod port, but that port was not completed. The recovered Windows application is partial and cannot build as provided.

Student: Alex Kemarsky. Supervision: Aaron Wetzler.

*The unchanged historical figure shows marker tracking and transformed image patches. It is not a result from a new run.*

## Original code

The [source snapshot](source/) contains 83 unchanged text files from the original handoff: C#/C++ code, Visual Studio project files and the student's [usage instructions](<source/README - usage.txt>). Per the usage notes, the two entrypoints are:

- `source/training-2010/Tracking.sln`: select a marker image, choose a training directory and generate the tree data.
- `source/tracking-2008/WebCamCSharp.sln`: load the marker and `Tree0.dat`, start camera capture and draw the tracked marker boundary.

`TrackingDll` handles feature detection, tree matching and RANSAC-based projective matching. FAST detection and RANSAC fitting rely on third-party files that were withheld (see below).

## Setup and historical usage

The handoff specifies Visual Studio 2010 for training and Visual Studio 2008 for tracking. The projects use Windows Forms/C#, native C++/MFC and mixed .NET 3.5/4.0 settings. They expect `C:\Temp` and a marker directory containing trained data. The [original usage guide](<source/README - usage.txt>) explains the menus and classifier/RANSAC thresholds.

The solutions cannot build as provided. Touchless WebCamLib/WebCamWrapper, EasyWebCam components, bundled FAST detector code and RANSAC files credited to Peter Kovesi were withheld pending separate provenance/license review. They leave missing project references, headers and functions. `detectFast.cpp` is a MATLAB MEX helper requiring `mex.h`; its MATLAB version is unknown.

## Validation and limitations

Recovery checked the text/project inventory and preserved the original usage record. No historical compiler, camera session, training job or tracking benchmark was run. The handoff's statement that VS2008 camera capture reached 640-by-480 pixels, and its reports of VS2010 capture/VS2008 training problems, are the student's historical observations.

No binaries were tested: handoff packages with `.aa`/`.bb` files were identified as DLL/EXE material and excluded. Trained trees, sample images, user-specific state and unreviewed documents were also excluded. There is no updated implementation, notebook or supported modern setup recipe. Restoration requires the missing dependencies and their applicable reuse terms.

## Attribution

Original student credits and file notices remain intact. No blanket license is added. Authors can open an issue to contribute changes, correct attribution or request removal.

## Links

- [Project entry on Aaron Wetzler's website](https://twerdster.github.io/#marker-tracking)
