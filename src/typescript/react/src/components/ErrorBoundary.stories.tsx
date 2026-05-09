import type { Meta, StoryObj } from "@storybook/react-vite";
import type { ReactNode } from "react";
import { ErrorBoundary } from "./ErrorBoundary";

/** Throws on render to demonstrate the ErrorBoundary fallback. */
function BrokenChild(): ReactNode {
  throw new Error("Simulated render crash");
}

function ErrorBoundaryShowcase({ variant }: { variant: "fallback" | "custom" | "healthy" }) {
  if (variant === "healthy") {
    return (
      <ErrorBoundary>
        <p className="rounded-lg border p-4 text-sm text-muted-foreground">
          No error — children render normally.
        </p>
      </ErrorBoundary>
    );
  }
  if (variant === "custom") {
    return (
      <ErrorBoundary
        fallback={(error, reset) => (
          <div
            className="rounded-lg border border-destructive p-4 text-sm text-destructive"
            role="alert"
          >
            <p className="font-medium">Custom fallback: {error.message}</p>
            <button onClick={reset} className="mt-2 underline">
              Reset
            </button>
          </div>
        )}
      >
        <BrokenChild />
      </ErrorBoundary>
    );
  }
  return (
    <ErrorBoundary>
      <BrokenChild />
    </ErrorBoundary>
  );
}

const meta = {
  title: "Panels/ErrorBoundary",
  component: ErrorBoundaryShowcase,
  parameters: { layout: "centered" },
  tags: ["autodocs"],
  decorators: [
    (Story: () => ReactNode) => (
      <div className="w-96">
        <Story />
      </div>
    ),
  ],
  argTypes: {
    variant: { control: "select", options: ["fallback", "custom", "healthy"] },
  },
} satisfies Meta<typeof ErrorBoundaryShowcase>;

export default meta;
type Story = StoryObj<typeof meta>;

export const FallbackTriggered: Story = {
  args: { variant: "fallback" },
};

export const CustomFallback: Story = {
  args: { variant: "custom" },
};

export const Healthy: Story = {
  args: { variant: "healthy" },
};
