import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";

const spikes = [
    {
        id: "161",
        title: "Suite Information Architecture Spike",
        focus: "Define spaces, navigation taxonomy, and role-based wayfinding.",
    },
    {
        id: "162",
        title: "Shadcn Confluence Workbench Spike",
        focus: "Validate article layout patterns, activity stream, and collaborative editing affordances.",
    },
    {
        id: "163",
        title: "Function Web App Capability Spike",
        focus: "Map function registry, execution guardrails, and auditable run outcomes.",
    },
    {
        id: "164",
        title: "BananaAI Rovo Replacement Spike",
        focus: "Define orchestration contract to combine custom models behind a unified BananaAI assistant.",
    },
    {
        id: "165",
        title: "Feedback Loop Promotion Spike",
        focus: "Define memory-first promotion loop from runtime/spec signals into prioritized follow-up slices.",
    },
];

export function ReviewSpikesPage() {
    return (
        <div className="space-y-4">
            <Card>
                <CardHeader>
                    <CardTitle>Follow-Up Review Spikes</CardTitle>
                    <CardDescription>
                        Scoped research slices to evolve the suite beyond classify/operator into a full workspace.
                    </CardDescription>
                </CardHeader>
            </Card>

            <div className="grid gap-3">
                {spikes.map((spike) => (
                    <Card key={spike.id}>
                        <CardHeader>
                            <CardTitle className="text-base">{spike.id} - {spike.title}</CardTitle>
                        </CardHeader>
                        <CardContent className="text-sm text-muted-foreground">{spike.focus}</CardContent>
                    </Card>
                ))}
            </div>
        </div>
    );
}
