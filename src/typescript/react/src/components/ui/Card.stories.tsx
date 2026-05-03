import type { Meta, StoryObj } from "@storybook/react-vite";
import { Card, CardContent, CardDescription, CardFooter, CardHeader, CardTitle } from "./card";
import { Button } from "./button";

const meta = {
  title: "UI/Card",
  component: Card,
  parameters: { layout: "centered" },
  tags: ["autodocs"],
  decorators: [
    (Story) => (
      <div className="w-96">
        <Story />
      </div>
    ),
  ],
} satisfies Meta<typeof Card>;

export default meta;
type Story = StoryObj<typeof meta>;

export const Default: Story = {
  render: () => (
    <Card>
      <CardHeader>
        <CardTitle>Ensemble Result</CardTitle>
        <CardDescription>Prediction from the banana classifier</CardDescription>
      </CardHeader>
      <CardContent>
        <p className="text-sm text-muted-foreground">
          Based on the image, the model ensemble determined this is a banana with high confidence.
        </p>
      </CardContent>
      <CardFooter>
        <Button size="sm" variant="outline">
          New prediction
        </Button>
      </CardFooter>
    </Card>
  ),
};

export const Minimal: Story = {
  render: () => (
    <Card>
      <CardContent className="pt-6">
        <p className="text-sm">Simple card with only content.</p>
      </CardContent>
    </Card>
  ),
};
