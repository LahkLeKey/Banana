/**
 * Spec 261: Performance instrumentation for measurable outcomes
 *
 * Tracks and exports performance metrics for:
 * - SC-001: First interactive frame <= 3.0s in 95% of runs
 * - SC-002: Overlay transitions preserve frame continuity (<= 1 dropped frame)
 * - SC-003: Input routing latency <= 100ms
 * - SC-004: 100% of WASM init failures enter degraded mode
 *
 * All metrics are exported to window.performance and telemetry endpoint.
 */

export interface PerformanceMetrics {
  // SC-001: First frame latency (milliseconds)
  boot_start_ms: number;
  boot_ready_ms: number;
  first_frame_ms: number;
  first_interactive_ms: number; // boot_start to first frame

  // SC-002: Frame continuity
  frame_count_total: number;
  frame_drops_overlay_transition: number;
  avg_frame_interval_ms: number;

  // SC-003: Input routing latency
  input_routing_latency_ms: number;
  viewport_mode_transitions: number;
  overlay_mode_transitions: number;

  // SC-004: Degraded mode entry
  degraded_entries: number;
  recovery_attempts: number;
  recovery_success_count: number;

  // General
  session_id: string;
  timestamp: number;
}

class PerformanceTracker {
  private metrics: PerformanceMetrics;
  private sessionStartMs: number;
  private lastFrameMs: number;
  private frameCount: number;

  constructor(sessionId: string) {
    this.sessionStartMs = Date.now();
    this.lastFrameMs = this.sessionStartMs;
    this.frameCount = 0;

    this.metrics = {
      boot_start_ms: 0,
      boot_ready_ms: 0,
      first_frame_ms: 0,
      first_interactive_ms: 0,
      frame_count_total: 0,
      frame_drops_overlay_transition: 0,
      avg_frame_interval_ms: 0,
      input_routing_latency_ms: 0,
      viewport_mode_transitions: 0,
      overlay_mode_transitions: 0,
      degraded_entries: 0,
      recovery_attempts: 0,
      recovery_success_count: 0,
      session_id: sessionId,
      timestamp: this.sessionStartMs,
    };

    // Mark boot start
    this.markBootStart();
  }

  markBootStart() {
    this.metrics.boot_start_ms = 0;
    window.performance?.mark?.("spec261:boot_start");
  }

  markBootReady() {
    const elapsed = Date.now() - this.sessionStartMs;
    this.metrics.boot_ready_ms = elapsed;
    window.performance?.mark?.("spec261:boot_ready");
    window.performance?.measure?.("spec261:boot_phase", "spec261:boot_start", "spec261:boot_ready");
  }

  markFirstFrame() {
    const elapsed = Date.now() - this.sessionStartMs;
    this.metrics.first_frame_ms = elapsed;
    this.metrics.first_interactive_ms = elapsed;
    window.performance?.mark?.("spec261:first_frame");
    window.performance?.measure?.(
      "spec261:boot_to_first_frame",
      "spec261:boot_start",
      "spec261:first_frame"
    );

    // Log SC-001 compliance
    if (elapsed <= 3000) {
      console.log(`✅ SC-001: First frame rendered in ${elapsed}ms (target: <=3000ms)`);
    } else {
      console.warn(`⚠️ SC-001: First frame took ${elapsed}ms (target: <=3000ms)`);
    }
  }

  recordFrameTick() {
    this.frameCount++;
    const now = Date.now();
    const frameInterval = now - this.lastFrameMs;

    // Update rolling average
    this.metrics.frame_count_total = this.frameCount;
    if (this.frameCount > 1) {
      const prevAvg = this.metrics.avg_frame_interval_ms || 0;
      this.metrics.avg_frame_interval_ms =
        (prevAvg * (this.frameCount - 1) + frameInterval) / this.frameCount;
    } else {
      this.metrics.avg_frame_interval_ms = frameInterval;
    }

    this.lastFrameMs = now;
  }

