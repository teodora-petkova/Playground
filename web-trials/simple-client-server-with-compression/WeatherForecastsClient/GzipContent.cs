using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;

//source: https://cymbeline.ch/2014/03/16/gzip-encoding-an-http-post-request-body/

internal sealed class GzipContent : HttpContent
{
    private readonly HttpContent content;
 
    public GzipContent(HttpContent content)
    {
        this.content = content;
 
        // keep the original content's headers
        foreach (KeyValuePair<string, IEnumerable<string>> header in content.Headers)
        {
            Headers.TryAddWithoutValidation(header.Key, header.Value);
        }
 
        //  and let the server know we've Gzip-compressed the body of this request.
        Headers.ContentEncoding.Add("gzip");
    }

    protected override async Task SerializeToStreamAsync(Stream decompressed, TransportContext context)
    {
        var compressed = new MemoryStream();
        using (var zip = new GZipStream(compressed, CompressionMode.Compress, leaveOpen:true))
        {
            await content.CopyToAsync(zip);
        }

        compressed.Seek(0, SeekOrigin.Begin);
        compressed.Position = 0;
        await compressed.CopyToAsync(decompressed);
        await compressed.FlushAsync();
        compressed.Close();
    }

    protected override bool TryComputeLength(out long length)
    {
        length = -1;
        return false;
    }
}