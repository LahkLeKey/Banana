/**
 * Domain-Driven Design: Game Engine UI Service
 *
 * Application-level service orchestrating Input, Interaction, and Viewport domains.
 * Implements state machine for engine lifecycle: idle → loading → running → error/unavailable.
 * Follows Dependency Inversion: domains are injected, not instantiated internally.
 * Follows Interface Segregation: exposes minimal, well-defined contract.
 */

import type { ContextMenuAction, EngineStatus } from "./GameEngineTypes";
import { InputAggregator, type MovementInput } from "./InputDomain";
import { InteractionManager, type InteractionOutcome } from "./InteractionDomain";
import { type ViewportSize, ViewportSizer } from "./ViewportDomain";

/**
 * Value Object: Represents the complete UI state at a point in time
 */
export interface GameEngineUIState {
  readonly engineStatus: EngineStatus;
  readonly error: string | null;
  readonly movementInput: MovementInput;
  readonly viewportSize: ViewportSize;
  readonly menuOpen: boolean;
  readonly interactionMessage: string | null;
  readonly timestamp: number;
}

/**
 * State transition event
 */
export type StateChangeListener = (state: GameEngineUIState) => void;

/**
 * Application Service: Orchestrates game engine UI domains
 * Single point of coordination for all UI state
 */
export class GameEngineUIService {
  private status: EngineStatus = "idle";
  private error: string | null = null;
  private inputAggregator: InputAggregator;
  private interactionManager: InteractionManager;
  private viewportSizer: ViewportSizer;
  private stateListeners: StateChangeListener[] = [];
  private lastBroadcastState: GameEngineUIState | null = null;

  constructor(viewportElement: HTMLElement, contextMenuActions: ContextMenuAction[] = []) {
    this.viewportSizer = new ViewportSizer();
    this.viewportSizer.attachTo(viewportElement);
    this.inputAggregator = new InputAggregator();
    this.interactionManager = new InteractionManager(contextMenuActions);
  }

  /**
   * Register a listener for state changes
   */
  onStateChange(listener: StateChangeListener): () => void {
    this.stateListeners.push(listener);
    // Return unsubscribe function
    return () => {
      this.stateListeners = this.stateListeners.filter((l) => l !== listener);
    };
  }

  /**
   * Broadcast current state to all listeners if changed
   */
  private broadcastStateChange(): void {
    const newState = this.getState();

    // Only emit if state actually changed
    if (
      this.lastBroadcastState === null ||
      !this.statesAreEqual(newState, this.lastBroadcastState)
    ) {
      this.lastBroadcastState = newState;
      this.stateListeners.forEach((listener) => listener(newState));
    }
  }

  /**
   * Get current complete UI state
   */
  getState(): GameEngineUIState {
    return {
      engineStatus: this.status,
      error: this.error,
      movementInput: this.inputAggregator.computeMovement(),
      viewportSize: this.viewportSizer.computeSize(),
      menuOpen: this.interactionManager.getMenuState().visible,
      interactionMessage: null, // Managed by component
      timestamp: Date.now(),
    };
  }

  /**
   * Transition engine status with optional error
   */
  setEngineStatus(status: EngineStatus, error?: string): void {
    this.status = status;
    this.error = error || null;
    this.broadcastStateChange();
  }

  /**
   * Get access to input domain for handling input events
   */
  getInputAggregator(): InputAggregator {
    return this.inputAggregator;
  }

  /**
   * Get access to interaction domain for menu operations
   */
  getInteractionManager(): InteractionManager {
    return this.interactionManager;
  }

  /**
   * Get access to viewport domain for sizing operations
   */
  getViewportSizer(): ViewportSizer {
    return this.viewportSizer;
  }

  /**
   * Execute a context menu action
   * @param actionId - Action identifier
   * @returns InteractionOutcome
   */
  executeMenuAction(actionId: string): InteractionOutcome {
    const outcome = this.interactionManager.executeAction(actionId);
    this.broadcastStateChange();
    return outcome;
  }

  /**
   * Open context menu at screen position
   */
  openContextMenu(clientX: number, clientY: number): void {
    const viewport = this.viewportSizer.computeSize();
    this.interactionManager.openMenu(clientX, clientY, viewport.cssWidth, viewport.cssHeight);
    this.broadcastStateChange();
  }

  /**
   * Close context menu
   */
  closeContextMenu(): void {
    this.interactionManager.closeMenu();
    this.broadcastStateChange();
  }

  /**
   * Clear all active input (e.g., on blur)
   */
  clearInput(): void {
    this.inputAggregator.clearAllInputs();
    this.broadcastStateChange();
  }

  /**
   * Check if viewport has changed significantly
   */
  checkViewportChange(): boolean {
    return this.viewportSizer.hasSizeChanged();
  }

  /**
   * Compare two states for equality (shallow comparison of critical fields)
   */
  private statesAreEqual(s1: GameEngineUIState, s2: GameEngineUIState): boolean {
    return (
      s1.engineStatus === s2.engineStatus &&
      s1.error === s2.error &&
      s1.movementInput.source === s2.movementInput.source &&
      s1.movementInput.moveX === s2.movementInput.moveX &&
      s1.movementInput.moveZ === s2.movementInput.moveZ &&
      s1.menuOpen === s2.menuOpen
    );
  }
}