  recordFrameDrop(reason: string) {
    if (reason === "overlay_transition") {
      this.metrics.frame_drops_overlay_transition++;
    }
  }

  recordInputRoutingLatency(deltaMs: number) {
    this.metrics.input_routing_latency_ms = deltaMs;

    if (deltaMs > 100) {
      console.warn(`⚠️ SC-003: Input routing took ${deltaMs}ms (target: <=100ms)`);
    } else {
      console.log(`✅ SC-003: Input routing latency ${deltaMs}ms (target: <=100ms)`);
    }
  }

  recordViewportModeTransition() {
    this.metrics.viewport_mode_transitions++;
    window.performance?.mark?.("spec261:viewport_mode_transition");
  }

  recordOverlayModeTransition() {
    this.metrics.overlay_mode_transitions++;
    window.performance?.mark?.("spec261:overlay_mode_transition");
  }

  recordDegradedEntry() {
    this.metrics.degraded_entries++;
    window.performance?.mark?.("spec261:degraded_entry");
  }

  recordRecoveryAttempt() {
    this.metrics.recovery_attempts++;
    window.performance?.mark?.("spec261:recovery_attempt");
  }

  recordRecoverySuccess() {
    this.metrics.recovery_success_count++;
    window.performance?.mark?.("spec261:recovery_success");

    // SC-004 compliance: 100% of degraded entries should result in recovery or API fallback
    const recoveryRate = this.metrics.recovery_success_count / this.metrics.degraded_entries;
    if (recoveryRate >= 1) {
      console.log(
        `✅ SC-004: 100% degraded entries recovered (${this.metrics.recovery_success_count}/${this.metrics.degraded_entries})`
      );
    }
  }

  /**
   * Export metrics for telemetry or external reporting
   */
  export(): PerformanceMetrics {
    return { ...this.metrics };
  }

  /**
   * Create performance telemetry event payload
   */
  toTelemetryPayload() {
    return {
      type: "performance_snapshot",
      session_id: this.metrics.session_id,
      timestamp: Date.now(),
      metrics: this.metrics,
      compliance: {
        sc_001_first_frame_3s: this.metrics.first_interactive_ms <= 3000,
        sc_002_overlay_frame_continuity: this.metrics.frame_drops_overlay_transition <= 1,
        sc_003_input_routing_100ms: this.metrics.input_routing_latency_ms <= 100,
        sc_004_degraded_mode_100_pct:
          this.metrics.recovery_success_count === this.metrics.degraded_entries,
      },
    };
  }
}

// Global tracker instance (one per React mount)
let globalTracker: PerformanceTracker | null = null;

export function initPerformanceTracker(sessionId: string): PerformanceTracker {
  globalTracker = new PerformanceTracker(sessionId);
  return globalTracker;
}

export function getPerformanceTracker(): PerformanceTracker | null {
  return globalTracker;
}

// Convenience functions for direct calls
export function recordBootReady() {
  getPerformanceTracker()?.markBootReady();
}

export function recordFirstFrame() {
  getPerformanceTracker()?.markFirstFrame();
}

export function recordFrameTick() {
  getPerformanceTracker()?.recordFrameTick();
}

export function recordDegradedEntry() {
  getPerformanceTracker()?.recordDegradedEntry();
}

export function recordRecoveryAttempt() {
  getPerformanceTracker()?.recordRecoveryAttempt();
}

export function recordRecoverySuccess() {
  getPerformanceTracker()?.recordRecoverySuccess();
}

export function recordInputRoutingLatency(deltaMs: number) {
  getPerformanceTracker()?.recordInputRoutingLatency(deltaMs);
}

export function exportPerformanceMetrics(): PerformanceMetrics | null {
  return globalTracker?.export() || null;
}

export function exportTelemetryPayload() {
  return globalTracker?.toTelemetryPayload() || null;
}
