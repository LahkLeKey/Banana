// @banana/ui annotation: web (DOM-only).
// Slice 027 -- canonical error wording surface. Pulls token color
// for the baseline error tone.

import type { HTMLAttributes, ReactNode } from "react";
import { tokens } from "../tokens";

export type ErrorTextProps = Omit<HTMLAttributes<HTMLParagraphElement>, "children"> & {
  children: ReactNode;
};

export function ErrorText({ children, style, ...rest }: ErrorTextProps) {
  return (
    <p
      data-testid="error-text"
      role="alert"
      style={{
        color: tokens.color.text.error,
        fontSize: tokens.font.size.xs,
        margin: 0,
        ...style,
      }}
      {...rest}
    >
      {children}
    </p>
  );
}
