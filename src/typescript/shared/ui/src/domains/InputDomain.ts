/**
 * Domain-Driven Design: Input Domain
 *
 * Aggregates keyboard and touch input into a coherent movement intent.
 * Follows Single Responsibility: only manages input state and aggregation.
 * Follows Dependency Inversion: exports pure functions, no framework dependencies.
 */

import type { RadialDirection } from "./GameEngineTypes";

/**
 * Value Object: Represents a discrete movement input at a point in time
 * Immutable and comparable
 */
export interface MovementInput {
  readonly moveX: -1 | 0 | 1;
  readonly moveZ: -1 | 0 | 1;
  readonly source: "keyboard" | "touch" | "pointer" | "none";
}

/** Empty movement (no input) */
export const NEUTRAL_MOVEMENT: MovementInput = {
  moveX: 0,
  moveZ: 0,
  source: "none",
};

/**
 * Entity: Manages aggregation of keyboard and touch inputs into a single movement intent
 * Resolves conflicts by priority: keyboard > touch > pointer
 */
export class InputAggregator {
  private activeKeys = new Set<string>();
  private activeRadialDirection: RadialDirection = null;

  /**
   * Update keyboard key state
   * @param key - Key name (lowercase)
   * @param pressed - true if key was pressed, false if released
   */
  updateKeyState(key: string, pressed: boolean): void {
    if (pressed) {
      this.activeKeys.add(key.toLowerCase());
    } else {
      this.activeKeys.delete(key.toLowerCase());
    }
  }

  /**
   * Update radial touch input state
   * @param direction - Cardinal direction from radial control, or null if inactive
   */
  updateRadialDirection(direction: RadialDirection): void {
    this.activeRadialDirection = direction;
  }

  /**
   * Compute aggregate movement input with priority resolution
   * @returns MovementInput representing the highest-priority active input
   */
  computeMovement(): MovementInput {
    // Keyboard has highest priority
    if (this.activeKeys.size > 0) {
      const moveX = ((this.activeKeys.has("d") || this.activeKeys.has("arrowright") ? 1 : 0) +
        (this.activeKeys.has("a") || this.activeKeys.has("arrowleft") ? -1 : 0)) as -1 | 0 | 1;
      const moveZ = ((this.activeKeys.has("w") || this.activeKeys.has("arrowup") ? 1 : 0) +
        (this.activeKeys.has("s") || this.activeKeys.has("arrowdown") ? -1 : 0)) as -1 | 0 | 1;
      return { moveX, moveZ, source: "keyboard" };
    }

    // Radial touch has next priority
    if (this.activeRadialDirection) {
      const moveX =
        this.activeRadialDirection === "right"
          ? 1
          : this.activeRadialDirection === "left"
            ? -1
            : (0 as -1 | 0 | 1);
      const moveZ =
        this.activeRadialDirection === "down"
          ? -1
          : this.activeRadialDirection === "up"
            ? 1
            : (0 as -1 | 0 | 1);
      return { moveX, moveZ, source: "touch" };
    }

    return NEUTRAL_MOVEMENT;
  }

  /**
   * Clear all active inputs (e.g., on blur or visibility change)
   */
  clearAllInputs(): void {
    this.activeKeys.clear();
    this.activeRadialDirection = null;
  }
}
