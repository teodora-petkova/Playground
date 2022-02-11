
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Text;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Primitives;

namespace WeatherForecasts
{
    public class CustomMiddleware
    {
        private readonly RequestDelegate _next;
        public CustomMiddleware(RequestDelegate next)
        {
            _next = next;
        }

        static async Task<Stream> DecompressStream(Stream compressed)
        {
            var outStream = new MemoryStream();
            using (var gzipStream = new GZipStream(compressed, CompressionMode.Decompress /*leaveOpen: true*/))
            {
                await gzipStream.CopyToAsync(outStream);
                await gzipStream.FlushAsync();

                var debugOutput = Encoding.UTF8.GetString(outStream.ToArray());
                Debug.WriteLine(debugOutput);
            }
            outStream.Seek(0, SeekOrigin.Begin);
            outStream.Position = 0;
            return outStream;

        }

        public async Task InvokeAsync(HttpContext httpContext)
        {
            if (httpContext.Request.Headers.ContainsKey("Content-Encoding"))
            {
                if (httpContext.Request.Headers["Content-Encoding"] == "gzip")
                {
                    httpContext.Request.Body = await DecompressStream(httpContext.Request.Body);
                }
            }

            await _next(httpContext);
        }
    }

    // extension method used to add the middleware to the HTTP request pipeline.
    public static class CustomMiddlewareExtensions
    {
        public static IApplicationBuilder UseCustomMiddleware(this IApplicationBuilder builder)
        {
            return builder.UseMiddleware<CustomMiddleware>();
        }
    }
}