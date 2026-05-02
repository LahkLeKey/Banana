// @banana/ui annotation: web (DOM-only).
// Slice 027 -- canonical retry button. Consumes the `escalation`
// token family for visual and pulls baseline copy from spec 023.

import type { ButtonHTMLAttributes } from "react";
import { tokens } from "../tokens";

export const RETRY_BUTTON_COPY = "Try again";

export type RetryButtonProps = Omit<
  ButtonHTMLAttributes<HTMLButtonElement>,
  "children" | "type"
> & {
  label?: string;
};

export function RetryButton({ label = RETRY_BUTTON_COPY, style, ...rest }: RetryButtonProps) {
  return (
    <button
      type="button"
      data-testid="retry-button"
      style={{
        backgroundColor: tokens.color.escalation.bg,
        color: tokens.color.escalation.fg,
        borderColor: tokens.color.escalation.accent,
        borderWidth: 1,
        borderStyle: "solid",
        borderRadius: tokens.radius.md,
        paddingInline: tokens.space[3],
        paddingBlock: tokens.space[2],
        fontSize: tokens.font.size.sm,
        fontWeight: tokens.font.weight.semibold,
        cursor: "pointer",
        ...style,
      }}
      {...rest}
    >
      {label}
    </button>
  );
}
