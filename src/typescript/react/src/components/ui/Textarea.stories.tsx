import type { Meta, StoryObj } from "@storybook/react-vite";
import { Textarea } from "./textarea";

const meta = {
  title: "UI/Textarea",
  component: Textarea,
  parameters: { layout: "centered" },
  tags: ["autodocs"],
  argTypes: {
    disabled: { control: "boolean" },
    placeholder: { control: "text" },
    rows: { control: { type: "range", min: 2, max: 10 } },
  },
} satisfies Meta<typeof Textarea>;

export default meta;
type Story = StoryObj<typeof meta>;

export const Default: Story = {
  args: { placeholder: "Type a message…", rows: 3 },
};

export const WithContent: Story = {
  args: { value: "Tell me about this banana.", readOnly: true, rows: 3 },
};

export const Disabled: Story = {
  args: { placeholder: "Sending…", disabled: true, rows: 3 },
};
