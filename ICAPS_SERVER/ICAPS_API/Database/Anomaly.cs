using System.Text.Json.Serialization;

namespace ICAPS_API.Database;


public record Anomaly(float lat, float lng, float offset, float width, float height, AnomalyType AnomalyType)
{
    [JsonIgnore]
    public int Id { get; init; }

    public static Anomaly[] GetTestAnomalies()
    {
        float ft = 0.243f;
        var roadWidth = 5 * ft;
        Anomaly[] anomalies =
        [
            new( 9.53709f, 6.46616f, 0, 0.5f * roadWidth, 2.5f * ft, AnomalyType.CRACK ),
            new( 9.53708f, 6.46587f, 0, roadWidth, 3.3f * ft, AnomalyType.SPEEDBUMP ),
            new( 9.53704f, 6.46561f, 0, roadWidth, 2.75f * ft, AnomalyType.SPEEDBUMP ),
            new( 9.53703f, 6.46549f, 0, roadWidth, 3.0f * ft, AnomalyType.SPEEDBUMP ),
            new( 9.53700f, 6.46523f, 0, roadWidth, 12 * ft , AnomalyType.CRACK ),
            new( 9.53696f, 6.46452f, 3.5f * ft,  2.8f *ft,3*ft , AnomalyType.POTHOLE ),
            new( 9.53697f, 6.46437f, 1.875f * ft,  8*ft, 4*ft, AnomalyType.POTHOLE ),
            new( 9.53690f, 6.46370f, 6.5f*ft, 2.5f*ft, 4*ft, AnomalyType.POTHOLE ),
            new( 9.53775f, 6.45959f, 4*ft, 5.25f*ft, 8.0f*ft, AnomalyType.POTHOLE ),
            new( 9.53779f, 6.45947f, 1.5f*ft, 14*ft, 5.5f*ft, AnomalyType.POTHOLE ),
            new( 9.53744f, 6.45620f, 8*ft, 1*ft, 1.5f*ft, AnomalyType.POTHOLE ),
            new( 9.53734f, 6.45604f, -7.0f * ft, 2.5f*ft, 3*ft, AnomalyType.CRACK ),
            new( 9.53732f, 6.45602f, -4.3f *ft, 6*ft, 5*ft, AnomalyType.CRACK ),
            new( 9.53705f, 6.45567f, 0, 7*ft, 3*ft, AnomalyType.POTHOLE ),
            new( 9.53647f, 6.45452f, 0, roadWidth, 2*ft, AnomalyType.POTHOLE ),
            new( 9.53625f, 6.45444f, 7*ft, 5*ft, ft, AnomalyType.POTHOLE ),
            new( 9.53629f, 6.45477f, 0, 1.5f*ft, roadWidth, AnomalyType.POTHOLE ),
            new( 9.53721f, 6.46071f, -5.5f*ft, 5*ft, 3*ft, AnomalyType.POTHOLE ),
            new( 9.53680f, 6.46428f, 0, roadWidth, 3.5f*ft, AnomalyType.SPEEDBUMP ),
            new( 9.53691f, 6.46473f, 3.5f*ft, 9*ft, 3*ft, AnomalyType.SPEEDBUMP ),
            new( 9.53695f, 6.46519f, 0, roadWidth, 3*ft, AnomalyType.SPEEDBUMP ),
            new( 9.53703f, 6.46720f, 0,roadWidth ,7.3f*ft, AnomalyType.SPEEDBUMP ),
        ];
        return anomalies;
    }
}

[JsonConverter(typeof(JsonStringEnumConverter))]
public enum AnomalyType
{
    CRACK = 0,
    POTHOLE,
    SPEEDBUMP,
    FIRE,
    VIBRATIONS
}
