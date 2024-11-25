using ICAPS_API.Events;

public class LogEventDb{
    public readonly Dictionary<Guid, SortedList<DateTime, Event>> logEvents = new();
    public 

    public void Append(Event @event){
        var stream  = logEvents!.GetValueOrDefault(@event.streamId, null);
        if(stream is null){
            logEvents[@event.streamId] = new SortedList<DateTime, Event>();
        }
        @event.CreatedAtTimestamp = DateTime.UtcNow;
        logEvents[@event.streamId].Add(@event.CreatedAtTimestamp, @event);

    }



    //public event OnChanged(EventArgs e)
    //    {}
}