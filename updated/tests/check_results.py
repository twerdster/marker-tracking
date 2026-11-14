#!/usr/bin/env python3
"""Check the recorded demo's tracking behavior; these are scoped regression gates."""
import argparse
import json
from pathlib import Path


def check(path):
    root = Path(path)
    photo = json.loads((root / "real-photo.json").read_text())["frames"][0]
    unrelated = json.loads((root / "unrelated.json").read_text())["frames"][0]
    replay = json.loads((root / "synthetic-replay" / "metrics.json").read_text())
    video = json.loads((root / "video-replay.json").read_text())["frames"]
    assert photo["found"] == 1 and photo["inliers"] >= 8, "Real photograph not matched"
    assert 0 <= photo["inlier_rmse_pixels"] <= 3, "Real-photo inlier fit exceeded threshold"
    assert unrelated["found"] == 0, "Unrelated image produced a marker overlay"
    assert replay["present_frames"] == 48 and replay["absent_frames"] == 12
    assert replay["found_present_frames"] >= 44, "Too many synthetic fixture misses"
    assert replay["false_positive_frames"] == 0, "Absent marker falsely detected"
    assert 0 <= replay["corner_rmse_found_frames_pixels"] < 15, "Synthetic corner error exceeded regression bound"
    assert len(video) == 60, "Video decoder did not process all frames"
    assert sum(f["found"] for f in video) >= 44, "Decoded replay lost too many visible detections"
    assert all(not video[k]["found"] for k in range(36, 48)), "Decoded absence frames falsely detected"
    print("PASS: real photograph, unrelated rejection, known-transform replay, absence and video decoding")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("output")
    check(parser.parse_args().output)
