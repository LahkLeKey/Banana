import type { Meta, StoryObj } from "@storybook/react-vite";
import { Input } from "./input";

const meta = {
  title: "UI/Input",
  component: Input,
  parameters: { layout: "centered" },
  tags: ["autodocs"],
  argTypes: {
    type: { control: "select", options: ["text", "number", "email", "password", "file"] },
    disabled: { control: "boolean" },
    placeholder: { control: "text" },
  },
} satisfies Meta<typeof Input>;

export default meta;
type Story = StoryObj<typeof meta>;

export const Default: Story = {
  args: { placeholder: "Paste an image URL…" },
};

export const WithValue: Story = {
  args: { value: "https://example.com/banana.jpg", readOnly: true },
};

export const Disabled: Story = {
  args: { placeholder: "Disabled input", disabled: true },
};

export const FileInput: Story = {
  args: { type: "file", accept: "image/*" },
};
