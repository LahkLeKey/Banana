/**
 * Spec 261: Overlay components Storybook catalog
 *
 * Visual reference and interactive demo for:
 * - HudOverlay (z-index 10, pass-through pointer events)
 * - MenuOverlay (z-index 20, soft focus lock)
 * - OverlayStack (z-index layer manager with focus-lock tracking)
 */

import type { Meta, StoryObj } from "@storybook/react";
import { useState } from "react";

import { HudOverlay } from "../../../react/src/overlays/HudOverlay";
import { MenuOverlay } from "../../../react/src/overlays/MenuOverlay";
import { OverlayStack } from "../../../react/src/overlays/OverlayStack";

// ── HudOverlay Stories ────────────────────────────────────────────────────────

const HudOverlayMeta: Meta<typeof HudOverlay> = {
  title: "Spec 261/Overlays/HudOverlay",
  component: HudOverlay,
  parameters: {
    layout: "fullscreen",
  },
};

export default HudOverlayMeta;

export const HudOverlayVisible: StoryObj<typeof HudOverlay> = {
  render: () => (
    <div style={{ position: "relative", width: "100%", height: "600px", background: "#1a1a1a" }}>
      {/* Simulated WASM viewport background */}
      <div
        style={{
          position: "absolute",
          inset: 0,
          background: "linear-gradient(45deg, #222, #333)",
          zIndex: 0,
        }}
      >
        <p style={{ color: "#666", margin: "20px", fontSize: "12px" }}>WASM Viewport (z-index: 0)</p>
      </div>

      {/* HUD overlay */}
      <HudOverlay />
    </div>
  ),
  parameters: {
    docs: {
      description: {
        story: "HUD overlay (z-index 10) displays pass-through pointer events. Useful for visual feedback that does not block WASM input.",
      },
    },
  },
};

// ── MenuOverlay Stories ───────────────────────────────────────────────────────

const MenuOverlayMeta: Meta<typeof MenuOverlay> = {
  title: "Spec 261/Overlays/MenuOverlay",
  component: MenuOverlay,
  parameters: {
    layout: "fullscreen",
  },
};

export const MenuOverlayStories = MenuOverlayMeta;

export const MenuOverlayClosed: StoryObj<typeof MenuOverlay> = {
  render: () => (
    <div style={{ position: "relative", width: "100%", height: "600px", background: "#1a1a1a" }}>
      <div
        style={{
          position: "absolute",
          inset: 0,
          background: "linear-gradient(45deg, #222, #333)",
          zIndex: 0,
        }}
      >
        <p style={{ color: "#666", margin: "20px", fontSize: "12px" }}>WASM Viewport (z-index: 0)</p>
      </div>

      <MenuOverlay open={false} />
    </div>
  ),
  parameters: {
    docs: {
      description: {
        story: "MenuOverlay closed state: returns null, no rendering. WASM viewport has full interaction control.",
      },
    },
  },
};

export const MenuOverlayOpen: StoryObj<typeof MenuOverlay> = {
  render: () => (
    <div style={{ position: "relative", width: "100%", height: "600px", background: "#1a1a1a" }}>
      <div
        style={{
          position: "absolute",
          inset: 0,
          background: "linear-gradient(45deg, #222, #333)",
          zIndex: 0,
        }}
      >
        <p style={{ color: "#666", margin: "20px", fontSize: "12px" }}>WASM Viewport (z-index: 0)</p>
      </div>

      <MenuOverlay open={true} />
    </div>
  ),
  parameters: {
    docs: {
      description: {
        story: "MenuOverlay open state (z-index 20): soft focus lock. WASM input is suspended; menu receives keyboard/pointer.",
      },
    },
  },
};

// ── OverlayStack Stories ──────────────────────────────────────────────────────

const OverlayStackMeta: Meta<typeof OverlayStack> = {
  title: "Spec 261/Overlays/OverlayStack",
  component: OverlayStack,
  parameters: {
    layout: "fullscreen",
  },
};

