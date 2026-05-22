/// <reference types="bun" />
import { describe, expect, mock, test } from "bun:test";
import { fireEvent, render, screen } from "@testing-library/react";

import { OverworldHud } from "./OverworldHud";

const baseProps = {
    snapshotEntities: {},
    localPlayerId: null,
    sessionBootstrap: {
        sessionId: "overworld",
        token: "gst_overworld",
        playerName: "tester",
        playerGuid: "c9155c7f-a804-4f62-ae4f-c3db36410f50",
    },
    connectionState: "connected" as const,
    serverTick: 42,
    serverMetrics: null,
    nativeActivePlayerCount: 1,
    sessionPlayers: [],
    buildStats: {
        health: 120,
        attack: 19,
        defense: 13,
        utility: 7,
    },
    comboResult: {
        triggered: true,
        damageBonusPct: 25,
        mitigationBonusPct: 10,
        partySynergyBonusPct: 6,
    },
    buildBusy: false,
};

describe("OverworldHud Build Lab", () => {
    test("renders build lab details when session bootstrap is present", () => {
        render(
            <OverworldHud
                {...baseProps}
                onSetClass={() => { }}
                onApplyPreset={() => { }}
                onEquipGear={() => { }}
                onEvaluateCombo={() => { }}
            />
        );

        expect(screen.getByText("Build Lab")).not.toBeNull();
        expect(screen.getByText("Stats: HP 120 · ATK 19 · DEF 13 · UTL 7")).not.toBeNull();
        expect(
            screen.getByText("Combo: triggered · dmg +25% · mit +10% · party +6%")
        ).not.toBeNull();
    });

    test("wires class buttons and build actions to callbacks", () => {
        const onSetClass = mock<(classType: 0 | 1 | 2) => void>(() => { });
        const onApplyPreset = mock<() => void>(() => { });
        const onEquipGear = mock<() => void>(() => { });
        const onEvaluateCombo = mock<() => void>(() => { });

        render(
            <OverworldHud
                {...baseProps}
                onSetClass={onSetClass}
                onApplyPreset={onApplyPreset}
                onEquipGear={onEquipGear}
                onEvaluateCombo={onEvaluateCombo}
            />
        );

        fireEvent.click(screen.getByRole("button", { name: "Vanguard" }));
        fireEvent.click(screen.getByRole("button", { name: "Arcanist" }));
        fireEvent.click(screen.getByRole("button", { name: "Ranger" }));
        fireEvent.click(screen.getByRole("button", { name: "Apply 3/1/2" }));
        fireEvent.click(screen.getByRole("button", { name: "Equip T2 Weapon" }));
        fireEvent.click(screen.getByRole("button", { name: "Try Combo" }));

        expect(onSetClass).toHaveBeenNthCalledWith(1, 0);
        expect(onSetClass).toHaveBeenNthCalledWith(2, 1);
        expect(onSetClass).toHaveBeenNthCalledWith(3, 2);
        expect(onApplyPreset).toHaveBeenCalledTimes(1);
        expect(onEquipGear).toHaveBeenCalledTimes(1);
        expect(onEvaluateCombo).toHaveBeenCalledTimes(1);
    });

    test("disables build buttons while action is in flight", () => {
        render(
            <OverworldHud
                {...baseProps}
                buildBusy={true}
                onSetClass={() => { }}
                onApplyPreset={() => { }}
                onEquipGear={() => { }}
                onEvaluateCombo={() => { }}
            />
        );

        const vanguardButton = screen.getByRole("button", { name: "Vanguard" }) as HTMLButtonElement;
        const presetButton = screen.getByRole("button", { name: "Apply 3/1/2" }) as HTMLButtonElement;
        const comboButton = screen.getByRole("button", { name: "Try Combo" }) as HTMLButtonElement;

        expect(vanguardButton.disabled).toBe(true);
        expect(presetButton.disabled).toBe(true);
        expect(comboButton.disabled).toBe(true);
    });

    test("hides build lab when session bootstrap is missing", () => {
        const { queryByText } = render(
            <OverworldHud
                {...baseProps}
                sessionBootstrap={null}
                onSetClass={() => { }}
                onApplyPreset={() => { }}
                onEquipGear={() => { }}
                onEvaluateCombo={() => { }}
            />
        );

        expect(queryByText("Build Lab")).toBeNull();
    });

    test("renders fallback copy when stats and combo are unavailable", () => {
        render(
            <OverworldHud
                {...baseProps}
                buildStats={null}
                comboResult={null}
                onSetClass={() => { }}
                onApplyPreset={() => { }}
                onEquipGear={() => { }}
                onEvaluateCombo={() => { }}
            />
        );

        expect(screen.getByText("Stats: uninitialized")).not.toBeNull();
        expect(screen.getByText("Combo: none")).not.toBeNull();
    });

    test("does not fire callbacks when buttons are disabled", () => {
        const onSetClass = mock<(classType: 0 | 1 | 2) => void>(() => { });
        const onApplyPreset = mock<() => void>(() => { });
        const onEquipGear = mock<() => void>(() => { });
        const onEvaluateCombo = mock<() => void>(() => { });

        render(
            <OverworldHud
                {...baseProps}
                buildBusy={true}
                onSetClass={onSetClass}
                onApplyPreset={onApplyPreset}
                onEquipGear={onEquipGear}
                onEvaluateCombo={onEvaluateCombo}
            />
        );

        fireEvent.click(screen.getByRole("button", { name: "Vanguard" }));
        fireEvent.click(screen.getByRole("button", { name: "Apply 3/1/2" }));
        fireEvent.click(screen.getByRole("button", { name: "Equip T2 Weapon" }));
        fireEvent.click(screen.getByRole("button", { name: "Try Combo" }));

        expect(onSetClass).toHaveBeenCalledTimes(0);
        expect(onApplyPreset).toHaveBeenCalledTimes(0);
        expect(onEquipGear).toHaveBeenCalledTimes(0);
        expect(onEvaluateCombo).toHaveBeenCalledTimes(0);
    });
});
