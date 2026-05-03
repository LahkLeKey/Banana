import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Textarea } from "../components/ui/textarea";
import { Button } from "../components/ui/button";

const modelLanes = [
    { name: "Banana Classifier", role: "Primary classification and ensemble verdict" },
    { name: "Ripeness Model", role: "Ripeness scoring and confidence explanation" },
    { name: "Not-Banana Detector", role: "Out-of-domain rejection and escalation" },
    { name: "Operator Reasoning", role: "Runbook-aware operational guidance" },
];

export function BananaAIPage() {
    return (
        <div className="space-y-4">
            <Card>
                <CardHeader>
                    <CardTitle>BananaAI</CardTitle>
                    <CardDescription>
                        Unified assistant scaffold that composes Banana custom models into a single Rovo-style replacement experience.
                    </CardDescription>
                </CardHeader>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Ask BananaAI</CardTitle>
                    <CardDescription>
                        Placeholder orchestration panel. Follow-up implementation wires intent routing and model execution.
                    </CardDescription>
                </CardHeader>
                <CardContent className="space-y-3">
                    <Textarea placeholder="Ask about classification, ripeness, model drift, or operational actions..." rows={5} />
                    <Button disabled>Run BananaAI (scaffold)</Button>
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Model Lanes</CardTitle>
                </CardHeader>
                <CardContent className="grid gap-2 text-sm">
                    {modelLanes.map((lane) => (
                        <div key={lane.name} className="rounded-md border p-3">
                            <p className="font-medium">{lane.name}</p>
                            <p className="text-muted-foreground">{lane.role}</p>
                        </div>
                    ))}
                </CardContent>
            </Card>
        </div>
    );
}
