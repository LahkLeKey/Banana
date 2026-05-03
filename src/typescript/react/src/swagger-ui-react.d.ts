declare module "swagger-ui-react"
{
    import type {ComponentType} from "react";

    type SwaggerUIProps = {
        url?: string;
        spec?: object;
        docExpansion?: "none" | "list" | "full";
        defaultModelsExpandDepth?: number;
    };

    const SwaggerUI: ComponentType<SwaggerUIProps>;
    export default SwaggerUI;
}
