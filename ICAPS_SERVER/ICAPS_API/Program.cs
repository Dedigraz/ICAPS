using System.Net;
using System.Net.WebSockets;
using System.Text;
using Microsoft.AspNetCore.Http.HttpResults;
using System.Text.Json;
using System.Text.Json.Serialization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

var builder = WebApplication.CreateBuilder(args);
float[] currentLocation = new float[2] { 9.53742f, 6.45959f }; // Default location

builder.Configuration.AddEnvironmentVariables();
var tomtomClient = new HttpClient
{
    BaseAddress = new Uri("""https://api.tomtom.com/routing/1/""")
};
var apiKey = builder.Configuration["TomTomApiKey"];

if (string.IsNullOrEmpty(apiKey))
{
    throw new InvalidOperationException("TomTom Api key missing, add the api key to environment variables, user secrets or in the appsettings.json to get the app running");
}

builder.Logging.AddConsole();
builder.Services.AddCors(options =>
{
    options.AddDefaultPolicy(
                      policy =>
                      {
                          policy.WithOrigins("*");
                      });
});
builder.Services.ConfigureHttpJsonOptions(e =>
{
    var enumConverter = new JsonStringEnumConverter<AnomalyType>();
    e.SerializerOptions.Converters.Add(enumConverter);
});
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
app.UseRouting();
app.UseCors();
app.UseWebSockets();

app.MapPost("/start", () =>
{
    return "START";
})
.WithName("Start")
.WithDescription("Start Operations ")
.WithOpenApi();

app.MapPost("/stop", () =>
{
    return "STOP";
})
.WithName("Stop")
.WithOpenApi();

app.MapGet("/status", () =>
{
    // Implement logic to get current car status
    return Results.Ok(new { status = "Running", routeCompletion = 42, roadHealth = "Anomaly", speed = 13, connectionStatus = "Connected" });
})
.WithName("GetStatus")
.WithOpenApi();

app.MapGet("/location", () =>
{
    return Results.Ok(currentLocation);
})
.WithName("GetLocation")
.WithOpenApi();

app.MapGet("/anomalies", () =>
{
    // Implement logic to get list of anomalies
    return Results.Ok(Anomaly.GetTestAnomalies());
})
.WithName("GetAnomalies")
.WithOpenApi();

app.MapGet("/route", () =>
{
    // Implement logic to get current route
    return Results.Ok(new[] { new[] { 9.537185, 6.467586 }, new[] { 9.537005, 6.464765 }, /* ... */ });
})
.WithName("GetTestRoute")
.WithOpenApi();

app.MapGet("/routes", async ([FromQuery] float startlat, [FromQuery] float startlng, [FromQuery] float endlat, [FromQuery] float endlng) =>
{
    // Implement logic to get current route
    var routePlanningLocations = $"{startlat},{startlng}:{endlat},{endlng}";
    var contentType = "json";
    var results = await tomtomClient.GetAsync($"calculateRoute/{routePlanningLocations}/{contentType}?key={apiKey}&coordinatePrecision=full");
    return Results.Ok(await results.Content.ReadAsStreamAsync());
})
.WithName("GetRoute")
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

app.MapPost("/ws/command", async (HttpContext context, ILogger<Program> logger) =>
{
    if (context.WebSockets.IsWebSocketRequest)
    {
        using var ws = await context.WebSockets.AcceptWebSocketAsync();
        logger.LogInformation("Command WebSocket connection established");
        byte[] buffer = new byte[1024 * 4];
        while (ws.State == WebSocketState.Open)
        {
            var result = await ws.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
            if (result.MessageType == WebSocketMessageType.Text)
            {
                string command = Encoding.UTF8.GetString(buffer, 0, result.Count);
                // Process command (start, stop, route update)
                logger.LogInformation($"Received command: {command}");
                // Send command to ControllerInterface (implement this)
            }
        }
    }
    else
    {
        context.Response.StatusCode = (int)HttpStatusCode.BadRequest;
    }
}).WithName("WebSocket command")
.Produces(400)
.Produces(200)
.WithOpenApi();

app.MapPost("/ws/update", async (HttpContext context, ILogger<Program> logger) =>
{
    if (context.WebSockets.IsWebSocketRequest)
    {
        using var ws = await context.WebSockets.AcceptWebSocketAsync();
        logger.LogInformation("Update WebSocket connection established");
        byte[] buffer = new byte[1024 * 4];
        while (ws.State == WebSocketState.Open)
        {
            var result = await ws.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
            if (result.MessageType == WebSocketMessageType.Text)
            {
                string update = Encoding.UTF8.GetString(buffer, 0, result.Count);
                logger.LogInformation($"Received update: {update}");

                // Parse the update and store the new location
                var updateData = System.Text.Json.JsonSerializer.Deserialize<Dictionary<string, float>>(update);
                if (updateData != null && updateData.ContainsKey("lat") && updateData.ContainsKey("lng"))
                {
                    currentLocation[0] = updateData["lat"];
                    currentLocation[1] = updateData["lng"];
                }
            }
        }
    }
    else
    {
        context.Response.StatusCode = (int)HttpStatusCode.BadRequest;
    }
}).WithName("WebSocket update")
.Produces(400)
.Produces(200)
.WithOpenApi();

await app.RunAsync();


record Anomaly(float lat, float lng, float offset, float width, float height, AnomalyType AnomalyType)
{
    public static Anomaly[] GetTestAnomalies()
    {
        float ft = 0.243f;
        var roadWidth = 5 * ft;
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
    FIRE_OR_COMBUSTION_ZONE,
    VIBRATIONS
}

