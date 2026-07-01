# fieldofchaos Graphite Imagegen Pipeline

## Scope

This pipeline is for still PNG artwork for the `fieldofchaos` project.

The rendering destination is:

```text
png/
```

Generated project PNG files must be saved in `png/` using the exact requested
filename and extension.

## Output Dimensions

Required generated PNG dimensions:

```text
1024 x 1024 pixels
```

Every generated PNG must be checked after it is copied into `png/`. If the
built-in image generator returns a different size, resize the final project
copy to `1024 x 1024` and verify it again before review.

## Rendering Path

Use the Codex built-in `image_gen` tool for production PNG artwork.

This path:

- does not require `OPENAI_API_KEY`;
- does not require local model inference;
- writes generated images under `/Users/barbalet/.codex/generated_images`;
- requires copying the chosen final PNG into `png/`.

Do not use local/offline placeholder renderers as final fieldofchaos artwork.
Do not leave project artwork only under `/Users/barbalet/.codex/generated_images`.

## Prompt Style Contract

A fieldofchaos graphite prompt should request:

- dense black-and-white hand-drawn graphite line art;
- strong cover-art composition with a clear central subject;
- heavy pencil cross-hatching, paper grain, tonal graphite shading, and rich
  scene-specific micro-detail;
- complete cinematic illustration, not a layout sketch, schematic, storyboard,
  CAD/vector preview, or sparse line drawing;
- no captions, readable text, logos, branded media covers, celebrities, UI,
  watermarks, anime, cartoon simplification, stick figures, or line-limb bodies.

## Generation Sequence

1. Write a specific prompt for the requested fieldofchaos PNG.
2. Generate the artwork with built-in `image_gen`.
3. Locate the newest generated PNG under
   `/Users/barbalet/.codex/generated_images`.
4. Copy the selected generated PNG into `png/` with the requested filename.
5. Verify the final file dimensions are exactly `1024 x 1024`.
6. If needed, resize the project copy to `1024 x 1024` and verify again.
7. Report the saved path and the verification result.

## Current Requested Render

Filename:

```text
png/cover.PNG
```

Required prompt:

```text
Three six sided dice caught in a swirling vertex with related swirling elements
around the three dice.
```
