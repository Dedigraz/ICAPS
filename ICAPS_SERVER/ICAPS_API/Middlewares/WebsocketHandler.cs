namespace ICAPS_API.Middlewares;

using System.Net.WebSockets;
using System.Text;
public abstract class WebSocketHandler
{
    protected ConnectionManager WebSocketConnectionManager { get; set; }
    private ILogger<WebSocketHandler> logger {get;set; }

    public WebSocketHandler(ConnectionManager webSocketConnectionManager, ILogger<WebSocketHandler> logger)
    {
        WebSocketConnectionManager = webSocketConnectionManager;
        this.logger = logger;
    }

    public virtual async Task OnConnected(WebSocket socket)
    {
        logger.LogInformation("Connecting Websocket");

        WebSocketConnectionManager.AddSocket(socket);
    }

    public virtual async Task OnDisconnected(WebSocket socket)
    {
        logger.LogInformation("Disconnecting Websocket");

        await WebSocketConnectionManager.RemoveSocket(WebSocketConnectionManager.GetId(socket));
    }

    public async Task SendMessageAsync(WebSocket socket, string message)
    {
        if(socket.State != WebSocketState.Open)
            return;

        await socket.SendAsync(buffer: new ArraySegment<byte>(array: Encoding.ASCII.GetBytes(message),
                                                                offset: 0,
                                                                count: message.Length),
                                messageType: WebSocketMessageType.Text,
                                endOfMessage: true,
                                cancellationToken: CancellationToken.None);
    }

    public async Task SendMessageAsync(string socketId, string message)
    {
        logger.LogInformation("sending message \t {Message} \t to device with id: {SocketId}", message, socketId);
        await SendMessageAsync(WebSocketConnectionManager.GetSocketById(socketId), message);
    }

    public async Task SendMessageToAllAsync(string message)
    {
                logger.LogInformation("sending message \t {Message} \t to all devices", message);
        foreach(var pair in WebSocketConnectionManager.GetAll())
        {
            if(pair.Value.State == WebSocketState.Open)
            {
                await SendMessageAsync(pair.Value, message);
            }
        }
    }

    public virtual Task ReceiveAsync(WebSocket socket, WebSocketReceiveResult result, byte[] buffer)
    {
        logger.LogInformation("Receiving message from Websocket {id}", WebSocketConnectionManager.GetId(socket));
        return Task.CompletedTask;
    }
}
