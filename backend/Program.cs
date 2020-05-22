using System;
using System.IO;
using System.Reflection;
using System.Text;
using MQTTnet;
using MQTTnet.Protocol;
using MQTTnet.Server;
using Serilog;

namespace backend
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            var currentPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

            Log.Logger = new LoggerConfiguration()
                .MinimumLevel.Debug()
                // ReSharper disable once AssignNullToNotNullAttribute
                .WriteTo.File(Path.Combine(currentPath,
                    @"log\SimpleMqttServer_.txt"), rollingInterval: RollingInterval.Day)
                .WriteTo.Console()
                .CreateLogger();

            var optionsBuilder = new MqttServerOptionsBuilder().WithDefaultEndpoint().WithApplicationMessageInterceptor(
                    c =>
                    {
                        c.AcceptPublish = true;
                        LogMessage(c);
                    }).WithSubscriptionInterceptor(
                    c =>
                    {
                        c.AcceptSubscription = true;
                        LogMessage(c, true);
                    });
            var mqttServer = new MqttFactory().CreateMqttServer();
            mqttServer.StartAsync(optionsBuilder.Build());
            Console.ReadLine();
        }

        private static void LogMessage(MqttSubscriptionInterceptorContext context, bool successful)
        {
            if (context == null)
            {
                return;
            }
            
            Log.Information(successful ? $"New subscription: ClientId = {context.ClientId}, TopicFilter = {context.TopicFilter}" : $"Subscription failed for clientId = {context.ClientId}, TopicFilter = {context.TopicFilter}");
        }

        private static void LogMessage(MqttApplicationMessageInterceptorContext context)
        {
            if (context == null)
            {
                return;
            }

            var payload = context.ApplicationMessage?.Payload == null ? null : Encoding.UTF8.GetString(context.ApplicationMessage?.Payload);

            Log.Information(
                $"Message: ClientId = {context.ClientId}, Topic = {context.ApplicationMessage?.Topic},"
                + $" Payload = {payload}, QoS = {context.ApplicationMessage?.QualityOfServiceLevel},"
                + $" Retain-Flag = {context.ApplicationMessage?.Retain}");
        }

        private static void LogMessage(MqttConnectionValidatorContext context, bool showPassword)
        {
            if (context == null)
            {
                return;
            }

            if (showPassword)
            {
                Log.Information(
                    $"New connection: ClientId = {context.ClientId}, Endpoint = {context.Endpoint},"
                    + $" Username = {context.Username}, Password = {context.Password},"
                    + $" CleanSession = {context.CleanSession}");
            }
            else
            {
                Log.Information(
                    $"New connection: ClientId = {context.ClientId}, Endpoint = {context.Endpoint},"
                    + $" Username = {context.Username}, CleanSession = {context.CleanSession}");
            }
        }
    }
}
