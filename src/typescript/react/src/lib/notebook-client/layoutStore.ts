import {create} from 'zustand';

import type {WorkflowDepth, WorkflowStepId} from '../../domain/notebook/workflow-domain';

export type ActiveDock = 'intel'|'objectives'|'player'|null;
export type IntelPanel = 'threat'|'signal'|'route';
export type ObjectivePanel = 'board'|'timeline';
export type PlayerPanel = 'rank'|'economy'|'streak';

const workflowOrder: WorkflowStepId[] = ['scan', 'shape', 'commit'];

function pickHudPanelFromPreset(preset: {
  explorer: boolean; menu: boolean; status: boolean; operations: boolean;
}): 'explorer'|'menu'|'status'|'operations'|null {
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
  }) => void;
  focusHudPanel: (panel: 'explorer'|'menu'|'status'|'operations'|null) => void;
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
      showExplorer: true,
      showMenu: true,
      showStatus: true,
      showOperations: true,
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
        explorerDropupOpen: false,
      }),
      focusHudPanel: (panel) => set(() => ({
                                      showExplorer: panel === 'explorer',
                                      showMenu: panel === 'menu',
                                      showStatus: panel === 'status',
                                      showOperations: panel === 'operations',
                                      explorerDropupOpen: false,
                                    })),
      resetHudPanels: () => set({
        showExplorer: true,
        showMenu: false,
        showStatus: false,
        showOperations: false,
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
