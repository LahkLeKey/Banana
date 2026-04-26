// @banana/ui annotation: cross-platform.
import type { Ripeness } from "../types";

const COLORS: Record<Ripeness, string> = {
    ripe: "bg-yellow-200 text-yellow-900",
    unripe: "bg-green-200 text-green-900",
    overripe: "bg-amber-700 text-amber-50",
};

export function RipenessLabel({ value }: { value: Ripeness }) {
    const style = COLORS[value] ?? "bg-gray-100 text-gray-700";
    return <span className={`rounded px-1.5 py-0.5 text-xs ${style}`}>{value}</span>;
}
