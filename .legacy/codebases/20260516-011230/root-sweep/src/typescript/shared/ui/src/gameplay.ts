export type MovementVector = {
    x: number; z : number;
};

export type GameplayBootState = {
    ready: boolean; channel : "react" | "electron" | "react-native";
};

export function normalizeMovement(input: MovementVector): MovementVector
{
    const length = Math.hypot(input.x, input.z);
    if (length <= 1e-6)
    {
        return {x : 0, z : 0};
    }
    return {x : input.x / length, z : input.z / length};
}
