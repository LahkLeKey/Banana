import type { Meta, StoryObj } from "@storybook/react-vite";
import { fn } from "storybook/test";
import { Button } from "./ui/button";
import { Input } from "./ui/input";
import { Card, CardContent } from "./ui/card";
import { Alert, AlertDescription, AlertTitle } from "./ui/alert";

/** Standalone demo of the Ensemble prediction panel controls. */
function EnsemblePanelDemo({
  loading = false,
  error = null,
  verdict = null,
  onSubmit = fn(),
}: {
  loading?: boolean;
  error?: string | null;
  verdict?: string | null;
  onSubmit?: () => void;
}) {
  return (
    <div className="w-96 space-y-4" data-testid="ensemble-panel-demo">
      <form
        className="space-y-2"
        onSubmit={(e) => {
          e.preventDefault();
          onSubmit();
        }}
        data-testid="ensemble-form"
      >
        <Input placeholder="Paste an image URL…" disabled={loading} />
        <Button type="submit" disabled={loading} className="w-full">
          {loading ? "Predicting…" : "Predict ensemble"}
        </Button>
      </form>
      {error !== null && (
        <Alert variant="destructive" data-testid="ensemble-error-alert">
          <AlertTitle>Prediction failed</AlertTitle>
          <AlertDescription>{error}</AlertDescription>
        </Alert>
      )}
      {verdict !== null && (
        <Card data-testid="ensemble-verdict-surface">
          <CardContent className="pt-6">
            <p className="font-medium text-foreground" data-testid="ensemble-verdict-copy">
              {verdict}
            </p>
          </CardContent>
        </Card>
      )}
    </div>
  );
}

const meta = {
  title: "Panels/EnsemblePanel",
  component: EnsemblePanelDemo,
  parameters: { layout: "centered" },
  tags: ["autodocs"],
} satisfies Meta<typeof EnsemblePanelDemo>;

export default meta;
type Story = StoryObj<typeof meta>;

export const Idle: Story = {};

export const Loading: Story = {
  args: { loading: true },
};

export const VerdictSuccess: Story = {
  args: { verdict: "Banana (confidence: 0.97)" },
};

export const VerdictError: Story = {
  args: { error: "The API could not process the image. Please try again." },
};
