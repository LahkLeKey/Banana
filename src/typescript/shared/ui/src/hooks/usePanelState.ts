import {useCallback, useState} from 'react';

export type PanelState = {
  readonly isCollapsed: boolean; readonly isExpanded: boolean; readonly isLoading: boolean; readonly error: string | null; readonly activeTab:
                                                                                                                                        string;
};

export type PanelStateActions = {
  readonly toggleCollapse: () => void; readonly expand: () => void; readonly collapse: () => void; readonly setLoading: (loading: boolean) => void; readonly setError: (error: string|null) => void; readonly setActiveTab: (
                                                                                                                                                                                                                  tab:
                                                                                                                                                                                                                      string) =>
                                                                                                                                                                                                                  void;
};

/**
 * usePanelState provides common state management for resizable/collapsible
 * panels. Handles collapse/expand, loading, error, and tab management.
 */
export function usePanelState(initialTab: string = ''): PanelState&
    PanelStateActions {
  const [isCollapsed, setIsCollapsed] = useState(false);
  const [isExpanded, setIsExpanded] = useState(false);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string|null>(null);
  const [activeTab, setActiveTab] = useState(initialTab);

  const toggleCollapse = useCallback(() => {
    setIsCollapsed((prev) => !prev);
  }, []);

  const expand = useCallback(() => {
    setIsExpanded(true);
    setIsCollapsed(false);
  }, []);

  const collapse = useCallback(() => {
    setIsCollapsed(true);
    setIsExpanded(false);
  }, []);

  const handleSetLoading = useCallback((loading: boolean) => {
    setIsLoading(loading);
  }, []);

  const handleSetError = useCallback((err: string|null) => {
    setError(err);
  }, []);

  const handleSetActiveTab = useCallback((tab: string) => {
    setActiveTab(tab);
  }, []);

  return {
    isCollapsed,
    isExpanded,
    isLoading,
    error,
    activeTab,
    toggleCollapse,
    expand,
    collapse,
    setLoading: handleSetLoading,
    setError: handleSetError,
    setActiveTab: handleSetActiveTab,
  };
}
