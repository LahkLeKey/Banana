using CInteropSharp.Api.Pipeline;
using CInteropSharp.Api.Pipeline.Steps;

using Microsoft.Extensions.Logging;

using Xunit;

namespace CInteropSharp.UnitTests;

public sealed class AuditStepTests
{
    [Fact]
    public void Order_IsExpected()
    {
        var step = new AuditStep(new RecordingLogger<AuditStep>());
        Assert.Equal(400, step.Order);
    }

    [Fact]
    public void Execute_UsesDefaultBonusWhenMissing()
    {
        var logger = new RecordingLogger<AuditStep>();
        var step = new AuditStep(logger);
        var context = new PipelineContext
        {
            Purchases = 5,
            Multiplier = 2
        };

        var output = step.Execute(context, input =>
        {
            input.NativeResult = 42;
            return input;
        });

        Assert.Same(context, output);
        Assert.Contains(logger.Messages, message => message.Contains("bonusBanana=0", StringComparison.Ordinal));
    }

    [Fact]
    public void Execute_UsesMetadataBonusWhenPresent()
    {
        var logger = new RecordingLogger<AuditStep>();
        var step = new AuditStep(logger);
        var context = new PipelineContext
        {
            Purchases = 5,
            Multiplier = 2
        };

        var output = step.Execute(context, input =>
        {
            input.NativeResult = 42;
            input.Metadata["bonus.banana"] = 99;
            return input;
        });

        Assert.Same(context, output);
        Assert.Contains(logger.Messages, message => message.Contains("bonusBanana=99", StringComparison.Ordinal));
    }

    private sealed class RecordingLogger<T> : ILogger<T>
    {
        public List<string> Messages { get; } = new();

        public IDisposable BeginScope<TState>(TState state)
            where TState : notnull
        {
            return NullScope.Instance;
        }

        public bool IsEnabled(LogLevel logLevel) => true;

        public void Log<TState>(
            LogLevel logLevel,
            EventId eventId,
            TState state,
            Exception? exception,
            Func<TState, Exception?, string> formatter)
        {
            Messages.Add(formatter(state, exception));
        }

        private sealed class NullScope : IDisposable
        {
            public static readonly NullScope Instance = new();

            public void Dispose()
            {
            }
        }
    }
}
