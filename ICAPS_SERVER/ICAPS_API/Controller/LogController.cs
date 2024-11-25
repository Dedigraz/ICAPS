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

    private readonly AppDbContext _dbContext;

    public LogController(LogEventDb logEventDb, ILogger<LogController> logger, MessageHandler messageHandler, IHttpClientFactory httpClientFactory, AppDbContext dbContext)
    {
        _logEventDb = logEventDb;
        _logger = logger;
        _messageHandler = messageHandler;
        itsClient = httpClientFactory.CreateClient("itsApi");
        _dbContext = dbContext;
    }

    [HttpGet("start")]
    public async Task<Results<Ok<string>, NotFound>> StartTest()
    {
        var startTest = new StartTest{
            CreatedAtTimestamp = DateTime.UtcNow,
        };
        _logEventDb.Append(startTest);
        string result =  await (await itsClient.GetAsync("/ret/len=0")).Content.ReadAsStringAsync();
        _dbContext.ITSReturns.AddRange(JsonSerializer.Deserialize<IEnumerable<ITSObj>>(result));
        await _dbContext.SaveChangesAsync();
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
        
        return Ok(_dbContext.ITSReturns.ToList());
    }
}
