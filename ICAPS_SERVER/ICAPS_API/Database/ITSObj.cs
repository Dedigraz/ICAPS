using Microsoft.EntityFrameworkCore;
using System.ComponentModel.DataAnnotations;
using System.Text.Json.Serialization;

namespace ICAPS_API.Database;

// returns a list of the objects using the link below
//http://127.0.0.1:8000/ret/len=0
// returns the latest object using the link
//http://127.0.0.1:8000/ret/len=1

public class ITSObj
{
    [Key]
    public int id { get; set; }
    [JsonPropertyName("coordinate")]
    public string? location { get; set; }
    public float? vibration { get; set; }
    [JsonPropertyName("x")]
    public float? x_offset { get; set; }
    [JsonPropertyName("y")]
    public float? y_offset { get; set; }
    [JsonPropertyName("w")]
    public float? width { get; set; }
    [JsonPropertyName("h")]
    public float? height { get; set; }
    [JsonPropertyName("type")]
    public string? AnomalyType { get; set; }
    [JsonPropertyName("confidence")]
    public string? confidence { get; set; }
    [JsonPropertyName("datetime")]
    public string? timestamp { get; set; }
    public string? region { get; set; }
    public string? platenumber { get; set; }
    public string? temperature { get; set; }
    public string? flamestatus { get; set; }
}

