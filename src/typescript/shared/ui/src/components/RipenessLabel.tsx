// @banana/ui annotation: cross-platform.
import type { Ripeness } from "../types.ts";

const COLORS: Record<Ripeness, string> = {
    green: "bg-green-200 text-green-900",
    yellow: "bg-yellow-200 text-yellow-900",
    spotted: "bg-amber-300 text-amber-900",
    brown: "bg-amber-700 text-amber-50",
};

export function RipenessLabel({ value }: { value: Ripeness }) {
    return <span className={`rounded px-1.5 py-0.5 text-xs ${COLORS[value]}`}>{value}</span>;
}
