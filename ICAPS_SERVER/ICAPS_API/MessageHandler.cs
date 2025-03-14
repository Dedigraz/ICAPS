using System.Net.WebSockets;
using System.Text;
using ICAPS_API.Middlewares;

public class MessageHandler : WebSocketHandler
{
    private readonly ILogger<MessageHandler> logger;
    public MessageHandler(ConnectionManager webSocketConnectionManager, ILogger<MessageHandler> logger) : base(webSocketConnectionManager, logger)
    {
    }

    public override async Task OnConnected(WebSocket socket)
    {
        await base.OnConnected(socket);

        var socketId = WebSocketConnectionManager.GetId(socket);
        await SendMessageToAllAsync($"{socketId} is now connected");
    }

    public override async Task ReceiveAsync(WebSocket socket, WebSocketReceiveResult result, byte[] buffer)
    {
        await base.ReceiveAsync(socket, result, buffer);

        var socketId = WebSocketConnectionManager.GetId(socket);
        var message = $"{socketId} said: {Encoding.UTF8.GetString(buffer, 0, result.Count)}";

        //await SendMessageToAllAsync(message);
    }
}