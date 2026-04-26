import type { ChatMessage } from '../types';

const ROLE_CLASSES: Record<ChatMessage['role'], string> = {
    user: 'ml-auto bg-yellow-200 text-yellow-900 border-yellow-300',
    assistant: 'mr-auto bg-lime-100 text-lime-900 border-lime-300',
    system: 'mx-auto bg-slate-100 text-slate-700 border-slate-300',
};

export function ChatMessageBubble({ message }: { message: ChatMessage }) {
    return (
        <article
            className={`w-full max-w-md rounded-lg border px-3 py-2 text-sm shadow-sm ${ROLE_CLASSES[message.role]}`}>
            <p className="whitespace-pre-wrap leading-snug">{message.content}</p>
            <p className="mt-1 text-[10px] uppercase tracking-wide opacity-70">
                {message.role} - {message.status}
            </p>
        </article>
    );
}