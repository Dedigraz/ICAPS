using System.Net;
using System.Net.WebSockets;
using System.Text;
using Microsoft.AspNetCore.Http.HttpResults;

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
            await sendMessage(ws, message: DateTime.UtcNow.ToString("HH:mm:ss") + " Seen");
        }
        logger.LogInformation("WebSocket handshake closed.");
    }
    else
    {
        context.Response.StatusCode = (int)HttpStatusCode.BadRequest;
    }
}).WithName("WebSocket receive pos")
.Produces(400)
.Produces(200)
.WithOpenApi();


app.UseWebSockets();
await app.RunAsync();

record WeatherForecast(DateOnly Date, int TemperatureC, string? Summary)
{
    public int TemperatureF => 32 + (int)(TemperatureC / 0.5556);
}

record Anomaly(float lat, float lng, float offset, float width, float height, AnomalyType AnomalyType)
{
    public Anomaly[] GetTestAnomalies(){
        float ft = 0.243f;
        var roadWidth = 5*ft;
        Anomaly[] anomalies =
        [
            new( 9.53709f, 6.46616f, 0, 0.5f * roadWidth, 2.5f * ft, AnomalyType.CRACK ),
            new( 9.53708f, 6.46587f, 0, roadWidth, 3.3f * ft, AnomalyType.SPEEDBUMP ),
            new( 9.53704f, 6.46561f, 0, roadWidth, 2.75f * ft, AnomalyType.SPEEDBUMP ),
            new( 9.53703f, 6.46549f, 0, roadWidth, 3.0f * ft, AnomalyType.SPEEDBUMP ),
            new( 9.53700f, 6.46523f, 0, roadWidth, 12 * ft , AnomalyType.CRACK ),
            new( 9.53696f, 6.46452f, 3.5f * ft,  2.8f *ft,3*ft , AnomalyType.POTHOLE ),
            new( 9.53697f, 6.46437f, 1.875f * ft,  8*ft, 4*ft, AnomalyType.POTHOLE ),
            new( 9.53690f, 6.46370f, 6.5f*ft, 2.5f*ft, 4*ft, AnomalyType.POTHOLE ),
            new( 9.53775f, 6.45959f, 4*ft, 5.25f*ft, 8.0f*ft, AnomalyType.POTHOLE ),
            new( 9.53779f, 6.45947f, 1.5f*ft, 14*ft, 5.5f*ft, AnomalyType.POTHOLE ),
            new( 9.53744f, 6.45620f, 8*ft, 1*ft, 1.5f*ft, AnomalyType.POTHOLE ),
            new( 9.53734f, 6.45604f, -7.0f * ft, 2.5f*ft, 3*ft, AnomalyType.CRACK ),
            new( 9.53732f, 6.45602f, -4.3f *ft, 6*ft, 5*ft, AnomalyType.CRACK ),
            new( 9.53705f, 6.45567f, 0, 7*ft, 3*ft, AnomalyType.POTHOLE ),
            new( 9.53647f, 6.45452f, 0, roadWidth, 2*ft, AnomalyType.POTHOLE ),
            new( 9.53625f, 6.45444f, 7*ft, 5*ft, ft, AnomalyType.POTHOLE ),
            new( 9.53629f, 6.45477f, 0, 1.5f*ft, roadWidth, AnomalyType.POTHOLE ),
            new( 9.53721f, 6.46071f, -5.5f*ft, 5*ft, 3*ft, AnomalyType.POTHOLE ),
            new( 9.53680f, 6.46428f, 0, roadWidth, 3.5f*ft, AnomalyType.SPEEDBUMP ),
            new( 9.53691f, 6.46473f, 3.5f*ft, 9*ft, 3*ft, AnomalyType.SPEEDBUMP ),
            new( 9.53695f, 6.46519f, 0, roadWidth, 3*ft, AnomalyType.SPEEDBUMP ),
            new( 9.53703f, 6.46720f, 0,roadWidth ,7.3f*ft, AnomalyType.SPEEDBUMP ),
        ];
        
        return anomalies;
    }
}

enum AnomalyType
{
    POTHOLE,
    SPEEDBUMP,
    COLLISION,
    ACCIDENT_SCENE,
    CRACK,
    MANHOLE,
    FIRE_OR_COMBUSTION_ZONE
}