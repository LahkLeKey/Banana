using Banana.Api.NativeInterop;
using Banana.Api.Pipeline.Mapping;
using Banana.Api.Pipeline;
using Banana.Api.Pipeline.Steps;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.IdentityModel.Tokens;
using System.Text;

var builder = WebApplication.CreateBuilder(args);
const string FrontendCorsPolicy = "BananaFrontend";

// Feature 063 — JWT auth (HS256). Secret from BANANA_JWT_SECRET env var.
// When the env var is absent the server starts without authentication enabled
// (dev/test convenience). In production this must be set.
var jwtSecret = builder.Configuration["BANANA_JWT_SECRET"]
    ?? Environment.GetEnvironmentVariable("BANANA_JWT_SECRET");

if (!string.IsNullOrWhiteSpace(jwtSecret))
{
    builder.Services
        .AddAuthentication(JwtBearerDefaults.AuthenticationScheme)
        .AddJwtBearer(opts =>
        {
            opts.TokenValidationParameters = new TokenValidationParameters
            {
                ValidateIssuerSigningKey = true,
                IssuerSigningKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(jwtSecret)),
                ValidateIssuer = false,
                ValidateAudience = false,
                ClockSkew = TimeSpan.FromSeconds(30),
            };
        });
    builder.Services.AddAuthorization(opts =>
    {
        opts.AddPolicy("AdminOnly",   p => p.RequireClaim("role", "admin"));
        opts.AddPolicy("OperatorUp",  p => p.RequireClaim("role", "admin", "operator"));
        opts.AddPolicy("ViewerUp",    p => p.RequireClaim("role", "admin", "operator", "viewer"));
    });
}
const string FrontendCorsPolicy = "BananaFrontend";

// Spec 007: single typed PipelineContext + single interop seam.
builder.Services.AddSingleton<INativeBananaClient, NativeBananaClient>();
builder.Services.AddScoped<PipelineContext>();
builder.Services.AddSingleton<INativeJsonMapper, NativeJsonMapper>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, InputValidationStep>();
// Slice 014 -- ensemble gated cascade. Order is enforced via IPipelineStep.Order:
// gating (100) -> escalation (200) -> calibration (300).
builder.Services.AddScoped<IPipelineStep<PipelineContext>, EnsembleGatingStep>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, EnsembleEscalationStep>();
builder.Services.AddScoped<IPipelineStep<PipelineContext>, EnsembleCalibrationStep>();
builder.Services.AddScoped<EnsembleGatingStep>();
builder.Services.AddScoped<EnsembleEscalationStep>();
builder.Services.AddScoped<EnsembleCalibrationStep>();
builder.Services.AddScoped<PipelineRunner<PipelineContext>>();
builder.Services.AddCors(options =>
{
    options.AddPolicy(FrontendCorsPolicy, policy =>
    {
        policy
            .WithOrigins(
                "http://localhost:5173",
                "http://127.0.0.1:5173")
            .AllowAnyHeader()
            .AllowAnyMethod();
    });
});
builder.Services.AddControllers();
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

var app = builder.Build();
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.UseCors(FrontendCorsPolicy);
if (!string.IsNullOrWhiteSpace(jwtSecret))
{
    app.UseAuthentication();
    app.UseAuthorization();
}

// Feature 100 — Security headers (CSP + hardening)
app.Use(async (context, next) =>
{
    var headers = context.Response.Headers;
    headers["X-Content-Type-Options"] = "nosniff";
    headers["X-Frame-Options"] = "DENY";
    headers["X-XSS-Protection"] = "0"; // modern browsers rely on CSP
    headers["Referrer-Policy"] = "strict-origin-when-cross-origin";
    headers["Permissions-Policy"] = "camera=(), microphone=(), geolocation=()";
    headers["Content-Security-Policy"] =
        "default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'; " +
        "img-src 'self' data:; connect-src 'self'; font-src 'self'; object-src 'none'; " +
        "upgrade-insecure-requests";
    await next();
});

app.MapGet("/health", () => Results.Ok(new { status = "ok" }));
app.MapControllers();

app.Run();

public partial class Program;
