import { useMemo, useState } from "react";
import SwaggerUI from "swagger-ui-react";
import "swagger-ui-react/swagger-ui.css";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Button } from "../components/ui/button";
import { resolveApiBaseUrl } from "../lib/api";

const FALLBACK_SPEC_URL = "https://api.banana.engineer/swagger/v1/swagger.json";

export function ApiDocsPage() {
    const resolvedBase = resolveApiBaseUrl();
    const defaultSpecUrl = useMemo(() => {
        if (!resolvedBase) return FALLBACK_SPEC_URL;
        return `${resolvedBase.replace(/\/$/, "")}/swagger/v1/swagger.json`;
    }, [resolvedBase]);

    const [specUrl, setSpecUrl] = useState(defaultSpecUrl);

    return (
        <div className="space-y-4">
            <Card>
                <CardHeader>
                    <CardTitle>API Docs</CardTitle>
                    <CardDescription>
                        Interactive Swagger docs embedded in Banana Workspace. Use this to inspect endpoints and execute test calls directly.
                    </CardDescription>
                </CardHeader>
                <CardContent className="space-y-3">
                    <label className="block space-y-1 text-sm">
                        <span className="text-muted-foreground">OpenAPI Spec URL</span>
                        <input
                            className="w-full rounded-md border bg-background px-3 py-2"
                            value={specUrl}
                            onChange={(event) => setSpecUrl(event.target.value)}
                            placeholder="https://api.banana.engineer/swagger/v1/swagger.json"
                        />
                    </label>
                    <div className="flex flex-wrap gap-2">
                        <Button variant="outline" onClick={() => setSpecUrl(defaultSpecUrl)}>Use API Base Default</Button>
                        <Button variant="outline" asChild>
                            <a href="https://api.banana.engineer/swagger" target="_blank" rel="noreferrer">Open Raw Swagger UI</a>
                        </Button>
                    </div>
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Interactive API Console</CardTitle>
                    <CardDescription>
                        If this panel is empty, ensure the API host exposes Swagger at /swagger/v1/swagger.json.
                    </CardDescription>
                </CardHeader>
                <CardContent>
                    <div className="rounded-md border bg-white p-2">
                        <SwaggerUI url={specUrl} docExpansion="list" defaultModelsExpandDepth={1} />
                    </div>
                </CardContent>
            </Card>
        </div>
    );
}
