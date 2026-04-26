export type Ripeness = 'green'|'yellow'|'spotted'|'brown';

export type ChatRole = 'user'|'assistant'|'system';
export type ChatMessageStatus = 'accepted'|'complete';

export type ChatMessage = {
  id: string; session_id: string; role: ChatRole; content: string;
  created_at: string;
  status: ChatMessageStatus;
};

export type ChatSession = {
  id: string; platform: string; created_at: string; updated_at: string;
  message_count: number;
};
