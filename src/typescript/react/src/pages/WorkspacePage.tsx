import { Link } from "react-router-dom";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";

const modules = [
    {
        title: "Knowledge Space",
        description: "Confluence-style documentation space for domain narratives, ADRs, and runbooks.",
        href: "/knowledge",
    },
    {
        title: "Functions Catalog",
        description: "Function web-app registry for executable automation and operator actions.",
        href: "/functions",
    },
    {
        title: "BananaAI",
        description: "Unified assistant combining custom models — Banana Classifier, Ripeness, Not-Banana Detector.",
        href: "/banana-ai",
    },
    {
        title: "Operator",
        description: "Live chat interface backed by the banana classification model.",
        href: "/operator",
    },
    {
        title: "Classify",
        description: "Direct banana image and text classification with ensemble verdict.",
        href: "/classify",
    },
    {
        title: "Review Spikes",
        description: "Scoped research slices: suite IA, Confluence workbench, Functions, BananaAI, and feedback-loop promotion.",
        href: "/review-spikes",
    },
];

export function WorkspacePage() {
    return (
        <div className="space-y-4">
            <Card>
                <CardHeader>
                    <CardTitle>Workspace</CardTitle>
                    <CardDescription>
                        banana.engineer — live at <a href="https://banana.engineer" className="underline" target="_blank" rel="noreferrer">banana.engineer</a> · API at <a href="https://api.banana.engineer" className="underline" target="_blank" rel="noreferrer">api.banana.engineer</a>
                    </CardDescription>
                </CardHeader>
            </Card>

            <div className="grid gap-4 md:grid-cols-2 lg:grid-cols-3">
                {modules.map((module) => (
                    <Link key={module.title} to={module.href} className="block group">
                        <Card className="h-full transition-colors group-hover:border-foreground/40">
                            <CardHeader>
                                <CardTitle className="text-base">{module.title}</CardTitle>
                                <CardDescription>{module.description}</CardDescription>
                            </CardHeader>
                        </Card>
                    </Link>
                ))}
            </div>
        </div>
    );
}
