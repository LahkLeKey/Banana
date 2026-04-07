using CInteropSharp.Api.Middleware;
using CInteropSharp.Api.NativeInterop;
using CInteropSharp.Api.Services;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddControllers();
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.AddScoped<INativePointsClient, NativePointsClient>();
builder.Services.AddScoped<IPointsService, PointsService>();

var app = builder.Build();

NativeLibraryResolver.EnsureConfigured(builder.Configuration, app.Logger);

app.UseMiddleware<ErrorHandlingMiddleware>();

app.UseSwagger();
app.UseSwaggerUI();

app.MapControllers();

app.Run();

public partial class Program;
