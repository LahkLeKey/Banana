export class ProxyHttpError extends Error {
  public readonly statusCode: number;
  public readonly payload: unknown;
  public readonly headers: Headers;

  public constructor(
      message: string, statusCode: number, payload: unknown, headers: Headers) {
    super(message);
    this.statusCode = statusCode;
    this.payload = payload;
    this.headers = headers;
  }
}

export type ProxyResponse<TPayload> = {
  statusCode: number; payload: TPayload; headers: Headers;
};

async function parseBody(response: Response): Promise<unknown> {
  const text = await response.text();
  if (text.length === 0) {
    return null;
  }

  try {
    return JSON.parse(text);
  } catch {
    return text;
  }
}

export async function requestJson<TPayload>(
    url: URL, init?: RequestInit): Promise<ProxyResponse<TPayload>> {
  const response = await fetch(url, init);
  const payload = (await parseBody(response)) as TPayload;

  if (!response.ok) {
    throw new ProxyHttpError(
        `Upstream request failed: ${response.status} ${response.statusText}`,
        response.status,
        payload,
        response.headers,
    );
  }

  return {
    statusCode: response.status,
    payload,
    headers: response.headers,
  };
}
