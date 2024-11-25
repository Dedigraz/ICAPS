using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace ICAPS_API.Migrations
{
    /// <inheritdoc />
    public partial class ITSobject : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "ITSReturns",
                columns: table => new
                {
                    id = table.Column<int>(type: "INTEGER", nullable: false)
                        .Annotation("Sqlite:Autoincrement", true),
                    location = table.Column<string>(type: "TEXT", nullable: false),
                    vibration = table.Column<float>(type: "REAL", nullable: false),
                    x_offset = table.Column<float>(type: "REAL", nullable: false),
                    y_offset = table.Column<float>(type: "REAL", nullable: false),
                    width = table.Column<float>(type: "REAL", nullable: false),
                    height = table.Column<float>(type: "REAL", nullable: false),
                    AnomalyType = table.Column<string>(type: "TEXT", nullable: false),
                    confidence = table.Column<string>(type: "TEXT", nullable: false),
                    timestamp = table.Column<string>(type: "TEXT", nullable: false),
                    region = table.Column<string>(type: "TEXT", nullable: false),
                    platenumber = table.Column<string>(type: "TEXT", nullable: false),
                    temperature = table.Column<string>(type: "TEXT", nullable: false),
                    flamestatus = table.Column<string>(type: "TEXT", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_ITSReturns", x => x.id);
                });
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "ITSReturns");
        }
    }
}
