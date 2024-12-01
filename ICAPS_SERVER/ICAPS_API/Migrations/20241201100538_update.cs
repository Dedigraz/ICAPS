using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace ICAPS_API.Migrations
{
    /// <inheritdoc />
    public partial class update : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<float>(
                name: "vibrationIntensity",
                table: "Anomalies",
                type: "REAL",
                nullable: false,
                defaultValue: 0f);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "vibrationIntensity",
                table: "Anomalies");
        }
    }
}
