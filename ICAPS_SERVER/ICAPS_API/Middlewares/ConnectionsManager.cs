namespace ICAPS_API.Middlewares;
using System.Collections.Concurrent;
using System.Net.WebSockets;

public class ConnectionManager
{
    private ConcurrentDictionary<string, WebSocket> _sockets = new ConcurrentDictionary<string, WebSocket>();

    public WebSocket GetSocketById(string id)
    {
        return _sockets.FirstOrDefault(p => p.Key == id).Value;
    }

    public ConcurrentDictionary<string, WebSocket> GetAll()
    {
        return _sockets;
    }

    public string GetId(WebSocket socket)
    {
        return _sockets.FirstOrDefault(p => p.Value == socket).Key;
    }
    public void AddSocket(WebSocket socket)
    {
        _sockets.TryAdd(CreateConnectionId(), socket);
    }

    public async Task RemoveSocket(string id)
    {
        WebSocket socket;
        _sockets.TryRemove(id, out socket);

        await socket.CloseAsync(closeStatus: WebSocketCloseStatus.NormalClosure,
                                statusDescription: "Closed by the ConnectionManager",
                                cancellationToken: CancellationToken.None);
    }

    private string CreateConnectionId()
    {
        return Guid.NewGuid().ToString();
    }
}