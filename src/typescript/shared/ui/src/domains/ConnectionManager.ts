export type ConnectionState = "idle" | "connecting" | "connected" | "reconnecting" | "disconnected";

export type HeartbeatLayerName = "transport" | "session" | "integrity";

export interface HeartbeatLayerConfig {
  readonly layer: HeartbeatLayerName;
  readonly intervalMs: number;
  readonly payloadFactory?: () => Record<string, unknown> | undefined;
}

export interface HeartbeatStats {
  readonly sent: number;
  readonly failed: number;
  readonly lastSentAt: number | null;
  readonly lastAckAt: number | null;
  readonly sequence: number;
}

export interface HeartbeatHealthReport {
  readonly healthy: boolean;
  readonly staleLayers: HeartbeatLayerName[];
}

export interface ConnectionOptions {
  readonly heartbeatIntervalMs: number;
  readonly reconnectBackoffMs: number;
  readonly maxReconnectAttempts: number;
  readonly heartbeatLayers?: HeartbeatLayerConfig[];
  readonly heartbeatTimeoutMs?: number;
}

export interface TransportClient {
  connect(): Promise<void>;
  disconnect(): void;
  send(message: string): void;
  isOpen(): boolean;
}

export class ConnectionManager {
  private transport: TransportClient;
  private options: ConnectionOptions;
  private state: ConnectionState = "idle";
  private reconnectAttempts = 0;
  private heartbeatTimers = new Map<HeartbeatLayerName, ReturnType<typeof setInterval>>();
  private heartbeatStats = new Map<
    HeartbeatLayerName,
    {
      sent: number;
      failed: number;
      lastSentAt: number | null;
      lastAckAt: number | null;
      sequence: number;
    }
  >();

  constructor(transport: TransportClient, options: ConnectionOptions) {
    this.transport = transport;
    this.options = options;
  }

  async start(): Promise<void> {
    this.state = "connecting";
    await this.transport.connect();
    this.state = "connected";
    this.reconnectAttempts = 0;
    this.startHeartbeats();
  }

  stop(): void {
    this.stopHeartbeats();
    this.transport.disconnect();
    this.state = "disconnected";
  }

  send(message: string): boolean {
    if (!this.transport.isOpen()) return false;

    this.transport.send(message);
    return true;
  }

  async handleDisconnect(): Promise<void> {
    this.stopHeartbeats();

    if (this.reconnectAttempts >= this.options.maxReconnectAttempts) {
      this.state = "disconnected";
      return;
    }

    this.state = "reconnecting";
    this.reconnectAttempts += 1;
    await this.transport.connect();
    this.state = "connected";
    this.startHeartbeats();
  }

  getState(): ConnectionState {
    return this.state;
  }

  getReconnectAttempts(): number {
    return this.reconnectAttempts;
  }

  triggerHeartbeat(layer: HeartbeatLayerName, payload?: Record<string, unknown>): boolean {
    return this.sendHeartbeat(layer, payload);
  }

  acknowledgeHeartbeat(layer: HeartbeatLayerName, acknowledgedAt = Date.now()): void {
    const stats = this.ensureLayerStats(layer);
    stats.lastAckAt = acknowledgedAt;
  }

  getHeartbeatStats(): Record<HeartbeatLayerName, HeartbeatStats> {
    const transport = this.ensureLayerStats("transport");
    const session = this.ensureLayerStats("session");
    const integrity = this.ensureLayerStats("integrity");

    return {
      transport: { ...transport },
      session: { ...session },
      integrity: { ...integrity },
    };
  }

  checkHeartbeatHealth(nowMs = Date.now()): HeartbeatHealthReport {
    const staleLayers: HeartbeatLayerName[] = [];
    const timeoutMs =
      this.options.heartbeatTimeoutMs ?? Math.max(1000, this.options.heartbeatIntervalMs * 4);

    const layers = ["transport", "session", "integrity"] as const;
    for (const layer of layers) {
      const stats = this.ensureLayerStats(layer);
      if (!stats.lastSentAt) continue;

      const isAwaitingAck = !stats.lastAckAt || stats.lastAckAt < stats.lastSentAt;
      if (isAwaitingAck && nowMs - stats.lastSentAt > timeoutMs) staleLayers.push(layer);
    }

    return {
      healthy: staleLayers.length === 0,
      staleLayers,
    };
  }

  private startHeartbeats(): void {
    this.stopHeartbeats();
    const layers = this.resolveHeartbeatLayers();
    for (const layer of layers) {
      const timer = setInterval(() => {
        this.sendHeartbeat(layer.layer, layer.payloadFactory?.());
      }, layer.intervalMs);
      this.heartbeatTimers.set(layer.layer, timer);
    }
  }

  private stopHeartbeats(): void {
    for (const timer of this.heartbeatTimers.values()) clearInterval(timer);
    this.heartbeatTimers.clear();
  }

  private resolveHeartbeatLayers(): HeartbeatLayerConfig[] {
    if (this.options.heartbeatLayers && this.options.heartbeatLayers.length > 0)
      return this.options.heartbeatLayers;

    const base = Math.max(250, this.options.heartbeatIntervalMs);
    return [
      {
        layer: "transport",
        intervalMs: base,
      },
      {
        layer: "session",
        intervalMs: base * 2,
      },
      {
        layer: "integrity",
        intervalMs: base * 3,
      },
    ];
  }

  private sendHeartbeat(layer: HeartbeatLayerName, payload?: Record<string, unknown>): boolean {
    const stats = this.ensureLayerStats(layer);

    if (!this.transport.isOpen()) {
      stats.failed += 1;
      return false;
    }

    stats.sequence += 1;
    stats.sent += 1;
    stats.lastSentAt = Date.now();

    this.transport.send(
      JSON.stringify({
        type: "heartbeat",
        layer,
        sequence: stats.sequence,
        ts: stats.lastSentAt,
        payload,
      })
    );
    return true;
  }

  private ensureLayerStats(layer: HeartbeatLayerName): {
    sent: number;
    failed: number;
    lastSentAt: number | null;
    lastAckAt: number | null;
    sequence: number;
  } {
    const existing = this.heartbeatStats.get(layer);
    if (existing) return existing;

    const created = {
      sent: 0,
      failed: 0,
      lastSentAt: null,
      lastAckAt: null,
      sequence: 0,
    };
    this.heartbeatStats.set(layer, created);
    return created;
  }
}
