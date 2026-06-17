import {create} from 'zustand';

import type {WorkflowDepth, WorkflowStepId} from '../../domain/notebook/workflow-domain';

export type ActiveDock = 'intel'|'objectives'|'player'|null;
export type IntelPanel = 'threat'|'signal'|'route';
export type ObjectivePanel = 'board'|'timeline';
export type PlayerPanel = 'rank'|'economy'|'streak';

const workflowOrder: WorkflowStepId[] = ['scan', 'shape', 'commit'];

function pickHudPanelFromPreset(preset: {
  explorer: boolean; menu: boolean; status: boolean; operations: boolean;
  visualizations: boolean;
  training: boolean;
}): 'explorer'|'menu'|'status'|'operations'|'visualizations'|'training'|null {
  if (preset.explorer) {
    return 'explorer';
  }
  if (preset.menu) {
    return 'menu';
  }
  if (preset.status) {
    return 'status';
  }
  if (preset.operations) {
    return 'operations';
  }
  if (preset.visualizations) {
    return 'visualizations';
  }
  if (preset.training) {
    return 'training';
  }
  return null;
}

type NotebookLayoutState = {
  activeDock: ActiveDock; intelPanel: IntelPanel;
  objectivePanel: ObjectivePanel;
  playerPanel: PlayerPanel;
  activeWorkflowStep: WorkflowStepId;
  workflowDepth: WorkflowDepth;
  showFullSourceDump: boolean;
  showExplorer: boolean;
  showMenu: boolean;
  showStatus: boolean;
  showOperations: boolean;
  showVisualizations: boolean;
  showTraining: boolean;
  lastHudSnapshot: {
    explorer: boolean; menu: boolean; status: boolean; operations: boolean;
    visualizations: boolean;
    training: boolean;
  };
  showIntelNodeWindow: boolean;
  showObjectiveNodeWindow: boolean;
  showPlayerNodeWindow: boolean;
  showQuestLogWindow: boolean;
  showNodeOpsWindow: boolean;
  lastGameplayWindowSnapshot: {
    intelNode: boolean; objectiveNode: boolean; playerNode: boolean;
    questLog: boolean;
    nodeOps: boolean;
  };
  lastGameplayDockSnapshot: ActiveDock;
  explorerDropupOpen: boolean;
  setActiveDock: (dock: ActiveDock) => void;
  setIntelPanel: (panel: IntelPanel) => void;
  setObjectivePanel: (panel: ObjectivePanel) => void;
  setPlayerPanel: (panel: PlayerPanel) => void;
  setActiveWorkflowStep: (step: WorkflowStepId) => void;
  setWorkflowDepth: (depth: WorkflowDepth) => void;
  setShowFullSourceDump: (show: boolean) => void;
  setExplorerDropupOpen: (open: boolean) => void;
  cycleWorkflowStep: (direction?: 1|- 1) => void;
  adjustWorkflowDepth: (delta: -1|1) => void;
  applyHudPreset: (preset: {
    explorer: boolean; menu: boolean; status: boolean; operations: boolean;
    visualizations: boolean;
    training: boolean;
  }) => void;
  setHudPanelVisibility: (
      panel: 'explorer'|'menu'|'status'|'operations'|'visualizations'|
      'training',
      visible: boolean,
      ) => void;
  toggleHudPanel: (panel: 'explorer'|'menu'|'status'|'operations'|
                   'visualizations'|'training') => void;
  closeAllHudPanels: () => void;
  reopenHudPanels: () => void;
  setGameplayWindowVisibility: (
      window: 'intelNode'|'objectiveNode'|'playerNode'|'questLog'|'nodeOps',
      visible: boolean,
      ) => void;
  closeAllGameplayWindows: () => void;
  reopenGameplayWindows: () => void;
  focusHudPanel: (panel: 'explorer'|'menu'|'status'|'operations'|
                  'visualizations'|'training'|null) => void;
  resetHudPanels: () => void;
  resetForSector: (hasSelection: boolean) => void;
};

