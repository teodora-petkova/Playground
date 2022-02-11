using System;
using System.Net.Http;
using System.Threading.Tasks;
using System.Net;
using System.Text.Json;
using System.Diagnostics;
using System.Text;

namespace TodoApiClient
{
    class Program
    {
        private static async Task GetForecasts(HttpClient client, string url)
        {
            using (var httpRequest = new HttpRequestMessage(HttpMethod.Get, url))
            {
                Debug.WriteLine("API request:\n " + await httpRequest.ToRawString());

                var apiResponse = await client.SendAsync(httpRequest);

                Debug.WriteLine("API response:\n " + await apiResponse.ToRawString());

                string jsonString = "";
                if (apiResponse.IsSuccessStatusCode)
                {
                    jsonString = await apiResponse.Content.ReadAsStringAsync();

                    Debug.WriteLine("Response content JSON as string: \n" + jsonString);
                    //var obj = JsonSerializer.Deserialize<WeatherForecast[]>(jsonString);
                }
            }
        }

       private static async Task PostForecasts(HttpClient client, string url)
        {
            using (var httpRequest = new HttpRequestMessage(HttpMethod.Post, url))
            {
                WeatherForecast w = new WeatherForecast
                { 
                    Date = DateTime.Now.AddDays(3),
                    TemperatureC = 10,
                    Summary = "new"
                };

                string json = JsonSerializer.Serialize(w);
                
                Debug.WriteLine("JSON Length: " + json.Length);
                
                // TODO: to understand why GZip compression removes the end of the stream in a random fashion... !!!
                httpRequest.Content = new StringContent(json+"123456789", Encoding.UTF8, "application/json");

                Debug.WriteLine("API request:\n " + await httpRequest.ToRawString());

                var apiResponse = await client.SendAsync(httpRequest);

                Debug.WriteLine("API response:\n " + await apiResponse.ToRawString());

                string jsonString = "";
                if (apiResponse.IsSuccessStatusCode)
                {
                    jsonString = await apiResponse.Content.ReadAsStringAsync();

                    Debug.WriteLine("Response JSON as string:\n " + await apiResponse.ToRawString());

                    var forecasts = JsonSerializer.Deserialize<WeatherForecast[]>(jsonString);

                    Debug.WriteLine("JSON response deserialized summaries:");
                    foreach (var forcast in forecasts)
                    {
                        Debug.WriteLine(forcast.Summary.ToString());
                    }
                }

            }
        }

        static async Task Main(string[] args)
        {
            string url = "https://localhost:5001/weatherforecast";
            
            using (HttpClientHandler clientHandler = new HttpClientHandler()
            {
                AutomaticDecompression = DecompressionMethods.GZip | DecompressionMethods.Deflate 
                
            })
            using(GzipCompressingHandler compressHandler = new GzipCompressingHandler(clientHandler))
            {
                // skip ssl certifications for the simple demo
                clientHandler.ServerCertificateCustomValidationCallback = 
                    (sender, cert, chain, sslPolicyErrors) => { return true; };


                using (var client = new HttpClient(compressHandler))
                {
                    client.DefaultRequestHeaders.Accept.Clear();

                    //client.DefaultRequestHeaders.AcceptEncoding.Add(new StringWithQualityHeaderValue("gzip"));
                    //client.DefaultRequestHeaders.AcceptEncoding.Add(new StringWithQualityHeaderValue("deflate"));

                    //await GetForecasts(client, url);

                    await PostForecasts(client, url);
                }
            }
        }
    }
}
