using Banana.Api.Middleware;
using Banana.Api.DataAccess;
using Banana.Api.NativeInterop;
using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Steps;
using Banana.Api.Services;
using Microsoft.Extensions.Options;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddControllers();
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.Configure<DbAccessOptions>(builder.Configuration.GetSection(DbAccessOptions.SectionName));
builder.Services.AddScoped<NativeDalDbDataAccessClient>();
builder.Services.AddScoped<ManagedNpgsqlDataAccessClient>();
builder.Services.AddScoped<IDataAccessPipelineClient>(static serviceProvider =>
{
	var options = serviceProvider.GetRequiredService<IOptions<DbAccessOptions>>().Value;
	return options.Mode switch
	{
		DbAccessMode.NativeDal => serviceProvider.GetRequiredService<NativeDalDbDataAccessClient>(),
		DbAccessMode.ManagedNpgsql => serviceProvider.GetRequiredService<ManagedNpgsqlDataAccessClient>(),
		_ => serviceProvider.GetRequiredService<NativeDalDbDataAccessClient>()
	};
});

builder.Services.AddScoped<INativeBananaClient, NativeBananaClient>();
builder.Services.AddScoped<IBatchService, BatchService>();
builder.Services.AddScoped<PipelineExecutor<PipelineContext>>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, ValidationStep>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, DatabaseAccessStep>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, NativeCalculationStep>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, PostProcessingStep>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, AuditStep>();
builder.Services.AddScoped<IBananaService, BananaService>();
builder.Services.AddScoped<IRipenessService, RipenessService>();
builder.Services.AddScoped<INotBananaService, NotBananaService>();

var app = builder.Build();

NativeLibraryResolver.EnsureConfigured(builder.Configuration, app.Logger);

app.UseMiddleware<ErrorHandlingMiddleware>();

app.UseSwagger();
app.UseSwaggerUI();

app.MapGet("/health", () => Results.Ok(new { status = "ok" }));
app.MapControllers();

app.Run();

public partial class Program;
