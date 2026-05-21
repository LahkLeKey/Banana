export type ReactNativeBootResult = {
    ready: true; channel : "react-native";
};

export function bootReactNativeRuntime(): ReactNativeBootResult
{
    return {ready : true, channel : "react-native"};
}
