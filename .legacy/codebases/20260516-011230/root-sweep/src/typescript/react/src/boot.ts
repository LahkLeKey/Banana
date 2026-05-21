export type ReactBootResult = {
    ready: true; channel : "react";
};

export function bootReactRuntime(): ReactBootResult
{
    return {ready : true, channel : "react"};
}
