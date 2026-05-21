import { Button } from "../ui/button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../ui/card";
import { Input } from "../ui/input";

export interface OverworldEntryCardProps {
    readonly playerName: string;
    readonly loading: boolean;
    readonly error: string | null;
    readonly onPlayerNameChange: (nextPlayerName: string) => void;
    readonly onEnterOverworld: () => void;
    readonly onResetAndEnter: () => void;
}

export function OverworldEntryCard({
    playerName,
    loading,
    error,
    onPlayerNameChange,
    onEnterOverworld,
    onResetAndEnter,
}: OverworldEntryCardProps) {
    return (
        <Card className="w-full max-w-lg">
            <CardHeader>
                <CardTitle>Server-authority overworld</CardTitle>
                <CardDescription>
                    Join the shared overworld session managed by the authoritative server.
                </CardDescription>
            </CardHeader>
            <CardContent className="space-y-4">
                <div className="space-y-2">
                    <label htmlFor="player-name">Player name</label>
                    <Input
                        id="player-name"
                        value={playerName}
                        onChange={(event) => onPlayerNameChange(event.target.value)}
                        placeholder="player"
                    />
                </div>

                {error ? (
                    <p role="alert" className="text-sm text-red-500">
                        {error}
                    </p>
                ) : null}

                <div className="flex gap-3">
                    <Button onClick={onEnterOverworld} disabled={loading}>
                        Enter overworld
                    </Button>
                    <Button variant="secondary" onClick={onResetAndEnter} disabled={loading}>
                        Reset and enter
                    </Button>
                </div>
            </CardContent>
        </Card>
    );
}
