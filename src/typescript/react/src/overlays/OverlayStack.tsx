/**
 * OverlayStack.tsx — Named z-index overlay slot system with focus-lock tracking.
 *
 * Spec 261 T004/T005: Overlay slot system with four named layers:
 *   HUD    z-index 10  — in-game status, mini-map, crosshair (pass-through, no focus lock)
 *   Menu   z-index 20  — pause/main menu panels (soft lock when open)
 *   Modal  z-index 30  — confirmations, prompts, blocking dialogs (hard lock always)
 *   Debug  z-index 40  — FPS counter, telemetry, error banners (pass-through, no focus lock)
 *
 * `overlayMode` is true when any focus-lock layer (menu/modal) has content.
 * Pass overlayMode to WasmViewport so it suspends scene input routing.
 *
 * All overlay containers use pointer-events: none by default so unhandled
 * clicks fall through to the WASM canvas. Children set data-overlay="true"
 * to receive pointer events.
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
  /** Called when focus-lock state changes (menu/modal open or close). */
  onOverlayModeChange?: (active: boolean) => void;
}

export function OverlayStack({ hud, menu, modal, debug, onOverlayModeChange }: OverlayStackProps) {
  // overlayMode = any focus-lock layer (menu or modal) is present
  const overlayMode = Boolean(menu) || Boolean(modal);

  // Notify parent synchronously on change
  onOverlayModeChange?.(overlayMode);

  return (
    <>
      {hud && <OverlaySlot zIndex={OVERLAY_Z.hud}>{hud}</OverlaySlot>}
      {menu && <OverlaySlot zIndex={OVERLAY_Z.menu}>{menu}</OverlaySlot>}
      {modal && <OverlaySlot zIndex={OVERLAY_Z.modal}>{modal}</OverlaySlot>}
      {debug && <OverlaySlot zIndex={OVERLAY_Z.debug}>{debug}</OverlaySlot>}
    </>
  );
}
