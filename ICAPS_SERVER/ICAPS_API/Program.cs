using System.Net;
using System.Net.WebSockets;
using System.IO;
using System.Text;
using Microsoft.AspNetCore.Http.HttpResults;
using System.Text.Json;
using System.Text.Json.Serialization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using ICAPS_API.Middlewares;
using ICAPS_API.Database;
using Microsoft.EntityFrameworkCore;
using ICAPS_API.Events;
using Microsoft.AspNetCore.Builder;

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
builder.Services.AddLogging();
builder.Services.AddHttpLogging((options) =>
{
    options.LoggingFields = Microsoft.AspNetCore.HttpLogging.HttpLoggingFields.All;
});
builder.Logging.AddConsole();

builder.Services.AddSingleton<LogEventDb>();
builder.Services.AddCors(options =>
{
    options.AddDefaultPolicy(
                      policy =>
                      {
                          policy.WithOrigins("*");
                      });
});

builder.Services.AddDbContext<AppDbContext>(options =>
{
    options.UseSqlite("Data Source=Datastore.db");
    options.UseSeeding(SeedDb);
    options.UseAsyncSeeding(SeedDbAsync);
});



builder.Services.ConfigureHttpJsonOptions(e =>
{
    var enumConverter = new JsonStringEnumConverter<AnomalyType>();
    e.SerializerOptions.Converters.Add(enumConverter);
});
builder.Services.AddControllers();
builder.Services.AddWebSocketManager();
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

//app.UseHttpsRedirection();
app.UseRouting();
app.UseCors();
app.UseWebSockets(new WebSocketOptions
{
    KeepAliveInterval = TimeSpan.FromSeconds(120),
});



app.MapPost("/start", async(MessageHandler messageHandler) =>
{
    //LogEventDb.Append(new StartTest
    //{

    //});
    await messageHandler.SendMessageToAllAsync("Start");
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

app.MapGet("/anomalies", (AppDbContext context) =>
{
    // Implement logic to get list of anomalies
    return Results.Ok(context.Anomalies.ToList());
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


app.Map("/",() =>
{
    return Results.Ok("~/index.html");
}).WithName("WebsocketEndpoint")
.WithOpenApi()
.WithHttpLogging(Microsoft.AspNetCore.HttpLogging.HttpLoggingFields.All);


app.UseEndpoints((e) =>
{
    e.MapControllers();
});

using (var scope = app.Services.CreateScope())
{
    var messageHandler = scope.ServiceProvider.GetRequiredService<MessageHandler>();
    app.MapWebSocketManager("", messageHandler);
}


using (var scope = app.Services.CreateScope())
{
    var db = scope.ServiceProvider.GetRequiredService<AppDbContext>();
    await db.Database.MigrateAsync();
    db.Database.EnsureCreated();
}



await app.RunAsync();


async Task SeedDbAsync(DbContext context, bool storeManagementDone, CancellationToken token)
{
    var db = (AppDbContext)context;
    if (db.Anomalies.Count() == 0) return;
    var anomalies = Anomaly.GetTestAnomalies();
    if (storeManagementDone)
    { }
    db.Anomalies.AddRange(anomalies);
    await db.SaveChangesAsync(token);

}

void SeedDb(DbContext context, bool storeManagementDone)
{
    var db = (AppDbContext)context;
    var anomalies = Anomaly.GetTestAnomalies();
    if (storeManagementDone)
    { }
    db.Anomalies.AddRange(anomalies);
    db.SaveChanges();

}