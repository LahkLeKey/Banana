/**
 * OverlayStack.tsx — Named z-index overlay slot system.
 *
 * Spec 261 T004: Overlay slot system with four named layers:
 *   HUD    z-index 10  — in-game status, mini-map, crosshair
 *   Menu   z-index 20  — pause/main menu panels
 *   Modal  z-index 30  — confirmations, prompts, blocking dialogs
 *   Debug  z-index 40  — FPS counter, telemetry, error banners
 *
 * Usage:
 *   <OverlayStack hud={<HudOverlay />} debug={<FpsOverlay />} />
 *
 * All overlay containers use pointer-events: none by default so unhandled
 * clicks fall through to the WASM canvas. Set data-overlay="true" on
 * interactive overlay children to receive pointer events.
 */

import type { ReactNode } from "react";

export const OVERLAY_Z = {
  hud: 10,
  menu: 20,
  modal: 30,
  debug: 40,
} as const;

interface OverlaySlotProps {
  zIndex: number;
  children: ReactNode;
}

function OverlaySlot({ zIndex, children }: OverlaySlotProps) {
  return (
    <div
      style={{
        position: "fixed",
        inset: 0,
        zIndex,
        pointerEvents: "none",
      }}
    >
      {children}
    </div>
  );
}

export interface OverlayStackProps {
  hud?: ReactNode;
  menu?: ReactNode;
  modal?: ReactNode;
  debug?: ReactNode;
}

export function OverlayStack({ hud, menu, modal, debug }: OverlayStackProps) {
  return (
    <>
      {hud && <OverlaySlot zIndex={OVERLAY_Z.hud}>{hud}</OverlaySlot>}
      {menu && <OverlaySlot zIndex={OVERLAY_Z.menu}>{menu}</OverlaySlot>}
      {modal && <OverlaySlot zIndex={OVERLAY_Z.modal}>{modal}</OverlaySlot>}
      {debug && <OverlaySlot zIndex={OVERLAY_Z.debug}>{debug}</OverlaySlot>}
    </>
  );
}
