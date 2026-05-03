import { Button } from "../ui/button";
import { SCAFFOLDS } from "../../lib/dsConstants";
import type { EndpointCatalogState } from "../../lib/dsTypes";
import type { EndpointOperation } from "../../lib/dataScienceBootstrap";

type AdvancedToolsProps = {
    catalogState: EndpointCatalogState;
    endpointOperations: EndpointOperation[];
    endpointFilter: string;
    onFilterChange: (v: string) => void;
    filteredOperations: EndpointOperation[];
    showApiTools: boolean;
    onToggleApiTools: () => void;
    onRefreshCatalog: () => void;
    onInjectEndpoint: (op: EndpointOperation) => void;
    onAddScaffold: (source: string) => void;
    onLoadApiBootstrap: () => void;
};

const CATALOG_STATE_CLASS: Record<EndpointCatalogState, string> = {
    idle: "bg-slate-100 text-slate-500",
    loading: "bg-amber-100 text-amber-600",
    ready: "bg-emerald-100 text-emerald-700",
    error: "bg-red-100 text-red-600",
};

export function AdvancedTools({
    catalogState,
    endpointOperations,
    endpointFilter,
    onFilterChange,
    filteredOperations,
    showApiTools,
    onToggleApiTools,
    onRefreshCatalog,
    onInjectEndpoint,
    onAddScaffold,
    onLoadApiBootstrap,
}: AdvancedToolsProps) {
    return (
        <div className="rounded-lg border border-dashed border-slate-300 bg-white p-4">
            <div className="flex flex-wrap items-start gap-5 lg:flex-nowrap">
                {/* Scaffold buttons */}
                <div className="min-w-0 flex-1">
                    <p className="mb-2 text-[10px] font-semibold uppercase tracking-wider text-slate-500">Scaffolds</p>
                    <div className="flex flex-wrap gap-1.5">
                        <Button size="sm" variant="outline" className="h-7 text-xs" onClick={onLoadApiBootstrap}>
                            API Bootstrap
                        </Button>
                        {SCAFFOLDS.map((s) => (
                            <Button
                                key={s.label}
                                size="sm"
                                variant="outline"
                                className="h-7 text-xs"
                                onClick={() => onAddScaffold(s.source)}
                            >
                                {s.label}
                            </Button>
                        ))}
                    </div>
                </div>

                {/* API catalog */}
                <div className="w-full shrink-0 lg:w-72">
                    <div className="flex items-center justify-between gap-2">
                        <p className="text-[10px] font-semibold uppercase tracking-wider text-slate-500">
                            API Catalog
                            <span className={`ml-1.5 rounded px-1.5 py-0.5 font-mono text-[10px] ${CATALOG_STATE_CLASS[catalogState]}`}>
                                {catalogState}
                            </span>
                        </p>
                        <div className="flex gap-1">
                            <Button size="sm" variant="outline" className="h-6 px-2 text-[11px]" onClick={onRefreshCatalog}>
                                Refresh
                            </Button>
                            <Button
                                size="sm"
                                variant={showApiTools ? "secondary" : "outline"}
                                className="h-6 px-2 text-[11px]"
                                onClick={onToggleApiTools}
                            >
                                {showApiTools ? "Hide" : `Show (${endpointOperations.length})`}
                            </Button>
                        </div>
                    </div>

                    {showApiTools && (
                        <div className="mt-2">
                            <input
                                className="w-full rounded border border-slate-200 bg-white px-2 py-1 text-xs focus:outline-none focus:ring-1 focus:ring-slate-300"
                                placeholder="Filter endpoints…"
                                value={endpointFilter}
                                onChange={(e) => onFilterChange(e.target.value)}
                            />
                            <div className="mt-1 max-h-48 space-y-px overflow-y-auto rounded border border-slate-200 bg-slate-50 p-1">
                                {filteredOperations.slice(0, 16).map((op, i) => (
                                    <div
                                        key={`${op.method}-${op.path}-${i}`}
                                        className="flex items-center justify-between gap-2 rounded px-2 py-1 hover:bg-white"
                                    >
                                        <span className="min-w-0 font-mono text-[11px] text-slate-700">
                                            <span className="mr-1.5 font-bold">{op.method}</span>
                                            <span className="truncate text-slate-500">{op.path}</span>
                                        </span>
                                        <Button
                                            size="sm"
                                            variant="outline"
                                            className="h-6 shrink-0 px-2 text-[10px]"
                                            onClick={() => onInjectEndpoint(op)}
                                        >
                                            Inject
                                        </Button>
                                    </div>
                                ))}
                                {filteredOperations.length === 0 && (
                                    <p className="px-2 py-2 text-[11px] text-slate-400">No matching endpoints.</p>
                                )}
                            </div>
                        </div>
                    )}
                </div>
            </div>
        </div>
    );
}
