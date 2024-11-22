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

    public LogController(LogEventDb logEventDb, ILogger<LogController> logger, MessageHandler messageHandler)
    {
        _logEventDb = logEventDb;
        _logger = logger;
        _messageHandler = messageHandler;
    }

    [HttpGet("start")]
    public async Task<Results<Ok<string>, NotFound>> StartTest()
    {
        var startTest = new StartTest{
            CreatedAtTimestamp = DateTime.UtcNow,
        };
        _logEventDb.Append(startTest);
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
}
