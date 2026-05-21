import type { InputAggregator } from "./InputDomain";
import type { InteractionManager } from "./InteractionDomain";

export type AntiCheatViolationReason =
  | "sequence_regression"
  | "sequence_gap"
  | "tick_ahead"
  | "tick_behind"
  | "clock_skew"
  | "invalid_axis";

export interface NetcodeAntiCheatConfig {
  readonly maxTickLead: number;
  readonly maxTickLag: number;
  readonly maxClientClockSkewMs: number;
  readonly maxSequenceGap: number;
  readonly maxViolationsBeforeFlag: number;
}

export interface AntiCheatStatus {
  readonly violationScore: number;
  readonly flagged: boolean;
  readonly lastViolationReason: AntiCheatViolationReason | null;
  readonly lastVerifiedTick: number;
  readonly rollingIntegrityHash: number;
}

export interface SimulationHeartbeat {
  readonly layer: "simulation";
  readonly heartbeatSequence: number;
  readonly serverTick: number;
  readonly lastInputSequence: number;
  readonly bufferedFrames: number;
  readonly rollingIntegrityHash: number;
  readonly violationScore: number;
  readonly flagged: boolean;
  readonly timestamp: number;
}

export interface InputCommand {
  readonly inputSequence: number;
  readonly tickIndex: number;
  readonly moveX: -1 | 0 | 1;
  readonly moveZ: -1 | 0 | 1;
  readonly timestamp: number;
}

export interface TickSnapshot {
  readonly tickIndex: number;
  readonly moveX: -1 | 0 | 1;
  readonly moveZ: -1 | 0 | 1;
  readonly timestamp: number;
  readonly isAuthoritative: boolean;
}

export interface NetcodeFrame {
  tick: number;
  input: InputCommand;
  predictedState: TickSnapshot;
  authoritativeState?: TickSnapshot;
}

export interface NetcodeDomainConfig {
  tickRate: number;
  maxPrediction: number;
}

export class NetcodeDomain {
  private static readonly DEFAULT_ANTI_CHEAT_CONFIG: NetcodeAntiCheatConfig = {
    maxTickLead: 3,
    maxTickLag: 6,
    maxClientClockSkewMs: 1500,
    maxSequenceGap: 5,
    maxViolationsBeforeFlag: 3,
  };

  private readonly config: NetcodeDomainConfig;
  private readonly inputAggregator: InputAggregator;
  private readonly interactionManager: InteractionManager;
  private readonly antiCheatConfig: NetcodeAntiCheatConfig;
  private frameBuffer: NetcodeFrame[] = [];
  private currentTick: number = 0;
  private inputSequence: number = 0;
  private acceptedInputSequence: number = 0;
  private acceptedInputTick: number = 0;
  private antiCheatViolationScore: number = 0;
  private antiCheatLastViolationReason: AntiCheatViolationReason | null = null;
  private rollingIntegrityHash: number = 0;
  private simulationHeartbeatSequence: number = 0;

  constructor(
    config: NetcodeDomainConfig,
    inputAggregator: InputAggregator,
    interactionManager: InteractionManager,
    antiCheatConfig: Partial<NetcodeAntiCheatConfig> = {}
  ) {
    this.config = config;
    this.inputAggregator = inputAggregator;
    this.interactionManager = interactionManager;
    this.antiCheatConfig = {
      ...NetcodeDomain.DEFAULT_ANTI_CHEAT_CONFIG,
      ...antiCheatConfig,
    };
  }

  public advanceTick(): void {
    const movement = this.inputAggregator.computeMovement();
    this.currentTick += 1;
    this.inputSequence += 1;

    const input: InputCommand = {
      inputSequence: this.inputSequence,
      tickIndex: this.currentTick,
      moveX: movement.moveX,
      moveZ: movement.moveZ,
      timestamp: Date.now(),
    };

    const predictedState = this.predictState(input);
    this.frameBuffer.push({
      tick: this.currentTick,
      input,
      predictedState,
    });
    this.acceptedInputSequence = input.inputSequence;
    this.acceptedInputTick = input.tickIndex;
    this.updateIntegrityHash(input);
    if (this.frameBuffer.length > this.config.maxPrediction) {
      this.frameBuffer.shift();
    }
  }

