import { useMemo, useState } from "react";
import { Platform, SafeAreaView, ScrollView, StatusBar, StyleSheet, Text, View } from "react-native";
import {
    Badge,
    Button,
    Card,
    CardContent,
    CardDescription,
    CardHeader,
    CardTitle,
    Input,
    Label,
    bananaUiTheme
} from "@banana/ui/native";

type RequestState = "idle" | "loading" | "ready" | "error";

type HealthResponse = {
    status: string;
};

type BananaResponse = {
    banana: number;
    purchases: number;
    multiplier: number;
};

const defaultApiBase =
    process.env.EXPO_PUBLIC_BANANA_API_BASE_URL ?? (Platform.OS === "android" ? "http://10.0.2.2:8080" : "http://localhost:8080");

function toErrorMessage(value: unknown): string {
    if (value instanceof Error) {
        return value.message;
    }

    return "Unknown error";
}

export default function App(): JSX.Element {
    const [apiBaseUrl, setApiBaseUrl] = useState(defaultApiBase);
    const [purchasesInput, setPurchasesInput] = useState("10");
    const [multiplierInput, setMultiplierInput] = useState("5");

    const [healthState, setHealthState] = useState<RequestState>("idle");
    const [bananaState, setBananaState] = useState<RequestState>("idle");

    const [healthPayload, setHealthPayload] = useState<HealthResponse | null>(null);
    const [bananaPayload, setBananaPayload] = useState<BananaResponse | null>(null);
    const [lastError, setLastError] = useState<string | null>(null);

    const parsedPurchases = useMemo(() => Math.max(0, Number(purchasesInput) || 0), [purchasesInput]);
    const parsedMultiplier = useMemo(() => Math.max(0, Number(multiplierInput) || 0), [multiplierInput]);

    const bananaSummary = useMemo(() => {
        if (!bananaPayload) {
            return "Run a banana request to render output.";
        }

        return `${bananaPayload.purchases} x ${bananaPayload.multiplier} = ${bananaPayload.banana}`;
    }, [bananaPayload]);

    async function runHealthCheck(): Promise<void> {
        setHealthState("loading");
        setLastError(null);

        try {
            const response = await fetch(`${apiBaseUrl}/health`);
            if (!response.ok) {
                throw new Error(`Health check failed with status ${response.status}`);
            }

            const payload = (await response.json()) as HealthResponse;
            setHealthPayload(payload);
            setHealthState("ready");
        } catch (error) {
            setHealthState("error");
            setLastError(toErrorMessage(error));
        }
    }

    async function runBananaRequest(): Promise<void> {
        setBananaState("loading");
        setLastError(null);

        try {
            const query = new URLSearchParams({
                purchases: String(parsedPurchases),
                multiplier: String(parsedMultiplier)
            });

            const response = await fetch(`${apiBaseUrl}/banana?${query.toString()}`);
            if (!response.ok) {
                throw new Error(`Banana request failed with status ${response.status}`);
            }

            const payload = (await response.json()) as BananaResponse;
            setBananaPayload(payload);
            setBananaState("ready");
        } catch (error) {
            setBananaState("error");
            setLastError(toErrorMessage(error));
        }
    }

    return (
        <SafeAreaView style={styles.safeArea}>
            <StatusBar barStyle="dark-content" />
            <ScrollView contentContainerStyle={styles.content}>
                <View style={styles.header}>
                    <Badge variant="secondary">Banana Mobile</Badge>
                    <Text style={styles.title}>React Native + shared @banana/ui primitives</Text>
                    <Text style={styles.subtitle}>
                        This app uses the same @banana/ui package as web and Electron, with React Native-specific component implementations.
                    </Text>
                </View>

                <Card>
                    <CardHeader>
                        <CardTitle>Runtime Inputs</CardTitle>
                        <CardDescription>
                            On Android emulator, localhost should map to 10.0.2.2. Replace this URL for a physical device.
                        </CardDescription>
                    </CardHeader>
                    <CardContent style={styles.formContent}>
                        <View style={styles.field}>
                            <Label>API base URL</Label>
                            <Input
                                autoCapitalize="none"
                                autoCorrect={false}
                                keyboardType="url"
                                onChangeText={setApiBaseUrl}
                                value={apiBaseUrl}
                            />
                        </View>

                        <View style={styles.field}>
                            <Label>Purchases</Label>
                            <Input keyboardType="numeric" onChangeText={setPurchasesInput} value={purchasesInput} />
                        </View>

                        <View style={styles.field}>
                            <Label>Multiplier</Label>
                            <Input keyboardType="numeric" onChangeText={setMultiplierInput} value={multiplierInput} />
                        </View>

                        <View style={styles.buttonRow}>
                            <Button onPress={() => void runHealthCheck()} variant="outline">
                                {healthState === "loading" ? "Checking..." : "Check Health"}
                            </Button>
                            <Button onPress={() => void runBananaRequest()}>
                                {bananaState === "loading" ? "Calculating..." : "Calculate Banana"}
                            </Button>
                        </View>
                    </CardContent>
                </Card>

                <Card>
                    <CardHeader>
                        <CardTitle>Results</CardTitle>
                        <CardDescription>Inspect shared runtime responses from mobile.</CardDescription>
                    </CardHeader>
                    <CardContent style={styles.resultsContent}>
                        <View style={styles.resultRow}>
                            <Label>Health</Label>
                            <Text style={styles.resultText}>{healthPayload?.status ?? "No response yet"}</Text>
                        </View>

                        <View style={styles.resultRow}>
                            <Label>Banana Summary</Label>
                            <Text style={styles.resultText}>{bananaSummary}</Text>
                        </View>

                        {lastError ? (
                            <View style={styles.errorBlock}>
                                <Label style={styles.errorLabel}>Last Error</Label>
                                <Text style={styles.errorText}>{lastError}</Text>
                            </View>
                        ) : null}
                    </CardContent>
                </Card>
            </ScrollView>
        </SafeAreaView>
    );
}

