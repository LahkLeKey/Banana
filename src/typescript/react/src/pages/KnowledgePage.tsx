import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";

export function KnowledgePage() {
    return (
        <div className="grid gap-4 lg:grid-cols-[220px_1fr_280px]">
            <Card>
                <CardHeader className="pb-2">
                    <CardTitle className="text-sm">Spaces</CardTitle>
                </CardHeader>
                <CardContent className="space-y-2 text-sm text-muted-foreground">
                    <p>Platform Engineering</p>
                    <p>Model Runtime</p>
                    <p>Delivery Contracts</p>
                    <p>Incident Playbooks</p>
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle>Deployment Readiness Playbook</CardTitle>
                    <CardDescription>Confluence-style article canvas scaffold</CardDescription>
                </CardHeader>
                <CardContent className="space-y-3 text-sm">
                    <p>
                        Objective: keep release readiness evidence linked to runtime contracts, rollout checks, and post-deploy health thresholds.
                    </p>
                    <p className="text-muted-foreground">
                        This page is intentionally scaffolded as a knowledge workbench placeholder and will be expanded in the shadcn-confluence spike.
                    </p>
                </CardContent>
            </Card>

            <Card>
                <CardHeader className="pb-2">
                    <CardTitle className="text-sm">Activity</CardTitle>
                </CardHeader>
                <CardContent className="space-y-2 text-sm text-muted-foreground">
                    <p>Spec-drain checkpoint published</p>
                    <p>Fly API deployment validated</p>
                    <p>Vercel production alias refreshed</p>
                </CardContent>
            </Card>
        </div>
    );
}
