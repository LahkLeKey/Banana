/**
 * Domain-Driven Design: Interaction Domain
 *
 * Manages context menu state, action execution, and outcome propagation.
 * Follows Interface Segregation: context menu, action handlers, and results are separate concerns.
 * Follows Open/Closed Principle: extensible action handlers without modifying core logic.
 */

import type { ContextMenuAction, ContextMenuState } from "../components/GameEngineTypes";

/**
 * Value Object: Represents the result of an interaction action
 * Immutable and serializable
 */
export interface InteractionOutcome {
  readonly actionId: string;
  readonly targetNormalizedX: number;
  readonly targetNormalizedY: number;
  readonly success: boolean;
  readonly message: string;
  readonly timestamp: number;
}

/**
 * Function signature for action handlers
 * Pure function: same input → same output, no side effects
 */
export type ActionHandler = (
  actionId: string,
  normalizedX: number,
  normalizedY: number
) => InteractionOutcome;

/**
 * Entity: Manages context menu state and action execution
 * Follows Single Responsibility: only manages menu state and action routing
 */
export class InteractionManager {
  private menuState: ContextMenuState;
  private availableActions: ContextMenuAction[];
  private actionHandlers: Map<string, ActionHandler>;

  constructor(actions: ContextMenuAction[] = []) {
    this.menuState = {
      visible: false,
      x: 0,
      y: 0,
      normalizedX: 0,
      normalizedY: 0,
    };
    this.availableActions = actions;
    this.actionHandlers = new Map();
  }

  /**
   * Register an action handler
   * @param actionId - Unique action identifier
   * @param handler - Pure function to execute the action
   */
  registerActionHandler(actionId: string, handler: ActionHandler): void {
    this.actionHandlers.set(actionId, handler);
  }

  /**
   * Open context menu at specified position
   * @param clientX - Client-space X coordinate
   * @param clientY - Client-space Y coordinate
   * @param viewportWidth - Viewport width in pixels
   * @param viewportHeight - Viewport height in pixels
   * @param menuWidth - Menu width for positioning
   * @param menuHeight - Menu height for positioning
   */
  openMenu(
    clientX: number,
    clientY: number,
    viewportWidth: number,
    viewportHeight: number,
    menuWidth = 220,
    menuHeight = 140
  ): ContextMenuState {
    const padding = 8;
    const safeX = Math.min(clientX, viewportWidth - menuWidth - padding);
    const safeY = Math.min(clientY, viewportHeight - menuHeight - padding);
    const x = Math.max(padding, safeX);
    const y = Math.max(padding, safeY);

    const normalizedX = viewportWidth > 0 ? clientX / viewportWidth : 0;
    const normalizedY = viewportHeight > 0 ? clientY / viewportHeight : 0;

    this.menuState = {
      visible: true,
      x,
      y,
      normalizedX,
      normalizedY,
    };

    return this.menuState;
  }

  /**
   * Close context menu
   */
  closeMenu(): ContextMenuState {
    this.menuState = { ...this.menuState, visible: false };
    return this.menuState;
  }

  /**
   * Get current menu state
   */
  getMenuState(): ContextMenuState {
    return this.menuState;
  }

  /**
   * Get available actions
   */
  getAvailableActions(): ContextMenuAction[] {
    return this.availableActions;
  }

  /**
   * Execute an action through its registered handler
   * @param actionId - Action to execute
   * @returns InteractionOutcome with result and message
   */
  executeAction(actionId: string): InteractionOutcome {
    const handler = this.actionHandlers.get(actionId);

    if (!handler) {
      return {
        actionId,
        targetNormalizedX: this.menuState.normalizedX,
        targetNormalizedY: this.menuState.normalizedY,
        success: false,
        message: `Action handler not registered: ${actionId}`,
        timestamp: Date.now(),
      };
    }

    try {
      const outcome = handler(actionId, this.menuState.normalizedX, this.menuState.normalizedY);
      this.closeMenu();
      return outcome;
    } catch (error) {
      return {
        actionId,
        targetNormalizedX: this.menuState.normalizedX,
        targetNormalizedY: this.menuState.normalizedY,
        success: false,
        message: error instanceof Error ? error.message : "Action execution failed",
        timestamp: Date.now(),
      };
    }
  }
}

/**
 * Factory function for creating standard outcome
 */
export function createOutcome(
  actionId: string,
  normalizedX: number,
  normalizedY: number,
  success: boolean,
  message: string
): InteractionOutcome {
  return {
    actionId,
    targetNormalizedX: normalizedX,
    targetNormalizedY: normalizedY,
    success,
    message,
    timestamp: Date.now(),
  };
}
