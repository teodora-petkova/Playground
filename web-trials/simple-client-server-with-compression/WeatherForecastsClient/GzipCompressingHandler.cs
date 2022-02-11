using System;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;

//source: https://cymbeline.ch/2014/03/16/gzip-encoding-an-http-post-request-body/

public sealed class GzipCompressingHandler : DelegatingHandler
{
    public GzipCompressingHandler(HttpMessageHandler innerHandler)
    {
        if (null == innerHandler)
        {
            throw new ArgumentNullException("NULL innerHandler for GzipCompressingHandler");
        }
 
        InnerHandler = innerHandler;
    }
 
    protected override Task<HttpResponseMessage> SendAsync(HttpRequestMessage request, CancellationToken cancellationToken)
    {
        HttpContent content = request.Content;
 
        if (request.Method == HttpMethod.Post)
        {
            // wrap the original HttpContent in our custom GzipContent class
            request.Content = new GzipContent(request.Content);
        }
 
        return base.SendAsync(request, cancellationToken);
    }
}