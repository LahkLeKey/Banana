import type { Meta, StoryObj } from "@storybook/react-vite";
import { fn } from "storybook/test";
import { Button } from "./ui/button";
import { Textarea } from "./ui/textarea";

interface ChatMessage {
  id: string;
  role: "user" | "assistant";
  content: string;
}

/** Standalone demo of the Chat panel controls. */
function ChatPanelDemo({
  ready = true,
  messages = [],
  onSend = fn(),
}: {
  ready?: boolean;
  messages?: ChatMessage[];
  onSend?: () => void;
}) {
  return (
    <div className="flex w-96 flex-col space-y-3" data-testid="chat-panel-demo">
      <div className="min-h-32 rounded-lg border p-3" data-testid="chat-messages">
        {messages.length === 0 ? (
          <p className="text-xs text-muted-foreground">No chat messages yet.</p>
        ) : (
          <ul className="space-y-2">
            {messages.map((m) => (
              <li key={m.id} className={m.role === "user" ? "text-right" : "text-left"}>
                <span
                  className={`inline-block rounded-md px-3 py-1 text-sm ${
                    m.role === "user"
                      ? "bg-primary text-primary-foreground"
                      : "bg-muted text-muted-foreground"
                  }`}
                >
                  {m.content}
                </span>
              </li>
            ))}
          </ul>
        )}
      </div>
      <form
        className="flex gap-2"
        onSubmit={(e) => {
          e.preventDefault();
          onSend();
        }}
      >
        <Textarea
          placeholder="Ask about the banana…"
          rows={2}
          disabled={!ready}
          className="flex-1"
        />
        <Button type="submit" disabled={!ready} size="sm">
          Send
        </Button>
      </form>
    </div>
  );
}

const meta = {
  title: "Panels/ChatPanel",
  component: ChatPanelDemo,
  parameters: { layout: "centered" },
  tags: ["autodocs"],
} satisfies Meta<typeof ChatPanelDemo>;

export default meta;
type Story = StoryObj<typeof meta>;

export const Empty: Story = {};

export const WithMessages: Story = {
  args: {
    messages: [
      { id: "1", role: "user", content: "Is this a ripe banana?" },
      { id: "2", role: "assistant", content: "Yes, the coloration suggests it is ripe." },
    ],
  },
};

export const Unavailable: Story = {
  args: { ready: false },
};
