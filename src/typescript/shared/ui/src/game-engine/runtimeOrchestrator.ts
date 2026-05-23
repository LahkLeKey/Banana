import type {MutableRefObject} from 'react';

import {getDirectionFromTouch, isMovementKey} from '../components/GameEngineTypes';
import type {GameEngineUIService} from '../domains/GameEngineUIService';
import type {NetcodeDomain} from '../domains/NetcodeDomain';
import type {PredictionDomain} from '../domains/PredictionDomain';
import type {ReplicationDomain} from '../domains/ReplicationDomain';

import type {GameSessionBootstrap} from './api';
import type {BananaEngineModule} from './types';

export type RuntimeOrchestratorContext = {
  mod: BananaEngineModule; canvas: HTMLCanvasElement; viewport: HTMLDivElement;
  uiService: GameEngineUIService;
  sessionBootstrap: GameSessionBootstrap | null;
  websocketRef: MutableRefObject<WebSocket|null>;
  outboundSequenceRef: MutableRefObject<number>;
  outboundTickRef: MutableRefObject<number>;
  netcodeRef: MutableRefObject<NetcodeDomain|null>;
  predictionRef: MutableRefObject<PredictionDomain|null>;
  replicationRef: MutableRefObject<ReplicationDomain|null>;
  renderHealthTimerRef: MutableRefObject<number>;
};

