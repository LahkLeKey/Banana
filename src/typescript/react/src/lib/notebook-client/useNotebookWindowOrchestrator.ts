import {useMemo, useState} from 'react';

import {useNotebookLayoutStore} from './layoutStore';

export type NotebookHudPanelId = 'explorer'|'menu'|'status'|'operations'|
    'intelNode'|'objectiveNode'|'playerNode'|'questLog'|'nodeOps';

type UseNotebookWindowOrchestratorOptions = {
  readonly initialPinnedPanels?: readonly NotebookHudPanelId[];
};

export function useNotebookWindowOrchestrator({
  initialPinnedPanels =
      [
        'menu',
        'explorer',
        'intelNode',
        'objectiveNode',
        'playerNode',
        'questLog',
        'nodeOps',
      ],
}: UseNotebookWindowOrchestratorOptions = {}) {
  const showExplorer = useNotebookLayoutStore((state) => state.showExplorer);
  const showMenu = useNotebookLayoutStore((state) => state.showMenu);
  const showStatus = useNotebookLayoutStore((state) => state.showStatus);
  const showOperations =
      useNotebookLayoutStore((state) => state.showOperations);
  const showObjectiveNode =
      useNotebookLayoutStore((state) => state.showObjectiveNodeWindow);
  const showIntelNode =
      useNotebookLayoutStore((state) => state.showIntelNodeWindow);
  const showPlayerNode =
      useNotebookLayoutStore((state) => state.showPlayerNodeWindow);
  const showQuestLog =
      useNotebookLayoutStore((state) => state.showQuestLogWindow);
  const showNodeOps =
      useNotebookLayoutStore((state) => state.showNodeOpsWindow);

  const toggleHudPanel =
      useNotebookLayoutStore((state) => state.toggleHudPanel);
  const setHudPanelVisibility =
      useNotebookLayoutStore((state) => state.setHudPanelVisibility);
  const setGameplayWindowVisibility =
      useNotebookLayoutStore((state) => state.setGameplayWindowVisibility);
  const resetHudPanels =
      useNotebookLayoutStore((state) => state.resetHudPanels);
  const closeAllHudPanels =
      useNotebookLayoutStore((state) => state.closeAllHudPanels);
  const reopenHudPanels =
      useNotebookLayoutStore((state) => state.reopenHudPanels);
  const closeAllGameplayWindows =
      useNotebookLayoutStore((state) => state.closeAllGameplayWindows);
  const reopenGameplayWindows =
      useNotebookLayoutStore((state) => state.reopenGameplayWindows);

  const [pinnedPanels, setPinnedPanels] = useState<NotebookHudPanelId[]>([
    ...initialPinnedPanels,
  ]);

  const panelVisibility = useMemo(
      () => ({
        explorer: showExplorer,
        menu: showMenu,
        status: showStatus,
        operations: showOperations,
        intelNode: showIntelNode,
        objectiveNode: showObjectiveNode,
        playerNode: showPlayerNode,
        questLog: showQuestLog,
        nodeOps: showNodeOps,
      } as const),
      [
        showExplorer,
        showMenu,
        showStatus,
        showOperations,
        showIntelNode,
        showObjectiveNode,
        showPlayerNode,
        showQuestLog,
        showNodeOps,
      ],
  );

  const togglePanelPin = (panel: NotebookHudPanelId) => {
    setPinnedPanels((previous) => {
      if (previous.includes(panel)) {
        return previous.filter((entry) => entry !== panel);
      }
      return [...previous, panel];
    });
  };

  const closePanel = (panel: NotebookHudPanelId) => {
    if (panel === 'explorer' || panel === 'menu' || panel === 'status' ||
        panel === 'operations') {
      setHudPanelVisibility(panel, false);
      return;
    }
    if (panel === 'objectiveNode') {
      setGameplayWindowVisibility('objectiveNode', false);
      return;
    }

    if (panel === 'intelNode') {
      setGameplayWindowVisibility('intelNode', false);
      return;
    }

    if (panel === 'playerNode') {
      setGameplayWindowVisibility('playerNode', false);
      return;
    }

    if (panel === 'questLog') {
      setGameplayWindowVisibility('questLog', false);
      return;
    }

    setGameplayWindowVisibility('nodeOps', false);
  };

  const togglePanel = (panel: NotebookHudPanelId) => {
    if (panel === 'explorer' || panel === 'menu' || panel === 'status' ||
        panel === 'operations') {
      toggleHudPanel(panel);
      return;
    }

    if (panel === 'objectiveNode') {
      setGameplayWindowVisibility('objectiveNode', !showObjectiveNode);
      return;
    }

    if (panel === 'intelNode') {
      setGameplayWindowVisibility('intelNode', !showIntelNode);
      return;
    }

    if (panel === 'playerNode') {
      setGameplayWindowVisibility('playerNode', !showPlayerNode);
      return;
    }

    if (panel === 'questLog') {
      setGameplayWindowVisibility('questLog', !showQuestLog);
      return;
    }

    setGameplayWindowVisibility('nodeOps', !showNodeOps);
  };

  const closeForViewport = () => {
    setHudPanelVisibility('explorer', false);
    setHudPanelVisibility('menu', false);
    setHudPanelVisibility('status', false);
    setHudPanelVisibility('operations', false);
  };

  const closeAllWindows = () => {
    closeAllHudPanels();
    closeAllGameplayWindows();
  };

  const reopenAllWindows = () => {
    reopenHudPanels();
    reopenGameplayWindows();
  };

  return {
    showExplorer,
    showMenu,
    showStatus,
    showOperations,
    showIntelNode,
    showObjectiveNode,
    showPlayerNode,
    showQuestLog,
    showNodeOps,
    panelVisibility,
    pinnedPanels,
    togglePanelPin,
    togglePanel,
    closePanel,
    closeForViewport,
    resetHudPanels,
    closeAllHudPanels,
    reopenHudPanels,
    closeAllWindows,
    reopenAllWindows,
  };
}
