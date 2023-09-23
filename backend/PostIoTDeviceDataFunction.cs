using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging;
using Microsoft.Azure.Devices;
using System.Threading.Tasks;
using System;
using System.IO;

public static class PostIotDeviceDataFunction
{
    private static readonly string iotHubConnectionString = Environment.GetEnvironmentVariable("IoTHubConnectionString");
    private static readonly ServiceClient serviceClient = CreateServiceClient();

    [FunctionName("PostIotDeviceDataFunction")]
    public static async Task<IActionResult> UpdateData(
    [HttpTrigger(AuthorizationLevel.Function, "post", Route = null)] HttpRequest req,
    ILogger log)
    {
        if (string.IsNullOrWhiteSpace(iotHubConnectionString))
        {
            log.LogError("IoTHubConnectionString environment variable is not set.");
            return new StatusCodeResult(StatusCodes.Status500InternalServerError);
        }

        string deviceId = req.Query["deviceId"];
        if (string.IsNullOrWhiteSpace(deviceId))
        {
            return new BadRequestObjectResult("Please provide a deviceId.");
        }

        string payload = await new StreamReader(req.Body).ReadToEndAsync();
        var response = await InvokeDirectMethod(deviceId, "UpdateDataMethod", payload);
        if (response.Status != 200)
        {
            log.LogWarning($"Device {deviceId} returned status: {response.Status}. Message: {response.GetPayloadAsJson()}");
            return new BadRequestObjectResult($"Device returned status: {response.Status}. Message: {response.GetPayloadAsJson()}");
        }

        return new OkObjectResult($"Device {deviceId} updated successfully.");
    }

    private static ServiceClient CreateServiceClient()
    {
        if (string.IsNullOrWhiteSpace(iotHubConnectionString))
        {
            return null;
        }

        return ServiceClient.CreateFromConnectionString(iotHubConnectionString);
    }

    private static async Task<CloudToDeviceMethodResult> InvokeDirectMethod(string deviceId, string methodName, string payload = null)
    {
        var methodInvocation = new CloudToDeviceMethod(methodName) { ResponseTimeout = TimeSpan.FromSeconds(30) };

        if (!string.IsNullOrEmpty(payload))
        {
            methodInvocation.SetPayloadJson(payload);
        }

        return await serviceClient.InvokeDeviceMethodAsync(deviceId, methodInvocation);
    }
}
