using CInteropSharp.Api.Middleware;
using CInteropSharp.Api.DataAccess;
using CInteropSharp.Api.NativeInterop;
using CInteropSharp.Api.Pipeline;
using CInteropSharp.Api.Pipeline.Steps;
using CInteropSharp.Api.Services;
using Microsoft.Extensions.Options;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddControllers();
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.Configure<DbAccessOptions>(builder.Configuration.GetSection(DbAccessOptions.SectionName));
builder.Services.AddScoped<LegacyNativeDbDataAccessClient>();
builder.Services.AddScoped<ManagedNpgsqlDataAccessClient>();
builder.Services.AddScoped<IDataAccessPipelineClient>(static serviceProvider =>
{
	var options = serviceProvider.GetRequiredService<IOptions<DbAccessOptions>>().Value;
	return options.Mode switch
	{
		DbAccessMode.LegacyNative => serviceProvider.GetRequiredService<LegacyNativeDbDataAccessClient>(),
		DbAccessMode.ManagedNpgsql => serviceProvider.GetRequiredService<ManagedNpgsqlDataAccessClient>(),
		_ => serviceProvider.GetRequiredService<LegacyNativeDbDataAccessClient>()
	};
});

builder.Services.AddScoped<INativePointsClient, NativePointsClient>();
builder.Services.AddScoped<PipelineExecutor<PipelineContext>>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, ValidationStep>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, DatabaseAccessStep>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, NativeCalculationStep>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, PostProcessingStep>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, AuditStep>();
builder.Services.AddScoped<IPointsService, PointsService>();

var app = builder.Build();

NativeLibraryResolver.EnsureConfigured(builder.Configuration, app.Logger);

app.UseMiddleware<ErrorHandlingMiddleware>();

app.UseSwagger();
app.UseSwaggerUI();

app.MapControllers();

app.Run();

public partial class Program;
