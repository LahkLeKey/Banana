using Microsoft.AspNetCore.Mvc;
using System.Collections.Concurrent;

namespace Banana.Api.Controllers;

[ApiController]
public sealed class ChatController : ControllerBase
{
    // In-process session store (ephemeral per instance)
    private static readonly ConcurrentDictionary<string, ChatSession> Sessions = new();

    [HttpPost("/chat/sessions")]
    public IActionResult CreateSession([FromBody] CreateSessionRequest? body)
    {
        var platform = (body?.Platform ?? "web").Trim();
        if (platform.Length == 0) platform = "web";

        var now = DateTime.UtcNow.ToString("o");
        var session = new ChatSession
        {
            Id = $"chat_{Guid.NewGuid():N}",
            Platform = platform,
            CreatedAt = now,
            UpdatedAt = now,
        };

        var welcome = session.AddMessage("assistant",
            "banana assistant ready; send a message to classify banana context deterministically",
            "complete");

        Sessions[session.Id] = session;

        return StatusCode(201, new
        {
            session = session.ToEcho(),
            metadata = body?.Metadata ?? new Dictionary<string, string>(),
            welcome_message = welcome,
        });
    }

    [HttpGet("/chat/sessions/{sessionId}")]
    public IActionResult GetSession(string sessionId)
    {
        if (!Sessions.TryGetValue(sessionId, out var session))
            return NotFound(TypedError("session_not_found", "chat session was not found"));

        return Ok(new { session = session.ToEcho(), messages = session.Messages });
    }

    [HttpPost("/chat/sessions/{sessionId}/messages")]
    public IActionResult PostMessage(string sessionId, [FromBody] PostMessageRequest? body)
    {
        if (string.IsNullOrWhiteSpace(body?.Content))
            return BadRequest(TypedError("invalid_argument", "content is required"));

        if (!Sessions.TryGetValue(sessionId, out var session))
            return NotFound(TypedError("session_not_found", "chat session was not found"));

        var clientId = body.ClientMessageId?.Trim();
        if (clientId != null && session.Idempotency.TryGetValue(clientId, out var existing))
        {
            return Ok(new
            {
                session_id = session.Id,
                duplicate = true,
                user_message = existing.UserMessage,
                assistant_message = existing.AssistantMessage,
            });
        }

        var userMsg = session.AddMessage("user", body.Content.Trim(), "accepted");
        var reply = BuildReply(body.Content);
        var assistantMsg = session.AddMessage("assistant", reply, "complete");

        if (clientId != null)
            session.Idempotency[clientId] = new IdempotencyEntry(userMsg, assistantMsg);

        return StatusCode(201, new
        {
            session_id = session.Id,
            duplicate = false,
            user_message = userMsg,
            assistant_message = assistantMsg,
        });
    }

    private static string BuildReply(string content)
    {
        // Simple keyword-based banana signal without native model dependency
        var lower = content.ToLowerInvariant();
        var bananaTerms = new[] { "banana", "ripe", "yellow", "peel", "bunch", "fruit" };
        var matched = bananaTerms.Where(t => lower.Contains(t)).ToArray();

        if (matched.Length > 0)
            return $"banana-forward signal detected ({string.Join(",", matched)}) [model=keyword-fallback]";

        return $"not-banana signal is stronger right now; include banana context for a fruit-focused answer [model=keyword-fallback]";
    }

    private static object TypedError(string code, string message) => new
    {
        error = new { code, message, retryable = false, retry_hint = "fix_request_then_retry" }
    };
}

public sealed class CreateSessionRequest
{
    public string? Platform { get; set; }
    public Dictionary<string, string>? Metadata { get; set; }
}

public sealed class PostMessageRequest
{
    public string? Content { get; set; }
    public string? ClientMessageId { get; set; }
}

public sealed class ChatMessage
{
    public string Id { get; set; } = "";
    public string SessionId { get; set; } = "";
    public string Role { get; set; } = "";
    public string Content { get; set; } = "";
    public string CreatedAt { get; set; } = "";
    public string Status { get; set; } = "";
}

public sealed class IdempotencyEntry(ChatMessage userMessage, ChatMessage assistantMessage)
{
    public ChatMessage UserMessage { get; } = userMessage;
    public ChatMessage AssistantMessage { get; } = assistantMessage;
}

public sealed class ChatSession
{
    public string Id { get; set; } = "";
    public string Platform { get; set; } = "";
    public string CreatedAt { get; set; } = "";
    public string UpdatedAt { get; set; } = "";
    private int _seq;
    public List<ChatMessage> Messages { get; } = [];
    public Dictionary<string, IdempotencyEntry> Idempotency { get; } = new();

    public ChatMessage AddMessage(string role, string content, string status)
    {
        _seq++;
        var msg = new ChatMessage
        {
            Id = $"{Id}_m{_seq:D4}",
            SessionId = Id,
            Role = role,
            Content = content,
            CreatedAt = DateTime.UtcNow.ToString("o"),
            Status = status,
        };
        Messages.Add(msg);
        UpdatedAt = msg.CreatedAt;
        return msg;
    }

    public object ToEcho() => new
    {
        id = Id,
        platform = Platform,
        created_at = CreatedAt,
        updated_at = UpdatedAt,
        message_count = Messages.Count,
    };
}
