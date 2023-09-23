using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;
using System.Threading.Tasks;
using Microsoft.Azure.Devices;
using System;
using System.Linq;

public static class GetIotDevicesDataFunction
{
    private static readonly string iotHubConnectionString = Environment.GetEnvironmentVariable("IoTHubConnectionString");
    private static readonly ServiceClient serviceClient = CreateServiceClient();

    [FunctionName("GetIotDevicesDataFunction")]
    public static async Task<IActionResult> Run(
        [HttpTrigger(AuthorizationLevel.Function, "get", Route = null)] HttpRequest req,
        ILogger log)
    {
        if (string.IsNullOrWhiteSpace(iotHubConnectionString))
        {
            log.LogError("IoTHubConnectionString environment variable is not set.");
            return new StatusCodeResult(StatusCodes.Status500InternalServerError);
        }

        log.LogInformation("Processing request to fetch IoT devices data.");

        string deviceIdsQueryString = req.Query["deviceIds"];
        if (string.IsNullOrWhiteSpace(deviceIdsQueryString))
        {
            return new BadRequestObjectResult("Please provide a list of deviceIds.");
        }

        var deviceIds = deviceIdsQueryString.Split(',');
        var tasks = deviceIds.Select(deviceId => FetchDeviceData(deviceId, log));
        var results = await Task.WhenAll(tasks);

        return new OkObjectResult(results);
    }

    private static ServiceClient CreateServiceClient()
    {
        if (string.IsNullOrWhiteSpace(iotHubConnectionString))
        {
            return null;
        }

        return ServiceClient.CreateFromConnectionString(iotHubConnectionString);
    }

    private static async Task<object> FetchDeviceData(string deviceId, ILogger log)
    {
        var response = await InvokeDirectMethod(deviceId, "GetDataMethod");
        if (response.Status != 200)
        {
            log.LogWarning($"Device {deviceId} returned status: {response.Status}. Message: {response.GetPayloadAsJson()}");
            return new { deviceId, error = $"Device returned status: {response.Status}. Message: {response.GetPayloadAsJson()}" };
        }

        var payload = JsonConvert.DeserializeObject<object>(response.GetPayloadAsJson());
        return payload;
    }

    private static async Task<CloudToDeviceMethodResult> InvokeDirectMethod(string deviceId, string methodName)
    {
        var methodInvocation = new CloudToDeviceMethod(methodName) { ResponseTimeout = TimeSpan.FromSeconds(30) };
        return await serviceClient.InvokeDeviceMethodAsync(deviceId, methodInvocation);
    }
}
