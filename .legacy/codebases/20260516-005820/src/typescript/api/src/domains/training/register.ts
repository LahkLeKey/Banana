import type {FastifyInstance} from "fastify";

import {registerLabelQueueRoutes} from "./label-queue.ts";
import {registerTrainingWorkbenchRoutes} from "./routes.ts";

export type TrainingControlPlaneRegistrationOptions = {
    enabled: boolean;
};

export async function registerTrainingControlPlaneRoutes(
    app: FastifyInstance, options: TrainingControlPlaneRegistrationOptions): Promise<void>
{
    if (!options.enabled)
    {
        app.log.info(
            "training control-plane routes disabled (set BANANA_ENABLE_TRAINING_CONTROL_PLANE=true to enable)");
        return;
    }

    await registerTrainingWorkbenchRoutes(app);
    await registerLabelQueueRoutes(app);
}