export const useNotebookLayoutStore = create<NotebookLayoutState>(
    (set) => ({
      activeDock: 'intel',
      intelPanel: 'threat',
      objectivePanel: 'board',
      playerPanel: 'rank',
      activeWorkflowStep: 'scan',
      workflowDepth: 1,
      showFullSourceDump: false,
      showExplorer: false,
      showMenu: false,
      showStatus: false,
      showOperations: false,
      showVisualizations: true,
      showTraining: false,
      lastHudSnapshot: {
        explorer: false,
        menu: false,
        status: false,
        operations: false,
        visualizations: true,
        training: false,
      },
      showIntelNodeWindow: false,
      showObjectiveNodeWindow: false,
      showPlayerNodeWindow: false,
      showQuestLogWindow: false,
      showNodeOpsWindow: false,
      lastGameplayWindowSnapshot: {
        intelNode: false,
        objectiveNode: false,
        playerNode: false,
        questLog: false,
        nodeOps: false,
      },
      lastGameplayDockSnapshot: 'intel',
      explorerDropupOpen: false,
      setActiveDock: (dock) => set({activeDock: dock}),
      setIntelPanel: (panel) => set({intelPanel: panel}),
      setObjectivePanel: (panel) => set({objectivePanel: panel}),
      setPlayerPanel: (panel) => set({playerPanel: panel}),
      setActiveWorkflowStep: (step) => set({activeWorkflowStep: step}),
      setWorkflowDepth: (depth) => set(
          {workflowDepth: Math.min(3, Math.max(1, depth)) as WorkflowDepth}),
      setShowFullSourceDump: (show) => set({showFullSourceDump: show}),
      setExplorerDropupOpen: (open) => set({explorerDropupOpen: open}),
      cycleWorkflowStep: (direction = 1) => set((state) => {
        const currentIndex = workflowOrder.indexOf(state.activeWorkflowStep);
        const nextIndex = (currentIndex + direction + workflowOrder.length) %
            workflowOrder.length;
        return {activeWorkflowStep: workflowOrder[nextIndex]};
      }),
      adjustWorkflowDepth: (delta) => set((state) => {
        const next = state.workflowDepth + delta;
        return {
          workflowDepth: Math.min(3, Math.max(1, next)) as WorkflowDepth,
        };
      }),
      applyHudPreset: (preset) => set({
        showExplorer: pickHudPanelFromPreset(preset) === 'explorer',
        showMenu: pickHudPanelFromPreset(preset) === 'menu',
        showStatus: pickHudPanelFromPreset(preset) === 'status',
        showOperations: pickHudPanelFromPreset(preset) === 'operations',
        showVisualizations: pickHudPanelFromPreset(preset) === 'visualizations',
        showTraining: pickHudPanelFromPreset(preset) === 'training',
        explorerDropupOpen: false,
      }),
      setHudPanelVisibility: (panel, visible) => set(
          (state) => ({
            showExplorer: panel === 'explorer' ? visible : state.showExplorer,
            showMenu: panel === 'menu' ? visible : state.showMenu,
            showStatus: panel === 'status' ? visible : state.showStatus,
            showOperations: panel === 'operations' ? visible :
                                                     state.showOperations,
            showVisualizations:
                panel === 'visualizations' ? visible : state.showVisualizations,
            showTraining: panel === 'training' ? visible : state.showTraining,
          })),
      toggleHudPanel: (panel) =>
          set((state) => ({
                showExplorer: panel === 'explorer' ? !state.showExplorer :
                                                     state.showExplorer,
                showMenu: panel === 'menu' ? !state.showMenu : state.showMenu,
                showStatus: panel === 'status' ? !state.showStatus :
                                                 state.showStatus,
                showOperations: panel === 'operations' ? !state.showOperations :
                                                         state.showOperations,
                showVisualizations: panel === 'visualizations' ?
                    !state.showVisualizations :
                    state.showVisualizations,
                showTraining: panel === 'training' ? !state.showTraining :
                                                     state.showTraining,
                explorerDropupOpen: false,
              })),
      closeAllHudPanels: () => set((state) => ({
                                     lastHudSnapshot: {
                                       explorer: state.showExplorer,
                                       menu: state.showMenu,
                                       status: state.showStatus,
                                       operations: state.showOperations,
                                       visualizations: state.showVisualizations,
                                       training: state.showTraining,
                                     },
                                     showExplorer: false,
                                     showMenu: false,
                                     showStatus: false,
                                     showOperations: false,
                                     showVisualizations: false,
                                     showTraining: false,
                                     explorerDropupOpen: false,
                                   })),
      reopenHudPanels: () =>
          set((state) => ({
                showExplorer: state.lastHudSnapshot.explorer,
                showMenu: state.lastHudSnapshot.menu,
                showStatus: state.lastHudSnapshot.status,
                showOperations: state.lastHudSnapshot.operations,
                showVisualizations: state.lastHudSnapshot.visualizations,
                showTraining: state.lastHudSnapshot.training,
                explorerDropupOpen: false,
              })),
      setGameplayWindowVisibility: (window, visible) => set(
          (state) => ({
            showIntelNodeWindow:
                window === 'intelNode' ? visible : state.showIntelNodeWindow,
            showObjectiveNodeWindow: window === 'objectiveNode' ?
                visible :
                state.showObjectiveNodeWindow,
            showPlayerNodeWindow:
                window === 'playerNode' ? visible : state.showPlayerNodeWindow,
            showQuestLogWindow:
                window === 'questLog' ? visible : state.showQuestLogWindow,
            showNodeOpsWindow: window === 'nodeOps' ? visible :
                                                      state.showNodeOpsWindow,
          })),
      closeAllGameplayWindows: () =>
          set((state) => ({
                lastGameplayWindowSnapshot: {
                  intelNode: state.showIntelNodeWindow,
                  objectiveNode: state.showObjectiveNodeWindow,
                  playerNode: state.showPlayerNodeWindow,
                  questLog: state.showQuestLogWindow,
                  nodeOps: state.showNodeOpsWindow,
                },
                lastGameplayDockSnapshot: state.activeDock,
                activeDock: null,
                showIntelNodeWindow: false,
                showObjectiveNodeWindow: false,
                showPlayerNodeWindow: false,
                showQuestLogWindow: false,
                showNodeOpsWindow: false,
              })),
      reopenGameplayWindows: () => set(
          (state) => ({
            activeDock: state.lastGameplayDockSnapshot,
            showIntelNodeWindow: state.lastGameplayWindowSnapshot.intelNode,
            showObjectiveNodeWindow:
                state.lastGameplayWindowSnapshot.objectiveNode,
            showPlayerNodeWindow: state.lastGameplayWindowSnapshot.playerNode,
            showQuestLogWindow: state.lastGameplayWindowSnapshot.questLog,
            showNodeOpsWindow: state.lastGameplayWindowSnapshot.nodeOps,
          })),
      focusHudPanel: (panel) =>
          set(() => ({
                showExplorer: panel === 'explorer',
                showMenu: panel === 'menu',
                showStatus: panel === 'status',
                showOperations: panel === 'operations',
                showVisualizations: panel === 'visualizations',
                showTraining: panel === 'training',
                explorerDropupOpen: false,
              })),
      resetHudPanels: () => set({
        showExplorer: true,
        showMenu: false,
        showStatus: false,
        showOperations: false,
        showVisualizations: false,
        showTraining: false,
        explorerDropupOpen: false,
      }),
      resetForSector: (hasSelection) => set({
        activeDock: hasSelection ? 'intel' : 'objectives',
        intelPanel: 'threat',
        objectivePanel: 'board',
        playerPanel: 'rank',
        activeWorkflowStep: 'scan',
        workflowDepth: 1,
        showFullSourceDump: false,
      }),
    }));
