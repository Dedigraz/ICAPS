using System.Net;
using System.Net.WebSockets;
using System.Text;

var builder = WebApplication.CreateBuilder(args);
builder.Logging.AddConsole();
// Add services to the container.
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

var app = builder.Build();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.UseHttpsRedirection();

var summaries = new[]
{
    "Freezing", "Bracing", "Chilly", "Cool", "Mild", "Warm", "Balmy", "Hot", "Sweltering", "Scorching"
};

app.MapGet("/weatherforecast", () =>
{
    var forecast = Enumerable.Range(1, 5).Select(index =>
        new WeatherForecast
        (
            DateOnly.FromDateTime(DateTime.Now.AddDays(index)),
            Random.Shared.Next(-20, 55),
            summaries[Random.Shared.Next(summaries.Length)]
        ))
        .ToArray();
    return forecast;
})
.WithName("GetWeatherForecast")
.WithOpenApi();

app.MapPost("/start", () =>
{
    return "START";
})
.WithName("Start")
.WithOpenApi();

app.MapPost("/stop", () =>
{
    return "STOP";
})
.WithName("Stop")
.WithOpenApi();

List<WebSocket> connections = new();

async Task sendMessage(WebSocket socket, string message)
{
    var bytes = Encoding.UTF8.GetBytes(message);
    var arraySegment = new ArraySegment<byte>(bytes, 0, bytes.Length);
    if (socket.State == WebSocketState.Open)
    {
        await socket.SendAsync(arraySegment, WebSocketMessageType.Text, true, CancellationToken.None);
    }
    else if (socket.State == WebSocketState.Closed || socket.State == WebSocketState.Aborted)
    {
        return;
    }
}
app.MapPost("/ws", async (HttpContext context, ILogger<Program> logger) =>
{
    if (context.WebSockets.IsWebSocketRequest)
    {
        using var ws = await context.WebSockets.AcceptWebSocketAsync();
        logger.LogInformation("Initializing WebSocket handshake");
        byte[] buffer = new byte[1024 * 4];
        connections.Add(ws);
        while (ws.State == WebSocketState.Open)
        {
            logger.LogInformation("WebSocket handshake Successfly established");
            var result = await ws.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);

            // if  result == something send response
            await sendMessage(ws,message:DateTime.UtcNow.ToString("HH:mm:ss")+" Seen");
        }
            logger.LogInformation("WebSocket handshake closed.");
    }
    else
    {
        context.Response.StatusCode = (int)HttpStatusCode.BadRequest;
    }
}).WithName("WebSocket receive pos")
.WithOpenApi();


app.UseWebSockets();
await app.RunAsync();

record WeatherForecast(DateOnly Date, int TemperatureC, string? Summary)
{
    public int TemperatureF => 32 + (int)(TemperatureC / 0.5556);
}