  public ingestClientCommand(
    command: InputCommand,
    serverTick: number,
    receivedAtMs = Date.now()
  ): boolean {
    const violation = this.validateClientCommand(command, serverTick, receivedAtMs);
    if (violation) {
      this.registerViolation(violation);
      return false;
    }

    this.acceptedInputSequence = command.inputSequence;
    this.acceptedInputTick = command.tickIndex;
    this.currentTick = Math.max(this.currentTick, command.tickIndex);
    const predictedState = this.predictState(command);
    this.frameBuffer.push({
      tick: command.tickIndex,
      input: command,
      predictedState,
    });
    this.updateIntegrityHash(command);

    if (this.frameBuffer.length > this.config.maxPrediction) {
      this.frameBuffer.shift();
    }

    return true;
  }

  public reconcileAuthoritativeState(tick: number, authoritativeState: TickSnapshot): void {
    const frame = this.frameBuffer.find((f) => f.tick === tick);
    if (frame) {
      frame.authoritativeState = authoritativeState;
    }
  }

  private predictState(input: InputCommand): TickSnapshot {
    const interactionBias = this.interactionManager.getMenuState().visible ? 0 : 0;
    return {
      tickIndex: input.tickIndex,
      moveX: (input.moveX + interactionBias) as -1 | 0 | 1,
      moveZ: input.moveZ,
      timestamp: input.timestamp,
      isAuthoritative: false,
    };
  }

  public getCurrentTick(): number {
    return this.currentTick;
  }

  public getFrameBuffer(): NetcodeFrame[] {
    return this.frameBuffer;
  }

  public getAntiCheatStatus(): AntiCheatStatus {
    return {
      violationScore: this.antiCheatViolationScore,
      flagged: this.antiCheatViolationScore >= this.antiCheatConfig.maxViolationsBeforeFlag,
      lastViolationReason: this.antiCheatLastViolationReason,
      lastVerifiedTick: this.acceptedInputTick,
      rollingIntegrityHash: this.rollingIntegrityHash,
    };
  }

  public createSimulationHeartbeat(serverTick: number): SimulationHeartbeat {
    this.simulationHeartbeatSequence += 1;
    return {
      layer: "simulation",
      heartbeatSequence: this.simulationHeartbeatSequence,
      serverTick,
      lastInputSequence: this.acceptedInputSequence,
      bufferedFrames: this.frameBuffer.length,
      rollingIntegrityHash: this.rollingIntegrityHash,
      violationScore: this.antiCheatViolationScore,
      flagged: this.antiCheatViolationScore >= this.antiCheatConfig.maxViolationsBeforeFlag,
      timestamp: Date.now(),
    };
  }

  private validateClientCommand(
    command: InputCommand,
    serverTick: number,
    receivedAtMs: number
  ): AntiCheatViolationReason | null {
    if (!this.isAxisValue(command.moveX) || !this.isAxisValue(command.moveZ)) return "invalid_axis";

    if (command.inputSequence <= this.acceptedInputSequence) return "sequence_regression";

    const sequenceGap = command.inputSequence - this.acceptedInputSequence;
    if (this.acceptedInputSequence > 0 && sequenceGap > this.antiCheatConfig.maxSequenceGap)
      return "sequence_gap";

    if (command.tickIndex - serverTick > this.antiCheatConfig.maxTickLead) return "tick_ahead";

    if (serverTick - command.tickIndex > this.antiCheatConfig.maxTickLag) return "tick_behind";

    const clockSkew = Math.abs(receivedAtMs - command.timestamp);
    if (clockSkew > this.antiCheatConfig.maxClientClockSkewMs) return "clock_skew";

    return null;
  }

  private registerViolation(reason: AntiCheatViolationReason): void {
    this.antiCheatLastViolationReason = reason;
    this.antiCheatViolationScore += reason === "sequence_gap" ? 2 : 1;
  }

  private isAxisValue(value: number): value is -1 | 0 | 1 {
    return value === -1 || value === 0 || value === 1;
  }

  private updateIntegrityHash(command: InputCommand): void {
    const axisMix = (command.moveX + 2) * 7 + (command.moveZ + 2) * 13;
    const timestampMix = command.timestamp & 0xffff;
    const inputMix = command.inputSequence * 31 + command.tickIndex * 17 + axisMix + timestampMix;
    this.rollingIntegrityHash = ((this.rollingIntegrityHash * 33) ^ inputMix) >>> 0;
  }
}
