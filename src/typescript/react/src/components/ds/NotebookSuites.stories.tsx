import type { Meta, StoryObj } from "@storybook/react-vite";
import { NOTEBOOK_LIBRARY, RUNTIME_CONTRACTS } from "../../lib/dsConstants";
import { NotebookCatalog } from "./NotebookCatalog";
import { NotebookReportView } from "./NotebookReportView";
import { NotebookToolbar } from "./NotebookToolbar";
import { OperationsRail } from "./OperationsRail";
import { getSuiteFixture, suiteFixtures } from "./storybook/dsStoryFixtures";

function SuiteShowcase({ suiteLabel }: { suiteLabel: string }) {
    const suite = getSuiteFixture(suiteLabel);

    return (
        <div className="w-[1500px] max-w-full space-y-4 bg-[radial-gradient(circle_at_top,_rgba(59,130,246,0.08),_transparent_42%),linear-gradient(180deg,#f8fafc,#eef2ff_60%,#f8fafc)] p-6">
            <NotebookToolbar
                notebookLabel={suite.label}
                cellCount={suite.cells.length}
                executedCount={suite.cells.filter((cell) => cell.execCount != null).length}
                runtimeState="ready"
                runtimeMessage="Storybook fixture with warmed runtime state."
                showLibrary={true}
                onRunAll={() => {}}
                onSave={() => {}}
                onExport={() => {}}
                onReset={() => {}}
                onToggleCatalog={() => {}}
                onWarmRuntime={() => {}}
            />

            <div className="grid gap-4 xl:grid-cols-[280px_minmax(0,1fr)]">
                <NotebookCatalog
                    activeNotebookLabel={suite.label}
                    library={NOTEBOOK_LIBRARY}
                    onLoadNotebook={() => {}}
                    onClose={() => {}}
                    onAddPythonCell={() => {}}
                    onAddMarkdownCell={() => {}}
                    onToggleAdvancedTools={() => {}}
                    showAdvancedTools={false}
                />
                <NotebookReportView cells={suite.cells} />
            </div>

            <OperationsRail
                runtimeState="ready"
                runtimeMessage={`${suite.description} Storybook preview state.`}
                importText={`{\n  "nbformat": 4,\n  "cells": []\n}`}
                onImportTextChange={() => {}}
                onImport={() => {}}
                contracts={RUNTIME_CONTRACTS}
            />
        </div>
    );
}

const meta = {
    title: "DS/Notebook Suites",
    component: SuiteShowcase,
    parameters: {
        layout: "fullscreen",
    },
    args: {
        suiteLabel: suiteFixtures[0]?.label,
    },
    argTypes: {
        suiteLabel: {
            control: "select",
            options: suiteFixtures.map((suite) => suite.label),
        },
    },
} satisfies Meta<typeof SuiteShowcase>;

export default meta;

type Story = StoryObj<typeof meta>;

export const PlatformSetup: Story = {
    args: { suiteLabel: "01 - Platform Setup" },
};

export const BananaClassifierWorkbench: Story = {
    args: { suiteLabel: "02 - Banana Classifier Workbench" },
};

export const RipenessAndModelOps: Story = {
    args: { suiteLabel: "03 - Ripeness and ML Model Ops" },
};

export const LogisticsTrucksAndHarvest: Story = {
    args: { suiteLabel: "04 - Logistics Trucks and Harvest" },
};

export const ChatSessionsAndAudit: Story = {
    args: { suiteLabel: "05 - Chat Sessions and Operator Audit" },
};

export const FullApiSmokeSuite: Story = {
    args: { suiteLabel: "06 - Full API Contract Smoke Suite" },
};

export const VisualizationWorkbench: Story = {
    args: { suiteLabel: "07 - Visualization Workbench" },
};