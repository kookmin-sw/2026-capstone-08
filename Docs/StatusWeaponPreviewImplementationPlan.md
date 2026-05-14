# Status Weapon Preview Implementation Plan

## Summary
The Status weapon panel must render the player's currently equipped weapon, not a shared or level-placed preview actor. The preview must keep a square, high-resolution render target, preserve the weapon aspect ratio, and automatically frame weapons with different sizes.

## Current Problem
- The Status panel previously displayed `/Game/UI/PreviewActor/M_UI_WeaponPreview`.
- That material samples the shared `/Game/UI/PreviewActor/RT_WeaponPreview`.
- Any actor that writes to the same render target can overwrite the panel image.
- A level-placed `BP_WeaponPreviewActor` can therefore make the Status panel show the preview actor's default weapon instead of the currently equipped weapon.
- Large or unusually shaped weapons can be clipped because the preview uses the preview actor's existing camera and mesh placement without fitting the selected weapon bounds.

## Weapon Source Rule
The preview source must come from gameplay equipment data in this order:

1. Current equipped weapon actor from `UMortisPlayerCombatComponent::GetPlayerCurrentEquippedWeapon()`.
2. The equipped weapon actor's `AMortisWeaponBase::GetWeaponMesh()`.
3. Other valid static or skeletal mesh components on the equipped weapon actor.
4. Weapon row lookup by the current equipped weapon tag.
5. Weapon row lookup by the equipped weapon actor class.
6. Weapon row `WeaponClass` default object mesh as the final fallback.

The UI must not use a manually placed level preview actor as the weapon source.

## Render Ownership Rule
- The Status widget owns its preview render target.
- The render target is created at runtime as a transient `UTextureRenderTarget2D`.
- Target resolution should be square, starting at `1024 x 1024`.
- `BP_WeaponPreviewActor` is still used as the rendering actor, but it is spawned offscreen by the Status widget.
- The spawned preview actor's `SceneCaptureComponent2D` writes to the widget-owned render target.
- The Status panel must display only the widget-owned render target path, either directly or through a Status-specific UI material.
- The Status panel should not rely on the shared `M_UI_WeaponPreview -> RT_WeaponPreview` display path.

This prevents other systems, level-placed actors, or editor test actors from overwriting the Status weapon preview.

## Preview Actor Safety
The preview actor and copied preview components must not affect gameplay:

- Spawn offscreen.
- Disable actor collision.
- Disable primitive collision on all preview components.
- Do not grant abilities.
- Do not bind hit delegates.
- Do not activate trails.
- Do not mutate inventory or equipment state.
- Destroy the spawned preview actor when the Status widget is destroyed or when no weapon is available.

## Weapon Size And Framing Rule
Each weapon can have different bounds, length, pivot, and component type. The preview must fit the selected weapon dynamically instead of relying on one fixed camera distance.

Implementation approach:

1. Apply the selected static or skeletal mesh to the preview actor's preview mesh component.
2. Select the active preview component:
   - `StatusWeaponPreviewStaticMesh` for static meshes.
   - `StatusWeaponPreviewSkeletalMesh` for skeletal meshes.
3. Reset the active component scale before measuring bounds.
4. Calculate the active component bounds after the mesh is assigned.
5. Move the component so the bounds center sits in front of the scene capture camera.
6. Use orthographic capture for the Status preview.
7. Set orthographic width from the largest bounds dimension:
   - `OrthoWidth = MaxBoundsDimension * Padding`
   - Suggested padding: `1.15` to `1.25`
   - Apply a minimum width so tiny weapons do not overfill the frame.
8. Keep the render target square and the UMG image square so the weapon is not stretched.
9. Clip the image inside the preview area.

This makes long swords, short weapons, thick weapons, static meshes, and skeletal meshes fit the same panel without manual per-weapon camera tuning.

## Background And Isolation Rule
- The scene capture should use the show-only list for the spawned preview actor.
- The capture should not render the level sky, floor, player, enemies, or placed preview actors.
- The preview area should use the panel background behind the rendered weapon.
- If a dark backing is needed, it should be part of the UI panel or the preview actor background, not the gameplay level.

