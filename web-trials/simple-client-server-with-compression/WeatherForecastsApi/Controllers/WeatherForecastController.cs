using System;
using System.Diagnostics;
using System.Linq;
using System.Text.Json;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;

namespace WeatherForecasts.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class WeatherForecastController : ControllerBase
    {
        private static readonly string[] Summaries = new[]
        {
            "Freezing", "Bracing", "Chilly", "Cool", "Mild", "Warm", "Balmy", "Hot", "Sweltering", "Scorching"
        };

        private readonly ILogger<WeatherForecastController> _logger;

        public WeatherForecastController(ILogger<WeatherForecastController> logger)
        {
            _logger = logger;
        }

        // only serialization
        [HttpGet]
        public async Task<IActionResult> Get()
        {
            
            Debug.WriteLine(await HttpContext.Request.ToRawString());

            var rng = new Random();
            var forecasts = Enumerable.Range(1, 5).Select(index => new WeatherForecast
            {
                Date = DateTime.Now.AddDays(index),
                TemperatureC = rng.Next(-20, 55),
                Summary = Summaries[rng.Next(Summaries.Length)]
            })
            .ToArray();

            return new JsonResult(forecasts,
                new JsonSerializerOptions
                {
                    PropertyNamingPolicy = null
                });
        }

        [HttpPost]
        //[Consumes(MediaTypeNames.Application.Json)]
        public async Task<IActionResult> Post([FromBody]WeatherForecast weatherForecast)
        {
            Debug.WriteLine(await HttpContext.Request.ToRawString());

            var rng = new Random();
            var forecasts = Enumerable.Range(1, 5)
            .Select(index => new WeatherForecast
            {
                Date = DateTime.Now.AddDays(index),
                TemperatureC = rng.Next(-20, 55),
                Summary = Summaries[rng.Next(Summaries.Length)]
            })
            .Append(weatherForecast)
            .ToArray();

            var result = new JsonResult(forecasts,
                new JsonSerializerOptions
                {
                    PropertyNamingPolicy = null
                });

            return result;
        }
    }
}
