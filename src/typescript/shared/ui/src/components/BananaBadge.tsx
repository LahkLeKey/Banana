// @banana/ui annotation: cross-platform (no DOM-only deps).
import type { ReactNode } from "react";

export function BananaBadge({ count, children }: { count: number; children?: ReactNode }) {
    return (
        <span className="inline-flex items-center gap-1 rounded-md bg-yellow-200 px-2 py-1 text-xs font-medium text-yellow-900">
            🍌 {count}
            {children}
        </span>
    );
}
