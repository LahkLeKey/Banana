import { type ReactNode } from "react";
import { NavLink, Outlet } from "react-router-dom";
import { Card, CardContent, CardHeader, CardTitle } from "./ui/card";

function NavItem({ to, label }: { to: string; label: string }) {
    return (
        <NavLink
            to={to}
            className={({ isActive }) =>
                [
                    "rounded-md px-3 py-2 text-sm transition-colors",
                    isActive ? "bg-primary text-primary-foreground" : "text-muted-foreground hover:bg-secondary hover:text-secondary-foreground",
                ].join(" ")
            }
        >
            {label}
        </NavLink>
    );
}

function Section({ title, children }: { title: string; children: ReactNode }) {
    return (
        <Card>
            <CardHeader className="pb-2">
                <CardTitle className="text-sm">{title}</CardTitle>
            </CardHeader>
            <CardContent className="flex flex-col gap-1">{children}</CardContent>
        </Card>
    );
}

export function WorkspaceShell() {
    return (
        <div className="min-h-screen bg-gradient-to-b from-background to-secondary/40">
            <header className="border-b bg-background/90 backdrop-blur">
                <div className="mx-auto flex max-w-7xl items-center justify-between px-4 py-3">
                    <div>
                        <p className="text-xs uppercase tracking-widest text-muted-foreground">Banana Suite</p>
                        <h1 className="text-lg font-semibold">Banana Workspace</h1>
                    </div>
                    <p className="text-xs text-muted-foreground">Model ops · knowledge · operator tools</p>
                </div>
            </header>

            <div className="mx-auto grid max-w-7xl gap-4 p-4 lg:grid-cols-[260px_1fr]">
                <aside className="space-y-3">
                    <Section title="Workspace">
                        <NavItem to="/workspace" label="Home" />
                        <NavItem to="/knowledge" label="Knowledge" />
                        <NavItem to="/functions" label="Functions" />
                        <NavItem to="/api-docs" label="API Docs" />
                        <NavItem to="/data-science" label="Data Science" />
                        <NavItem to="/banana-ai" label="BananaAI" />
                        <NavItem to="/review-spikes" label="Review Spikes" />
                    </Section>

                    <Section title="Classifier Tools">
                        <NavItem to="/classify" label="Classify" />
                        <NavItem to="/operator" label="Operator" />
                    </Section>
                </aside>

                <main>
                    <Outlet />
                </main>
            </div>
        </div>
    );
}
