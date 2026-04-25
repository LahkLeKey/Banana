using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;

var builder = WebApplication.CreateBuilder(args);

// Spec 007: single typed PipelineContext + single interop seam.
builder.Services.AddSingleton<INativeBananaClient, NativeBananaClient>();
builder.Services.AddScoped<PipelineContext>();
builder.Services.AddControllers();
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

var app = builder.Build();
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.MapGet("/health", () => Results.Ok(new { status = "ok" }));
app.MapControllers();

app.Run();

public partial class Program;