export function attachRuntimeOrchestrator(context: RuntimeOrchestratorContext):
    () => void {
  const {
    mod,
    canvas,
    viewport,
    uiService,
    sessionBootstrap,
    websocketRef,
    outboundSequenceRef,
    outboundTickRef,
    netcodeRef,
    predictionRef,
    replicationRef,
    renderHealthTimerRef,
  } = context;

  const inputMgr = uiService.getInputAggregator();
  uiService.getViewportSizer();

  let activePointerId: number|null = null;
  let activeTouchId: number|null = null;
  let touchStartTimer = 0;
  let touchAnchor: {x: number; y: number}|null = null;
  let radialActive = false;
  let loopInterval = 0;
  let tickCounter = 0;
  let consecutiveBlankFrameProbes = 0;

  const clearTransientInputState = () => {
    window.clearTimeout(touchStartTimer);
    touchStartTimer = 0;
    activeTouchId = null;
    activePointerId = null;
    touchAnchor = null;
    radialActive = false;
  };

  const clearMovementIntent = () => {
    uiService.clearInput();
    clearTransientInputState();
    mod.ccall('engine_set_move_input', 'null', ['number', 'number'], [0, 0]);
  };

  const keyDown = (event: KeyboardEvent) => {
    const key = event.key.toLowerCase();
    if (isMovementKey(key)) {
      inputMgr.updateKeyState(key, true);
      event.preventDefault();
    }
  };

  const keyUp = (event: KeyboardEvent) => {
    const key = event.key.toLowerCase();
    if (isMovementKey(key)) {
      inputMgr.updateKeyState(key, false);
    }
  };

  const onBlur = () => {
    clearMovementIntent();
  };

  const onVisibilityChange = () => {
    if (document.hidden) {
      clearMovementIntent();
    }
  };

  const onTouchStart = (event: TouchEvent) => {
    if (event.touches.length === 0) return;

    const touch = event.touches[0];
    activeTouchId = touch.identifier;

    const bounds = viewport.getBoundingClientRect();
    const touchX = bounds ? touch.clientX - bounds.left : touch.clientX;
    const touchY = bounds ? touch.clientY - bounds.top : touch.clientY;
    touchAnchor = {x: touchX, y: touchY};

    touchStartTimer = window.setTimeout(() => {
      radialActive = true;
    }, 150);
  };

  const onTouchMove = (event: TouchEvent) => {
    if (activeTouchId === null) return;

    const touch =
        Array.from(event.touches).find((t) => t.identifier === activeTouchId);
    if (!touch) return;

    const bounds = viewport.getBoundingClientRect();
    const touchX = bounds ? touch.clientX - bounds.left : touch.clientX;
    const touchY = bounds ? touch.clientY - bounds.top : touch.clientY;
    if (!radialActive || !touchAnchor) return;

    const direction =
        getDirectionFromTouch(touchX, touchY, touchAnchor.x, touchAnchor.y);
    inputMgr.updateRadialDirection(direction);
  };

  const onTouchEnd = (event: TouchEvent) => {
    const touch = Array.from(event.changedTouches)
                      .find((t) => t.identifier === activeTouchId);
    if (!touch) return;

    window.clearTimeout(touchStartTimer);
    activeTouchId = null;
    touchAnchor = null;
    radialActive = false;
    inputMgr.updateRadialDirection(null);
  };

  const onPointerDown = (event: PointerEvent) => {
    if (event.pointerType === 'mouse') return;

    activePointerId = event.pointerId;

    const bounds = viewport.getBoundingClientRect();
    const touchX = bounds ? event.clientX - bounds.left : event.clientX;
    const touchY = bounds ? event.clientY - bounds.top : event.clientY;
    touchAnchor = {x: touchX, y: touchY};

    window.clearTimeout(touchStartTimer);
    touchStartTimer = window.setTimeout(() => {
      radialActive = true;
    }, 150);
  };

  const onPointerMove = (event: PointerEvent) => {
    if (activePointerId !== event.pointerId) return;

    const bounds = viewport.getBoundingClientRect();
    const touchX = bounds ? event.clientX - bounds.left : event.clientX;
    const touchY = bounds ? event.clientY - bounds.top : event.clientY;
    if (!radialActive || !touchAnchor) return;

    const direction =
        getDirectionFromTouch(touchX, touchY, touchAnchor.x, touchAnchor.y);
    inputMgr.updateRadialDirection(direction);
  };

  const onPointerUp = (event: PointerEvent) => {
    if (activePointerId !== event.pointerId) return;

    window.clearTimeout(touchStartTimer);
    activePointerId = null;
    touchAnchor = null;
    radialActive = false;
    inputMgr.updateRadialDirection(null);
  };

  const probeFramebuffer = () => {
    try {
      const gl = canvas.getContext('webgl2');
      if (!gl) {
        return;
      }
      const width = Math.max(1, canvas.width);
      const height = Math.max(1, canvas.height);
      const sampleAt = (x: number, y: number) => {
        const pixel = new Uint8Array(4);
        gl.readPixels(x, y, 1, 1, gl.RGBA, gl.UNSIGNED_BYTE, pixel);
        return pixel;
      };
      const samples = [
        sampleAt(Math.floor(width / 2), Math.floor(height / 2)),
        sampleAt(Math.floor(width / 4), Math.floor(height / 4)),
        sampleAt(Math.max(0, width - 2), Math.max(0, height - 2)),
      ];
      const hasAnyRenderedPixel = samples.some(
          (pixel) => pixel[0] !== 0 || pixel[1] !== 0 || pixel[2] !== 0 ||
              pixel[3] !== 0);
      if (hasAnyRenderedPixel) {
        consecutiveBlankFrameProbes = 0;
        return;
      }
      consecutiveBlankFrameProbes += 1;
      if (consecutiveBlankFrameProbes >= 2) {
        consecutiveBlankFrameProbes = 0;
      }
    } catch {
      // Non-critical probe failures should not affect gameplay runtime.
    }
  };

  const fpsLoop = () => {
    try {
      const movement = inputMgr.computeMovement();
      netcodeRef.current?.advanceTick();
      const tick = netcodeRef.current?.getCurrentTick() ?? 0;
      replicationRef.current?.recordState(tick, {
        tick,
        timestamp: Date.now(),
        entities: {
          localPlayer: {
            moveX: movement.moveX,
            moveZ: movement.moveZ,
            source: movement.source,
          },
        },
        isAuthoritative: false,
      });
      predictionRef.current?.predict(movement);
      if (sessionBootstrap &&
          websocketRef.current?.readyState === WebSocket.OPEN) {
        outboundSequenceRef.current += 1;
        outboundTickRef.current += 1;
        websocketRef.current.send(JSON.stringify({
          type: 'input',
          sequence: outboundSequenceRef.current,
          tick: outboundTickRef.current,
          moveX: movement.moveX,
          moveZ: movement.moveZ,
          timestamp: Date.now(),
        }));
      }
      mod.ccall(
          'engine_set_move_input', 'null', ['number', 'number'],
          [movement.moveX, movement.moveZ]);
      mod.ccall('engine_tick', 'number', ['number'], [1 / 60]);
      tickCounter += 1;
      if (tickCounter % 60 === 0) {
        probeFramebuffer();
      }
    } catch {
      // Keep the loop alive when individual ticks fail.
    }
  };

  window.addEventListener('keydown', keyDown, {capture: true});
  window.addEventListener('keyup', keyUp, {capture: true});
  window.addEventListener('blur', onBlur);
  document.addEventListener('visibilitychange', onVisibilityChange);
  window.addEventListener('touchstart', onTouchStart, {passive: true});
  window.addEventListener('touchmove', onTouchMove, {passive: true});
  window.addEventListener('touchend', onTouchEnd, {passive: true});
  window.addEventListener('pointerdown', onPointerDown, {passive: true});
  window.addEventListener('pointermove', onPointerMove, {passive: true});
  window.addEventListener('pointerup', onPointerUp, {passive: true});
  window.addEventListener('pointercancel', onPointerUp, {passive: true});

  loopInterval = window.setInterval(fpsLoop, 16);
  renderHealthTimerRef.current = window.setTimeout(() => {
    probeFramebuffer();
  }, 1200);

  return () => {
    window.clearInterval(loopInterval);
    window.clearTimeout(touchStartTimer);
    window.clearTimeout(renderHealthTimerRef.current);

    window.removeEventListener('keydown', keyDown, {capture: true});
    window.removeEventListener('keyup', keyUp, {capture: true});
    window.removeEventListener('blur', onBlur);
    document.removeEventListener('visibilitychange', onVisibilityChange);
    window.removeEventListener('touchstart', onTouchStart);
    window.removeEventListener('touchmove', onTouchMove);
    window.removeEventListener('touchend', onTouchEnd);
    window.removeEventListener('pointerdown', onPointerDown);
    window.removeEventListener('pointermove', onPointerMove);
    window.removeEventListener('pointerup', onPointerUp);
    window.removeEventListener('pointercancel', onPointerUp);
  };
}