# Updated output assets

`real-photo.png` is the updated tree tracker's output on OpenCV 4.12.0 `box_in_scene.png`, after training from `box.png`. `unrelated.png` records rejection of `fruits.jpg`. Original sample URLs and SHA256 hashes are in [`../validation/macos-release/inputs.json`](../validation/macos-release/inputs.json). The OpenCV project's [license text](OPENCV-LICENSE.txt) is retained here with the sample-derived visual material; no blanket license is added to the recovered student code.

`replay.gif` and `replay.mp4` were generated from the recorded Mac Release run's synthetic perspective/partial-occlusion/absence/reacquisition sequence. They are new output, not an original camera recording. Green boundaries and yellow points are computed from the adapted classifier and OpenCV RANSAC; synthetic ground-truth corners are used only for the recorded error calculation. The GIF is resized to 600 pixels wide at 8 fps with 96 colors; the MP4 retains 720×480 at 12 fps. Neither changes the measured JSON results.

`new-badge.svg` is the same local text badge used by the other updated project guides. File sizes, hashes and transformations for the output media are recorded in [`../validation/media-manifest.json`](../validation/media-manifest.json).
