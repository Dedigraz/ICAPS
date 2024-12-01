using ICAPS_API.Database;
using ICAPS_API.Events;
using Microsoft.AspNetCore.Http.HttpResults;
using Microsoft.AspNetCore.Mvc;
using System.Text.Json;

namespace ICAPS_API.Controller;

[Route("api/log")]
public class LogController : ControllerBase
{
    private readonly LogEventDb _logEventDb;
    private readonly ILogger<LogController> _logger;
    private readonly MessageHandler _messageHandler;
    private readonly HttpClient itsClient;

    // private readonly AppDbContext _dbContext;

    public LogController(LogEventDb logEventDb, ILogger<LogController> logger, MessageHandler messageHandler, IHttpClientFactory httpClientFactory, AppDbContext dbContext)
    {
        _logEventDb = logEventDb;
        _logger = logger;
        _messageHandler = messageHandler;
        itsClient = httpClientFactory.CreateClient("itsApi");
        // _dbContext = dbContext;
    }

    [HttpGet("start")]
    public async Task<Results<Ok<string>, NotFound>> StartTest()
    {
        var startTest = new StartTest{
            CreatedAtTimestamp = DateTime.UtcNow,
        };
        _logEventDb.Append(startTest);

        var result =  await (await itsClient.GetAsync("/ret/len=0")).Content.ReadAsStreamAsync();
        // _dbContext.ITSReturns.AddRange(JsonSerializer.Deserialize<IEnumerable<ITSObj>>(result));
        // await _dbContext.SaveChangesAsync();
        _logEventDb.ExternalData = (await JsonSerializer.DeserializeAsync<List<ITSObj>>(result))!;
        var message = JsonSerializer.Serialize(startTest);
        await _messageHandler.SendMessageToAllAsync(message);
        return TypedResults.Ok("Start Event");
    }

    [HttpPost]
    public Results<Ok<string>, NotFound> Post([FromBody] Event @event)
    {
        if (@event is null)
        {
            return TypedResults.NotFound();
        }
        _logEventDb.Append(@event);
        return TypedResults.Ok("hey");
    }

    
    [HttpGet("stop")]
    public async Task<Results<Ok<string>, NoContent>> StopTest()
    {
        // get the testId from the last start event
        var testId = _logEventDb.logEvents.Values.Last().Last().Value.streamId;
        if(testId == Guid.Empty)
        {
            return TypedResults.NoContent();
        }
        var stopTest = new StopTest
        {
            testId = testId,
            StoppedAtTimestamp = DateTime.UtcNow,
        };
        _logEventDb.Append(stopTest);
        var message = JsonSerializer.Serialize(stopTest);
        await _messageHandler.SendMessageToAllAsync(message);
        return TypedResults.Ok("Stop Event");
    }

    [HttpGet("getAnomalies")]
    public IActionResult GetAnomalies()
    {
        if (_logEventDb.ExternalData is [])
        {
            return NotFound("No data available, are you sure you started the test?");
        }
        return Ok(_logEventDb.ExternalData);
    }

    [ApiExplorerSettings(IgnoreApi = true)]
    public void FromExernal()
    {
        List<Anomaly> anomalies = new();
        foreach (var obj in _logEventDb.ExternalData)
        {
            var lat = float.Parse( obj.location.Split(' ')[0].Split('=')[1]);
            var lng = float.Parse( obj.location.Split(' ')[1].Split('=')[1]);
            var vibrationIntensity = 0;
            float offset = obj.x_offset.GetValueOrDefault();
            float width = obj.width.GetValueOrDefault();
            float height = obj.height.GetValueOrDefault();
            AnomalyType anomalyType;
             switch (obj.AnomalyType.ToUpper())
            {
                case "CRACK":
                    anomalyType = AnomalyType.CRACK;
                    break;
                case "POTHOLE":
                    anomalyType = AnomalyType.POTHOLE;
                    break;
                case "SPEEDBUMP":
                    anomalyType =  AnomalyType.SPEEDBUMP;
                    break;
                default:
                    if(int.Parse(obj.temperature.Split('c')[0]) > 50)
                    {
                        anomalyType = AnomalyType.FIRE;
                    }
                    else
                    {
                        anomalyType = AnomalyType.VIBRATIONS;
                        vibrationIntensity = 0;
                    }
                    break;
            };
            anomalies.Add(new Anomaly(lat,lng, offset,width,height,vibrationIntensity,anomalyType));
        }
    }
}