export const OverlayStackDefault = OverlayStackMeta;

export const OverlayStackViewportMode: StoryObj<typeof OverlayStack> = {
  render: () => {
    const [overlayMode, setOverlayMode] = useState(false);

    return (
      <div style={{ position: "relative", width: "100%", height: "600px", background: "#1a1a1a" }}>
        {/* Simulated WASM viewport */}
        <div
          style={{
            position: "absolute",
            inset: 0,
            background: "linear-gradient(45deg, #222, #333)",
            zIndex: 0,
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
          }}
        >
          <div style={{ color: "#666", textAlign: "center" }}>
            <p style={{ fontSize: "12px", marginBottom: "10px" }}>WASM Viewport (z-index: 0)</p>
            <p style={{ fontSize: "10px", color: "#555" }}>
              overlayMode: <strong style={{ color: overlayMode ? "#f00" : "#0f0" }}>{String(overlayMode)}</strong>
            </p>
          </div>
        </div>

        {/* Overlay stack with HUD + debug only (no focus-lock layers) */}
        <OverlayStack
          hud={
            <div
              style={{
                padding: "10px",
                background: "rgba(0, 100, 200, 0.8)",
                color: "#fff",
                fontSize: "12px",
              }}
            >
              HUD (z-index 10, pass-through)
            </div>
          }
          debug={
            <div
              style={{
                padding: "10px",
                background: "rgba(0, 0, 0, 0.5)",
                color: "#0f0",
                fontSize: "10px",
                fontFamily: "monospace",
              }}
            >
              Debug (z-index 40, pass-through)
            </div>
          }
          onOverlayModeChange={setOverlayMode}
        />
      </div>
    );
  },
  parameters: {
    docs: {
      description: {
        story: "OverlayStack in viewport mode: HUD + debug layers render (pass-through). overlayMode = false. WASM captures all input.",
      },
    },
  },
};

export const OverlayStackOverlayMode: StoryObj<typeof OverlayStack> = {
  render: () => {
    const [overlayMode, setOverlayMode] = useState(false);

    return (
      <div style={{ position: "relative", width: "100%", height: "600px", background: "#1a1a1a" }}>
        {/* Simulated WASM viewport, partially visible */}
        <div
          style={{
            position: "absolute",
            inset: 0,
            background: "linear-gradient(45deg, #222, #333)",
            zIndex: 0,
            opacity: overlayMode ? 0.3 : 1,
            display: "flex",
            alignItems: "center",
            justifyContent: "center",
          }}
        >
          <div style={{ color: "#666", textAlign: "center" }}>
            <p style={{ fontSize: "12px", marginBottom: "10px" }}>WASM Viewport (opacity: {overlayMode ? "0.3" : "1"})</p>
            <p style={{ fontSize: "10px", color: "#555" }}>
              overlayMode: <strong style={{ color: overlayMode ? "#f00" : "#0f0" }}>{String(overlayMode)}</strong>
            </p>
          </div>
        </div>

        {/* Overlay stack with modal (focus-lock active) */}
        <OverlayStack
          hud={
            <div
              style={{
                padding: "10px",
                background: "rgba(0, 100, 200, 0.8)",
                color: "#fff",
                fontSize: "12px",
              }}
            >
              HUD (z-index 10, pass-through)
            </div>
          }
          modal={
            <div
              style={{
                position: "fixed",
                inset: "20%",
                background: "rgba(50, 50, 50, 0.95)",
                border: "2px solid #0f0",
                borderRadius: "8px",
                padding: "20px",
                color: "#fff",
                zIndex: 30,
                boxShadow: "0 0 20px rgba(0, 0, 0, 0.8)",
              }}
            >
              <h3 style={{ margin: "0 0 10px 0" }}>Modal Overlay (z-index 30)</h3>
              <p style={{ fontSize: "12px", marginBottom: "10px" }}>Hard focus lock active</p>
              <button
                onClick={() => setOverlayMode(false)}
                style={{
                  padding: "8px 16px",
                  background: "#0f0",
                  color: "#000",
                  border: "none",
                  borderRadius: "4px",
                  cursor: "pointer",
                  fontWeight: "bold",
                }}
              >
                Close Modal
              </button>
            </div>
          }
          onOverlayModeChange={setOverlayMode}
        />
      </div>
    );
  },
  parameters: {
    docs: {
      description: {
        story: "OverlayStack in overlay mode: modal layer open with hard focus lock. overlayMode = true. WASM input suspended.",
      },
    },
  },
};

