import { describe, expect, test } from "bun:test";
import { render, screen } from "@testing-library/react";

import { ActorTooltipOverlay } from "./ActorTooltipOverlay";

describe("ActorTooltipOverlay", () => {
    test("renders tooltip labels for player entities", () => {
        render(
            <ActorTooltipOverlay
                localPlayerId={null}
                entities={{
                    "entity-1": {
                        id: 1,
                        type: "player",
                        active: true,
                        x: 1,
                        y: 0,
                        z: 1,
                        state: 1,
                        ownerPlayerId: "player_abc123",
                        ownerPlayerName: "alpha",
                        tooltipLabel: "alpha · abc123",
                    },
                    "entity-2": {
                        id: 2,
                        type: "npc",
                        active: true,
                        x: 4,
                        y: 0,
                        z: 3,
                        state: 1,
                    },
                }}
            />
        );

        expect(screen.getByLabelText("Actor labels")).not.toBeNull();
        expect(screen.getByText("alpha · abc123")).not.toBeNull();
    });

    test("returns no overlay when no player labels are present", () => {
        const { container } = render(
            <ActorTooltipOverlay
                localPlayerId={null}
                entities={{
                    "entity-2": {
                        id: 2,
                        type: "npc",
                        active: true,
                        x: 4,
                        y: 0,
                        z: 3,
                        state: 1,
                    },
                }}
            />
        );

        expect(container.firstChild).toBeNull();
    });
});
