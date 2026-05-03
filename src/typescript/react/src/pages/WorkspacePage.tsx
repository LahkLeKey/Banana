import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";

const modules = [
    {
        title: "Knowledge Space",
        description: "Confluence-style documentation space for domain narratives, ADRs, and runbooks.",
    },
    {
        title: "Functions Catalog",
        description: "Function web-app registry for executable automation and operator actions.",
    },
    {
        title: "Model Ops Review",
        description: "Review lane for classifier readiness, drift summaries, and evidence bundles.",
    },
    {
        title: "Runtime Command Center",
        description: "Cross-environment runtime checks, deploy hooks, and incident breadcrumbs.",
    },
];

export function WorkspacePage() {
    return (
        <div className="space-y-4">
            <Card>
                <CardHeader>
                    <CardTitle>Workspace Overview</CardTitle>
                    <CardDescription>
                        This shell expands Banana from a single classifier screen into a suite-style app surface.
                    </CardDescription>
                </CardHeader>
            </Card>

            <div className="grid gap-4 md:grid-cols-2">
                {modules.map((module) => (
                    <Card key={module.title}>
                        <CardHeader>
                            <CardTitle className="text-base">{module.title}</CardTitle>
                            <CardDescription>{module.description}</CardDescription>
                        </CardHeader>
                    </Card>
                ))}
            </div>

            <Card>
                <CardHeader>
                    <CardTitle>Current Delivery Status</CardTitle>
                </CardHeader>
                <CardContent className="text-sm text-muted-foreground">
                    The classify flow remains live while the suite information architecture and function surfaces are staged through follow-up review spikes.
                </CardContent>
            </Card>
        </div>
    );
}
