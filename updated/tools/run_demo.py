#!/usr/bin/env python3
"""Download pinned OpenCV sample photographs and run the compiled tracker."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import urllib.request

SAMPLES = {
    "box.png": "1094629c1e2ebbc8a9ae2c9c63e18c1dff0f5f1481146e02f3b11a3d30feb20a",
    "box_in_scene.png": "8b0225ff76244a42bd1400c0904f8b7afea7d97b7d8115495e42e98ad347bd51",
    "fruits.jpg": "9c031d80a1c52da5eca790db896baffec6a7e52bf786cdb7bbfca5c7f880e6a1",
}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", type=Path, required=True, help="Built marker_tracker executable")
    parser.add_argument("--output", type=Path, required=True, help="New output directory")
    args = parser.parse_args()
    exe = args.exe.resolve()
    if not exe.is_file():
        parser.error("Build marker_tracker first; --exe must name its executable")
    args.output.mkdir(parents=True, exist_ok=False)
    inputs = args.output / "inputs"
    inputs.mkdir()
    provenance = []
    for filename, digest in SAMPLES.items():
        url = "https://raw.githubusercontent.com/opencv/opencv/4.12.0/samples/data/" + filename
        with urllib.request.urlopen(url, timeout=30) as response:
            data = response.read(2_000_000)
        if hashlib.sha256(data).hexdigest() != digest:
            raise ValueError("Sample SHA256 mismatch: " + filename)
        (inputs / filename).write_bytes(data)
        provenance.append({"name": filename, "url": url, "sha256": digest, "bytes": len(data)})
    (args.output / "inputs.json").write_text(json.dumps(provenance, indent=2) + "\n")
    def run(*parts):
        subprocess.run([str(exe), *map(str, parts)], check=True)
    run("selftest")
    model = args.output / "model"
    run("train", inputs / "box.png", model)
    run("image", model, inputs / "box_in_scene.png", args.output / "real-photo.png", args.output / "real-photo.json")
    run("image", model, inputs / "fruits.jpg", args.output / "unrelated.png", args.output / "unrelated.json")
    run("demo", model, args.output / "synthetic-replay")
    run("video", model, args.output / "synthetic-replay" / "input.avi", args.output / "video-replay.avi", args.output / "video-replay.json")
    print("Results saved in", args.output.resolve())

if __name__ == "__main__":
    main()