const styles = StyleSheet.create({
    safeArea: {
        backgroundColor: bananaUiTheme.colors.background,
        flex: 1
    },
    content: {
        gap: bananaUiTheme.spacing.lg,
        paddingHorizontal: bananaUiTheme.spacing.lg,
        paddingVertical: bananaUiTheme.spacing.xl
    },
    header: {
        gap: bananaUiTheme.spacing.sm
    },
    title: {
        color: bananaUiTheme.colors.foreground,
        fontSize: 30,
        fontWeight: "700",
        lineHeight: 36
    },
    subtitle: {
        color: bananaUiTheme.colors.mutedForeground,
        fontSize: 15,
        lineHeight: 21
    },
    formContent: {
        gap: bananaUiTheme.spacing.md
    },
    field: {
        gap: bananaUiTheme.spacing.xs
    },
    buttonRow: {
        flexDirection: "row",
        flexWrap: "wrap",
        gap: bananaUiTheme.spacing.sm,
        justifyContent: "flex-start",
        marginTop: bananaUiTheme.spacing.sm
    },
    resultsContent: {
        gap: bananaUiTheme.spacing.md
    },
    resultRow: {
        gap: bananaUiTheme.spacing.xs
    },
    resultText: {
        color: bananaUiTheme.colors.foreground,
        fontSize: 15,
        lineHeight: 22
    },
    errorBlock: {
        backgroundColor: "#FFE8E8",
        borderColor: "#F3B2B2",
        borderRadius: bananaUiTheme.radius.md,
        borderWidth: 1,
        gap: bananaUiTheme.spacing.xs,
        padding: bananaUiTheme.spacing.md
    },
    errorLabel: {
        color: "#A21616"
    },
    errorText: {
        color: "#7A1414",
        fontSize: 14,
        lineHeight: 20
    }
});