export const OverlayStackFullLayers: StoryObj<typeof OverlayStack> = {
  render: () => {
    const [menuOpen, setMenuOpen] = useState(false);
    const [overlayMode, setOverlayMode] = useState(false);

    return (
      <div style={{ position: "relative", width: "100%", height: "600px", background: "#1a1a1a" }}>
        {/* Simulated WASM viewport */}
        <div
          style={{
            position: "absolute",
            inset: 0,
            background: "linear-gradient(45deg, #222, #333)",
            zIndex: 0,
            opacity: overlayMode ? 0.3 : 1,
          }}
        >
          <p style={{ color: "#666", margin: "20px", fontSize: "12px" }}>WASM Viewport (opacity: {overlayMode ? "0.3" : "1"})</p>
        </div>

        {/* Full overlay stack */}
        <OverlayStack
          hud={
            <div
              style={{
                padding: "10px",
                background: "rgba(0, 100, 200, 0.8)",
                color: "#fff",
                fontSize: "11px",
              }}
            >
              HUD Layer (z-index 10)
            </div>
          }
          menu={
            menuOpen && (
              <div
                style={{
                  position: "fixed",
                  left: "10%",
                  top: "50%",
                  transform: "translateY(-50%)",
                  width: "200px",
                  background: "rgba(40, 40, 40, 0.95)",
                  border: "1px solid #666",
                  borderRadius: "4px",
                  padding: "15px",
                  color: "#fff",
                  zIndex: 20,
                }}
              >
                <div style={{ fontSize: "12px", fontWeight: "bold", marginBottom: "10px" }}>Menu (z-index 20)</div>
                <button
                  onClick={() => setMenuOpen(false)}
                  style={{
                    width: "100%",
                    padding: "6px",
                    background: "#333",
                    color: "#fff",
                    border: "1px solid #666",
                    borderRadius: "2px",
                    cursor: "pointer",
                  }}
                >
                  Close
                </button>
              </div>
            )
          }
          debug={
            <div
              style={{
                position: "fixed",
                bottom: "10px",
                right: "10px",
                padding: "8px 12px",
                background: "rgba(0, 0, 0, 0.7)",
                color: "#0f0",
                fontSize: "10px",
                fontFamily: "monospace",
                borderRadius: "4px",
                zIndex: 40,
              }}
            >
              overlayMode: {String(overlayMode)}
            </div>
          }
          onOverlayModeChange={setOverlayMode}
        />

        {/* Control button */}
        <button
          onClick={() => setMenuOpen(!menuOpen)}
          style={{
            position: "fixed",
            bottom: "20px",
            left: "20px",
            padding: "8px 16px",
            background: "#0f0",
            color: "#000",
            border: "none",
            borderRadius: "4px",
            cursor: "pointer",
            fontWeight: "bold",
            zIndex: 50,
          }}
        >
          {menuOpen ? "Close Menu" : "Open Menu"}
        </button>
      </div>
    );
  },
  parameters: {
    docs: {
      description: {
        story: "OverlayStack with all four layers: HUD (10), Menu (20), Modal (30), Debug (40). Test layer stacking and focus-lock transitions.",
      },
    },
  },
};
