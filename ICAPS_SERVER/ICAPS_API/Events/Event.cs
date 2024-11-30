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
    public float[] newLocation {get;set;}
    public override Guid streamId => carId;
}

public class TestParametersSet : Event{
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