## Transparent Background Rule
- Show-only capture prevents level objects from appearing in the preview, but it does not by itself remove empty render-target pixels.
- Empty render-target pixels can still appear as a black rectangle if the UI displays the render target directly.
- The final Status weapon preview should display the weapon over the Status panel without a visible render-target box.
- Use a Status-specific UI material named `M_UI_StatusWeaponPreview`.
- The material must expose a texture parameter named `WeaponPreviewTexture`.
- The widget-owned render target should be assigned to `WeaponPreviewTexture` at runtime through a dynamic material instance.
- The material opacity should primarily use the render target alpha as an inverted mask: `Opacity = 1 - Alpha`.
- The material should also keep a conservative RGB fallback mask so the weapon remains visible if a capture source or platform returns an alpha channel that does not separate weapon pixels from empty pixels.
- The scene capture should use a capture source that provides the alpha needed by this mask, such as `SCS_SceneColorHDR` with inverse opacity in alpha.
- Do not reuse the shared `M_UI_WeaponPreview -> RT_WeaponPreview` path for the transparent Status panel preview.

## Implemented Code Notes
- `UMortisInventoryStatusPageWidget` now creates a widget-owned transient render target for the Status weapon preview.
- The current implementation displays the widget-owned render target through `M_UI_StatusWeaponPreview`, not the shared `M_UI_WeaponPreview -> RT_WeaponPreview` path.
- `M_UI_StatusWeaponPreview` samples the widget-owned render target through the `WeaponPreviewTexture` parameter.
- Its opacity uses `Max(1 - Alpha, RGBBrightnessFallback)` so empty black capture pixels do not draw as a box, while weapon pixels can still appear if the captured alpha channel is not useful.
- The spawned preview actor's scene capture is forced to:
  - `PrimitiveRenderMode = PRM_UseShowOnlyList`
  - `CaptureSource = SCS_SceneColorHDR`
  - `CompositeMode = SCCM_Overwrite`
  - manual capture only
- The original `BP_WeaponPreviewActor` capture component is disabled for the Status panel path.
- Runtime-only Status components are created on the spawned preview actor:
  - `StatusWeaponPreviewStaticMesh`
  - `StatusWeaponPreviewSkeletalMesh`
  - `StatusWeaponPreviewCapture`
  - `StatusWeaponPreviewLight`
- The show-only list is narrowed to the active Status weapon mesh component, not the whole level, the original preview mesh, or any level-placed preview actor.
- Atmosphere, fog, sky lighting, volumetric fog, and volumetric cloud show flags are disabled on the Status capture.
- A local point light is created near the offscreen preview mesh so the weapon is visible without affecting the playable level.
- Preview framing is calculated from the active mesh bounds:
  - static meshes use `StatusWeaponPreviewStaticMesh`
  - skeletal meshes use `StatusWeaponPreviewSkeletalMesh`
  - the capture switches to orthographic projection
  - orthographic width is based on the larger projected bounds dimension with padding
  - camera location is recentered on the weapon bounds origin

## Name And Grade Rule
- Prefer `FMortisWeaponRow::WeaponName`.
- Prefer `FMortisWeaponRow::Grade`.
- If the inventory subsystem has no current weapon row, resolve the row from:
  - current equipped weapon tag
  - equipped weapon actor class
- Do not display raw actor names such as `BP_Player_Weapon_DarkIron_C_0`.
- If no data row can be resolved, use `Equipped Weapon` as the fallback name.

## Validation Plan
- Remove any manually placed `BP_WeaponPreviewActor` from test levels unless the level specifically needs it for unrelated testing.
- Compile C++ after render target ownership and framing changes.
- Open and compile `WBP_InventoryPage_Status`.
- Test at 16:9 resolution.
- Verify the Status panel shows the currently equipped weapon.
- Verify changing equipped weapons updates the preview source.
- Verify weapon name and grade match the weapon data row.
- Verify the preview is not overwritten by a level-placed preview actor.
- Verify long and short weapons fit without stretching or clipping.
- Verify static mesh and skeletal mesh weapons both render.
- Verify no gameplay collision, hit, ability, trail, or inventory side effects occur.
- Verify the transparent material path removes the black preview rectangle while keeping the weapon visible.
- Verify `M_UI_StatusWeaponPreview` uses the widget-owned render target, not the shared `RT_WeaponPreview`.

## Current Stop Point
Review the transparent-background result in PIE before further visual tuning.
