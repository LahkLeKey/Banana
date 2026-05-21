import type { RadialControlState } from "./GameEngineTypes";
import { RADIAL_CONFIG } from "./GameEngineTypes";

export interface MobileRadialControlProps {
  state: RadialControlState;
}

/**
 * Mobile radial touch control for movement input
 * Displays a radial UI showing current touch position and active direction
 * Used by mobile and tablet platforms
 */
export function MobileRadialControl({ state }: MobileRadialControlProps) {
  const { RADIUS, INNER_RADIUS } = RADIAL_CONFIG;

  if (!state.visible) {
    return null;
  }

  return (
    <div
      data-testid="mobile-radial-control"
      style={{
        position: "absolute",
        left: state.x - RADIUS,
        top: state.y - RADIUS,
        width: RADIUS * 2,
        height: RADIUS * 2,
        pointerEvents: "none",
        zIndex: 800,
      }}
    >
      <svg
        aria-label="Mobile movement radial"
        role="img"
        width={RADIUS * 2}
        height={RADIUS * 2}
        viewBox={`0 0 ${RADIUS * 2} ${RADIUS * 2}`}
        style={{ position: "absolute" }}
      >
        <title>Mobile movement radial</title>
        {/* Outer circle background */}
        <circle
          cx={RADIUS}
          cy={RADIUS}
          r={RADIUS}
          fill="rgba(0, 0, 0, 0.4)"
          stroke="rgba(255, 255, 255, 0.2)"
          strokeWidth="1"
        />

        {/* Inner circle (dead zone) */}
        <circle
          cx={RADIUS}
          cy={RADIUS}
          r={INNER_RADIUS}
          fill="rgba(255, 255, 255, 0.05)"
          stroke="rgba(255, 255, 255, 0.15)"
          strokeWidth="1"
        />

        {/* Direction indicators */}
        {/* Up (W) */}
        <path
          d={`M ${RADIUS} ${INNER_RADIUS} L ${RADIUS + 12} ${INNER_RADIUS + 15} L ${RADIUS} ${RADIUS - 5} L ${RADIUS - 12} ${INNER_RADIUS + 15} Z`}
          fill={
            state.activeDirection === "up" ? "rgba(100, 200, 255, 0.8)" : "rgba(255, 255, 255, 0.2)"
          }
          stroke="rgba(255, 255, 255, 0.3)"
          strokeWidth="0.5"
        />

        {/* Down (S) */}
        <path
          d={`M ${RADIUS} ${RADIUS + INNER_RADIUS + 5} L ${RADIUS + 12} ${RADIUS - INNER_RADIUS - 15} L ${RADIUS} ${RADIUS + 5} L ${RADIUS - 12} ${RADIUS - INNER_RADIUS - 15} Z`}
          fill={
            state.activeDirection === "down"
              ? "rgba(100, 200, 255, 0.8)"
              : "rgba(255, 255, 255, 0.2)"
          }
          stroke="rgba(255, 255, 255, 0.3)"
          strokeWidth="0.5"
        />

        {/* Left (A) */}
        <path
          d={`M ${INNER_RADIUS} ${RADIUS} L ${INNER_RADIUS + 15} ${RADIUS - 12} L ${RADIUS - 5} ${RADIUS} L ${INNER_RADIUS + 15} ${RADIUS + 12} Z`}
          fill={
            state.activeDirection === "left"
              ? "rgba(100, 200, 255, 0.8)"
              : "rgba(255, 255, 255, 0.2)"
          }
          stroke="rgba(255, 255, 255, 0.3)"
          strokeWidth="0.5"
        />

        {/* Right (D) */}
        <path
          d={`M ${RADIUS + INNER_RADIUS + 5} ${RADIUS} L ${RADIUS - INNER_RADIUS - 15} ${RADIUS - 12} L ${RADIUS + 5} ${RADIUS} L ${RADIUS - INNER_RADIUS - 15} ${RADIUS + 12} Z`}
          fill={
            state.activeDirection === "right"
              ? "rgba(100, 200, 255, 0.8)"
              : "rgba(255, 255, 255, 0.2)"
          }
          stroke="rgba(255, 255, 255, 0.3)"
          strokeWidth="0.5"
        />

        {/* Center touch indicator */}
        <circle cx={RADIUS} cy={RADIUS} r="4" fill="rgba(255, 255, 255, 0.3)" />
      </svg>

      {/* Direction labels */}
      <DirectionLabel
        position="top"
        offset={INNER_RADIUS}
        label="W"
        active={state.activeDirection === "up"}
      />
      <DirectionLabel
        position="bottom"
        offset={INNER_RADIUS}
        label="S"
        active={state.activeDirection === "down"}
      />
      <DirectionLabel
        position="left"
        offset={INNER_RADIUS}
        label="A"
        active={state.activeDirection === "left"}
      />
      <DirectionLabel
        position="right"
        offset={INNER_RADIUS}
        label="D"
        active={state.activeDirection === "right"}
      />
    </div>
  );
}

interface DirectionLabelProps {
  position: "top" | "bottom" | "left" | "right";
  offset: number;
  label: string;
  active: boolean;
}

function DirectionLabel({ position, offset, label, active }: DirectionLabelProps) {
  const commonStyle: React.CSSProperties = {
    position: "absolute",
    fontSize: 11,
    fontFamily: "monospace",
    fontWeight: 700,
    color: active ? "rgba(100, 200, 255, 1)" : "rgba(255, 255, 255, 0.4)",
    textShadow: "0 1px 3px rgba(0, 0, 0, 0.5)",
    pointerEvents: "none",
  };

  const positionStyles: Record<string, React.CSSProperties> = {
    top: {
      top: offset - 8,
      left: "50%",
      transform: "translateX(-50%)",
    },
    bottom: {
      bottom: offset - 8,
      left: "50%",
      transform: "translateX(-50%)",
    },
    left: {
      left: offset - 8,
      top: "50%",
      transform: "translateY(-50%)",
    },
    right: {
      right: offset - 8,
      top: "50%",
      transform: "translateY(-50%)",
    },
  };

  const testIds: Record<string, string> = {
    top: "mobile-radial-up",
    bottom: "mobile-radial-down",
    left: "mobile-radial-left",
    right: "mobile-radial-right",
  };

  return (
    <div data-testid={testIds[position]} style={{ ...commonStyle, ...positionStyles[position] }}>
      {label}
    </div>
  );
}
