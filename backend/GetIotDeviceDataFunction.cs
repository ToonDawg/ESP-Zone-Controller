using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging;
using System.Threading.Tasks;
using Microsoft.Azure.Devices;
using System;

public static class GetIotDeviceDataFunction
{
    private static readonly string iotHubConnectionString = Environment.GetEnvironmentVariable("IoTHubConnectionString");
    private static ServiceClient serviceClient = ServiceClient.CreateFromConnectionString(iotHubConnectionString);

    [FunctionName("GetIotDeviceData")]
    public static async Task<IActionResult> Run(
        [HttpTrigger(AuthorizationLevel.Function, "get", Route = null)] HttpRequest req,
        ILogger log)
    {
        log.LogInformation("C# HTTP trigger function processed a request.");

        string deviceId = req.Query["deviceId"];

        if (string.IsNullOrWhiteSpace(deviceId))
        {
            return new BadRequestObjectResult("Please provide a deviceId.");
        }

        // Command the device to send telemetry using a C2D message
        await SendTelemetryCommandToDevice(deviceId);

        // Simulating a wait for the device to send telemetry and for the telemetry to be processed.
        var deviceData = await WaitForTelemetryData(deviceId);

        return new OkObjectResult(deviceData);
    }

    private static async Task SendTelemetryCommandToDevice(string deviceId)
    {
        var commandMessage = new Message(Encoding.ASCII.GetBytes("SendTelemetry"));
        await serviceClient.SendAsync(deviceId, commandMessage);
    }

    private static async Task<string> WaitForTelemetryData(string deviceId)
    {
        DateTime timeout = DateTime.UtcNow.AddSeconds(30); // Set a timeout, e.g., 30 seconds

        while (DateTime.UtcNow < timeout)
        {
            // Check the data store for telemetry data from the device
            var telemetryData = RetrieveTelemetryFromDataStore(deviceId);

            if (telemetryData != null)
            {
                return telemetryData;
            }

            await Task.Delay(TimeSpan.FromSeconds(1)); // Wait for a short time before checking again
        }

        return null; // Return null or some default value if telemetry data isn't received in time
    }

}
