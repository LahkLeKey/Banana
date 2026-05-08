/**
 * Spec 261: Advanced Overlay Behaviors
 *
 * Extends spec 261 overlay stack with production-ready features:
 * - Modal stacking (nested modals with backdrop management)
 * - Focus trap (keyboard Tab stays within topmost modal)
 * - Escape key handling (close topmost modal on ESC)
 * - Backdrop click handling (close on backdrop click if enabled)
 */

import { useCallback, useEffect, useRef, useState } from "react";

export interface ModalConfig {
  id: string;
  title?: string;
  closeOnBackdropClick?: boolean;
  closeOnEscape?: boolean;
  focusTrap?: boolean;
  zIndex?: number;
}

export interface ModalHandle {
  open: (config: ModalConfig) => void;
  close: (id: string) => void;
  closeAll: () => void;
}

/**
 * useModalStack: Manage a stack of modals with deterministic behavior
 */
export function useModalStack(): ModalHandle {
  const [stack, setStack] = useState<ModalConfig[]>([]);
  const stackRef = useRef<ModalConfig[]>([]);

  const open = useCallback((config: ModalConfig) => {
    setStack((prev) => {
      const updated = [...prev, config];
      stackRef.current = updated;
      return updated;
    });
  }, []);

  const close = useCallback((id: string) => {
    setStack((prev) => {
      const updated = prev.filter((m) => m.id !== id);
      stackRef.current = updated;
      return updated;
    });
  }, []);

  const closeAll = useCallback(() => {
    setStack([]);
    stackRef.current = [];
  }, []);

  // Escape key handling
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.key !== "Escape" || stack.length === 0) return;

      const topmost = stack[stack.length - 1];
      if (topmost.closeOnEscape !== false) {
        close(topmost.id);
      }
    };

    window.addEventListener("keydown", handleKeyDown);
    return () => window.removeEventListener("keydown", handleKeyDown);
  }, [stack, close]);

  return { open, close, closeAll };
}

/**
 * ModalBackdrop: Renders a semi-transparent backdrop with optional click handling
 */
export function ModalBackdrop(props: { visible: boolean; zIndex: number; onClick?: () => void }) {
  if (!props.visible) return null;

  return (
    <button
      type="button"
      style={{
        position: "fixed",
        inset: 0,
        background: "rgba(0, 0, 0, 0.5)",
        zIndex: props.zIndex - 1,
        opacity: props.visible ? 1 : 0,
        transition: "opacity 200ms ease-in-out",
        pointerEvents: props.onClick ? "auto" : "none",
        border: "none",
        cursor: props.onClick ? "pointer" : "default",
      }}
      onClick={props.onClick}
      data-testid="modal-backdrop"
    />
  );
}

/**
 * FocusTrap: Manage keyboard focus within modal
 */
export function useFocusTrap(ref: React.RefObject<HTMLElement>, enabled: boolean) {
  useEffect(() => {
    if (!enabled || !ref.current) return;

    const element = ref.current;

    // Get all focusable elements within the modal
    const focusableSelectors = [
      "a[href]",
      "button:not([disabled])",
      "textarea:not([disabled])",
      'input[type="text"]:not([disabled])',
      'input[type="radio"]:not([disabled])',
      'input[type="checkbox"]:not([disabled])',
      "select:not([disabled])",
      '[tabindex]:not([tabindex="-1"])',
    ];

    const focusableElements = element.querySelectorAll(focusableSelectors.join(","));
    const firstFocusable = focusableElements[0] as HTMLElement | undefined;
    const lastFocusable = focusableElements[focusableElements.length - 1] as
      | HTMLElement
      | undefined;

    // Set initial focus
    if (firstFocusable && document.activeElement !== element) {
      firstFocusable.focus();
    }

    const handleKeyDown = (event: KeyboardEvent) => {
      if (event.key !== "Tab") return;

      if (event.shiftKey) {
        // Shift+Tab
        if (document.activeElement === firstFocusable) {
          event.preventDefault();
          lastFocusable?.focus();
        }
      } else {
        // Tab
        if (document.activeElement === lastFocusable) {
          event.preventDefault();
          firstFocusable?.focus();
        }
      }
    };

    element.addEventListener("keydown", handleKeyDown);
    return () => element.removeEventListener("keydown", handleKeyDown);
  }, [ref, enabled]);
}

/**
 * Example: Advanced modal component with stacking, focus trap, and escape handling
 */
export function AdvancedModal(props: {
  id: string;
  title: string;
  visible: boolean;
  zIndex: number;
  children: React.ReactNode;
  onClose: () => void;
  focusTrap?: boolean;
  closeOnBackdropClick?: boolean;
}) {
  const modalRef = useRef<HTMLDivElement>(null);

  useFocusTrap(modalRef as React.RefObject<HTMLElement>, props.focusTrap ?? true);

  if (!props.visible) return null;

  return (
    <>
      <ModalBackdrop
        visible={props.visible}
        zIndex={props.zIndex}
        onClick={props.closeOnBackdropClick !== false ? props.onClose : undefined}
      />
      <div
        ref={modalRef}
        style={{
          position: "fixed",
          inset: "20%",
          background: "#fff",
          zIndex: props.zIndex,
          borderRadius: "8px",
          boxShadow: "0 10px 40px rgba(0, 0, 0, 0.3)",
          display: "flex",
          flexDirection: "column",
          overflow: "hidden",
        }}
        role="dialog"
        aria-modal="true"
        aria-labelledby={`modal-title-${props.id}`}
        data-testid={`modal-${props.id}`}
      >
        <div style={{ padding: "16px", borderBottom: "1px solid #e0e0e0" }}>
          <h2 id={`modal-title-${props.id}`} style={{ margin: 0 }}>
            {props.title}
          </h2>
        </div>
        <div style={{ flex: 1, overflowY: "auto", padding: "16px" }}>{props.children}</div>
        <div
          style={{
            padding: "16px",
            borderTop: "1px solid #e0e0e0",
            display: "flex",
            justifyContent: "flex-end",
            gap: "8px",
          }}
        >
          <button
            type="button"
            onClick={props.onClose}
            style={{
              padding: "8px 16px",
              background: "#f0f0f0",
              border: "1px solid #ccc",
              borderRadius: "4px",
              cursor: "pointer",
            }}
          >
            Close
          </button>
        </div>
      </div>
    </>
  );
}
