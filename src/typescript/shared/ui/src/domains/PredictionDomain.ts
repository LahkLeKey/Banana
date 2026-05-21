import type { MovementInput } from "./InputDomain";

export interface PredictionWorldState {
  readonly tickIndex: number;
  readonly positionX: number;
  readonly positionZ: number;
  readonly timestamp: number;
}

export interface PredictionResult {
  readonly tickIndex: number;
  readonly inputApplied: MovementInput;
  readonly stateAfter: PredictionWorldState;
  readonly confidence: number;
  readonly divergenceReason?: string;
}

export interface PredictionReplayResult {
  readonly authoritative: PredictionResult;
  readonly replayed: PredictionResult[];
}

export interface LocalSimulator {
  step(input: MovementInput): PredictionWorldState;
  setState(state: PredictionWorldState): void;
  getState(): PredictionWorldState;
}

export class DeterministicLocalSimulator implements LocalSimulator {
  private state: PredictionWorldState = {
    tickIndex: 0,
    positionX: 0,
    positionZ: 0,
    timestamp: Date.now(),
  };

  step(input: MovementInput): PredictionWorldState {
    this.state = {
      tickIndex: this.state.tickIndex + 1,
      positionX: this.state.positionX + input.moveX,
      positionZ: this.state.positionZ + input.moveZ,
      timestamp: Date.now(),
    };
    return this.state;
  }

  setState(state: PredictionWorldState): void {
    this.state = state;
  }

  getState(): PredictionWorldState {
    return this.state;
  }
}

export class PredictionDomain {
  private simulator: LocalSimulator;
  private predictionCache = new Map<number, PredictionResult>();
  private maxCacheSize: number;

  constructor(simulator: LocalSimulator, maxCacheSize = 60) {
    this.simulator = simulator;
    this.maxCacheSize = Math.max(1, maxCacheSize);
  }

  predict(input: MovementInput): PredictionResult {
    const nextState = this.simulator.step(input);
    const result: PredictionResult = {
      tickIndex: nextState.tickIndex,
      inputApplied: input,
      stateAfter: nextState,
      confidence: 1,
    };

    this.predictionCache.set(result.tickIndex, result);
    this.trimCache();
    return result;
  }

  reconcile(authoritativeState: PredictionWorldState): PredictionResult {
    return this.reconcileAndReplay(authoritativeState).authoritative;
  }

  reconcileAndReplay(authoritativeState: PredictionWorldState): PredictionReplayResult {
    const predicted = this.predictionCache.get(authoritativeState.tickIndex);
    const divergence =
      !predicted ||
      predicted.stateAfter.positionX !== authoritativeState.positionX ||
      predicted.stateAfter.positionZ !== authoritativeState.positionZ;

    if (!divergence) {
      return {
        authoritative: {
          tickIndex: authoritativeState.tickIndex,
          inputApplied: predicted.inputApplied,
          stateAfter: authoritativeState,
          confidence: 1,
        },
        replayed: [],
      };
    }

    this.simulator.setState(authoritativeState);
    const authoritative: PredictionResult = {
      tickIndex: authoritativeState.tickIndex,
      inputApplied: predicted?.inputApplied ?? { moveX: 0, moveZ: 0, source: "none" },
      stateAfter: authoritativeState,
      confidence: 0,
      divergenceReason: "authoritative_mismatch",
    };
    this.predictionCache.set(authoritative.tickIndex, authoritative);

    const replayed: PredictionResult[] = [];
    const laterTicks = [...this.predictionCache.keys()]
      .filter((tickIndex) => tickIndex > authoritativeState.tickIndex)
      .sort((left, right) => left - right);

    for (const tickIndex of laterTicks) {
      const cached = this.predictionCache.get(tickIndex);
      if (!cached) continue;

      const replayedState = this.simulator.step(cached.inputApplied);
      const replayedResult: PredictionResult = {
        tickIndex: replayedState.tickIndex,
        inputApplied: cached.inputApplied,
        stateAfter: replayedState,
        confidence: 1,
      };
      this.predictionCache.set(replayedResult.tickIndex, replayedResult);
      replayed.push(replayedResult);
    }

    return {
      authoritative,
      replayed,
    };
  }

  getPrediction(tickIndex: number): PredictionResult | undefined {
    return this.predictionCache.get(tickIndex);
  }

  clear(): void {
    this.predictionCache.clear();
  }

  private trimCache(): void {
    if (this.predictionCache.size <= this.maxCacheSize) return;

    const ordered = [...this.predictionCache.keys()].sort((a, b) => a - b);
    while (ordered.length > this.maxCacheSize) {
      const oldest = ordered.shift();
      if (oldest !== undefined) this.predictionCache.delete(oldest);
    }
  }
}
