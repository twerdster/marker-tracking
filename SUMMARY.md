# marker-tracking: maintenance context

Read this file first, then SKILL.md and README.md. This context describes the repository as of 2026-11-13.

## Purpose and current source

Partial Windows marker training/tracking with C#/C++, classification trees, FAST references and RANSAC projective matching; intended iPod port was not completed.

83 unchanged original text files in source/, with tracking-2008/WebCamCSharp.sln, training-2010/Tracking.sln and source/README - usage.txt. Touchless/EasyWebCam, FAST and RANSAC dependencies were withheld pending terms review.

## Use and validation

Read source/README - usage.txt. Historical workflow: train marker with VS2010, then load marker/Tree0.dat and capture using VS2008. Projects expect C:\Temp and .NET 3.5/4.0/MFC dependencies.

Recovery inspected text/project inventory and original usage. No compiler, camera runtime, training job or tracking benchmark was run. Historical 640x480 capture is a handoff statement, not a current measurement.

Solutions cannot build as provided because references/headers/functions are missing. No updated port/notebook. Compiled disguised DLL/EXE handoff packages, trained trees, samples and user state were excluded.

## Documentation maintenance

The 2026-11-12 documentation revision organizes the README around implementation, available source, setup, recorded checks and limits. It preserves historical visuals and links, with captions separating historical results from updated examples. No runnable code, source provenance, pinned revisions, licenses or validation results changed in this revision. Recorded earlier tests were not rerun for the prose-only edits.

Preserve original source and authorship. New ports belong under updated/ and require their own scope/validation. Do not infer a blanket license from recovery. Review this file and SKILL.md before every commit and synchronize changes to workflow, commands, validation or status. Check relative links and figure rendering; never describe inventory counts or component tests as full-application validation.

Two actual Claude editorial review rounds on 2026-11-12 covered the README and related changed documentation, including context files, component guides and applicable algorithm/build notes or schematic text. Useful qualifications were checked against source, preserved project records and saved validation logs, then applied. Editorial review adds no runtime coverage.

## Author correspondence recorded on 2026-11-13

On 2026-11-12, Alex Kemarsky gave Aaron Wetzler permission to use the recovered project code. This records Alex Kemarsky's response only; it does not grant rights to the withheld third-party dependencies or establish a new repository-wide license. The incomplete build and validation limits above remain unchanged.

This update changes documentation only. Source files, pinned revisions, assets, licenses and recorded validation results are unchanged. Private correspondence remains outside the repository. No new build, runtime or numerical validation was performed for this update.

Two actual Claude editorial rounds on 2026-11-13 reviewed these author-correspondence notes and the related documentation/context changes. Minor factual and wording corrections were applied against the correspondence and existing repository records. This review and documentation update add no runtime validation.
