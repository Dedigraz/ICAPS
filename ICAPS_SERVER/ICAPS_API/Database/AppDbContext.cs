namespace ICAPS_API.Database;
using Microsoft.EntityFrameworkCore;

public class AppDbContext : DbContext{
    public DbSet<Anomaly> Anomalies { get; set; }           
    public AppDbContext()
    {
        
    }

    public AppDbContext(DbContextOptions<AppDbContext> options) : base(options)
    {
        
    }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);
        
    }
}