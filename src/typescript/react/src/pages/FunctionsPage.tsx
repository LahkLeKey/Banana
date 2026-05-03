import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";

const functions = [
    { name: "Drain Spec Queue", owner: "Automation", status: "Ready" },
    { name: "Promote Vercel Release", owner: "Delivery", status: "Blocked" },
    { name: "Capture Host Inventory", owner: "Platform", status: "Ready" },
    { name: "Validate Runtime Contracts", owner: "Integration", status: "Ready" },
];

export function FunctionsPage() {
    return (
        <div className="space-y-4">
            <Card>
                <CardHeader>
                    <CardTitle>Function Catalog</CardTitle>
                    <CardDescription>
                        Function web-app scaffold for actionable automations and workflow entry points.
                    </CardDescription>
                </CardHeader>
            </Card>

            <div className="grid gap-3">
                {functions.map((item) => (
                    <Card key={item.name}>
                        <CardContent className="flex items-center justify-between py-4 text-sm">
                            <div>
                                <p className="font-medium">{item.name}</p>
                                <p className="text-muted-foreground">Owner: {item.owner}</p>
                            </div>
                            <span className="rounded-full border px-2 py-1 text-xs">{item.status}</span>
                        </CardContent>
                    </Card>
                ))}
            </div>
        </div>
    );
}
