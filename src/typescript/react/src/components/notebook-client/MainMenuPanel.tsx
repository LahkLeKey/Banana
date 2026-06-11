import { PanelBase, RouteStatTile as StatTile, RouteSubActionLink } from '@banana/ui';

type MainMenuPanelProps = {
    readonly indexedFileCount: number;
    readonly notebookCellCount: number;
    readonly manifestAvailable: boolean;
    readonly playerLevel: number;
    readonly totalXp: number;
    readonly comboStreak: number;
    readonly completedQuestCount: number;
    readonly totalQuestCount: number;
};

export function MainMenuPanel({
    indexedFileCount,
    notebookCellCount,
    manifestAvailable,
    playerLevel,
    totalXp,
    comboStreak,
    completedQuestCount,
    totalQuestCount,
}: MainMenuPanelProps) {
    const statusText = `Lv${playerLevel} · ${totalXp} XP · ${completedQuestCount}/${totalQuestCount} quests · ${manifestAvailable ? 'Synced' : 'Awaiting sync'}`;

    return (
        <PanelBase
            title="Main Menu"
            headerAction={
                <>
                    <RouteSubActionLink href="/notebooks/native-c-catalog.ipynb">Payload</RouteSubActionLink>
                    <RouteSubActionLink href="/notebooks/catalog-index.json">Manifest</RouteSubActionLink>
                </>
            }
            footer={statusText}
            variant="default"
        >
            <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(140px, 1fr))', gap: 8 }}>
                <StatTile label="Map Sectors" value={indexedFileCount} />
                <StatTile label="Runtime Cells" value={notebookCellCount} />
                <StatTile
                    label="Connection"
                    value={manifestAvailable ? 'Synced' : 'Awaiting'}
                    accent={manifestAvailable ? '#86efac' : '#fca5a5'}
                />
                <StatTile label="Cmd Level" value={playerLevel} accent="#fcd34d" />
                <StatTile label="Total XP" value={totalXp} accent="#67e8f9" />
                <StatTile label="Combo" value={`x${Math.max(1, comboStreak)}`} accent="#a5b4fc" />
                <StatTile label="Quests" value={`${completedQuestCount}/${totalQuestCount}`} accent="#f9a8d4" />
            </div>
        </PanelBase>
    );
}
