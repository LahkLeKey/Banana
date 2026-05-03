export default async function middleware(request: Request): Promise<Response>
{
    const url = new URL(request.url);

    if (url.pathname.startsWith("/api/legacy/"))
    {
        url.pathname = url.pathname.replace("/api/legacy/", "/api/v2/");
        return Response.redirect(url.toString(), 308);
    }

    return fetch(request);
}
