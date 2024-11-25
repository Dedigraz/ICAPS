using ICAPS_API.Database;
using System.Text.Json.Serialization;

namespace ICAPS_API.Events;

public abstract class Event{
    public abstract Guid streamId {get;}
    public DateTime CreatedAtTimestamp {get;set;}
}

public class StartTest : Event{

    [JsonIgnore]
    public Guid testId {get;init; } = Guid.NewGuid();    
    public override Guid streamId => testId;
}

public class StopTest : Event{
    public Guid testId {get;set;}
    public DateTime StoppedAtTimestamp {get;set;}

    public override Guid streamId => testId;
}


public class CarLocatonUpdated : Event{
    public Guid carId {get;set;}
    public float[] location {get;set;}
    public override Guid streamId => carId;
}

public class ParametersSet : Event{
    public Guid carId {get;set;}
    public float[] parameters {get;set;}
    public override Guid streamId => carId;
}

public class AnomalyFound : Event
{
    public Guid carId { get; set; }
    public AnomalyType anomalyType { get; set; }

    public override Guid streamId => carId;
}
//LocationUpdatedEvent
//ParametersSetEvent
//AnomalyFoundEvent
public class TransferObject
{
    public float lat { get; set; }
    public float lng { get; set; }
    public float vibration { get; set; }
    [JsonPropertyName("x")]
    public float offset { get; set; }
    public float width { get; set; }
    public float height { get; set; }
    public AnomalyType AnomalyType { get; set; }
}

