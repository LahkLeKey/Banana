export type PanelThemeName =|'deep-space'|'sunset-grid'|'mint-circuit'|
    'aurora-grid-light'|'aurora-grid-dark'|'cobalt-haze-light'|
    'cobalt-haze-dark'|'obsidian-frost-light'|'obsidian-frost-dark'|
    'tide-glass-light'|'tide-glass-dark';

export type PanelThemeCategory =
    |'spectrum-lab'|'aurora-grid'|'cobalt-haze'|'obsidian-frost'|'tide-glass';

export type PanelThemeMode = 'light'|'dark';

export type PanelThemeTokens = {
  readonly surfaceRgb: string; readonly borderRgb: string; readonly contentRgb: string; readonly accent: string; readonly desktopGradient: string; readonly headerBackground: string; readonly taskbarBackground: string; readonly chromeBorder:
                                                                                                                                                                                                                                       string;
};

export type PanelThemeMeta = {
  readonly name: PanelThemeName; readonly label: string; readonly category: PanelThemeCategory; readonly mode:
                                                                                                             PanelThemeMode;
};

type PanelThemeDefinition = PanelThemeMeta&{
  readonly tokens: PanelThemeTokens;
};

export const PANEL_THEME_VARIABLES = {
  surfaceRgb: '--banana-panel-surface-rgb',
  borderRgb: '--banana-panel-border-rgb',
  contentRgb: '--banana-panel-content-rgb',
  accent: '--banana-panel-accent',
  desktopGradient: '--banana-panel-desktop-gradient',
  headerBackground: '--banana-panel-header-background',
  taskbarBackground: '--banana-panel-taskbar-background',
  chromeBorder: '--banana-panel-chrome-border',
  fontFamily: '--banana-panel-font-family',
  headerTextColor: '--banana-panel-header-text-color',
  subtleTextColor: '--banana-panel-subtle-text-color',
} as const;

const PANEL_THEME_TYPography: Record < PanelThemeName, {
  readonly fontFamily: string;
  readonly headerTextColor: string;
  readonly subtleTextColor: string;
}
> = {
  'deep-space': {
    fontFamily: '"IBM Plex Sans", "Segoe UI", system-ui, sans-serif',
    headerTextColor: 'rgb(226 232 240)',
    subtleTextColor: 'rgb(148 163 184)',
  },
  'sunset-grid': {
    fontFamily: '"Trebuchet MS", "Segoe UI", system-ui, sans-serif',
    headerTextColor: 'rgb(255 237 213)',
    subtleTextColor: 'rgb(251 191 153)',
  },
  'mint-circuit': {
    fontFamily: '"Manrope", "Segoe UI", system-ui, sans-serif',
    headerTextColor: 'rgb(204 251 241)',
    subtleTextColor: 'rgb(110 231 183)',
  },
  'aurora-grid-light': {
    fontFamily: '"Source Sans 3", "Segoe UI", system-ui, sans-serif',
    headerTextColor: 'rgb(15 23 42)',
    subtleTextColor: 'rgb(71 85 105)',
  },
  'aurora-grid-dark': {
    fontFamily: '"Source Sans 3", "Segoe UI", system-ui, sans-serif',
    headerTextColor: 'rgb(226 232 240)',
    subtleTextColor: 'rgb(148 163 184)',
  },
  'cobalt-haze-light': {
    fontFamily: '"Aptos", "Segoe UI", system-ui, sans-serif',
    headerTextColor: 'rgb(17 24 39)',
    subtleTextColor: 'rgb(75 85 99)',
  },
  'cobalt-haze-dark': {
    fontFamily: '"Aptos", "Segoe UI", system-ui, sans-serif',
    headerTextColor: 'rgb(241 245 249)',
    subtleTextColor: 'rgb(148 163 184)',
  },
  'obsidian-frost-light': {
    fontFamily: '"Georgia", "Times New Roman", serif',
    headerTextColor: 'rgb(15 23 42)',
    subtleTextColor: 'rgb(82 82 91)',
  },
  'obsidian-frost-dark': {
    fontFamily: '"Georgia", "Times New Roman", serif',
    headerTextColor: 'rgb(244 244 245)',
    subtleTextColor: 'rgb(161 161 170)',
  },
  'tide-glass-light': {
    fontFamily: '"Calibri", "Segoe UI", system-ui, sans-serif',
    headerTextColor: 'rgb(17 24 39)',
    subtleTextColor: 'rgb(71 85 105)',
  },
  'tide-glass-dark': {
    fontFamily: '"Calibri", "Segoe UI", system-ui, sans-serif',
    headerTextColor: 'rgb(226 232 240)',
    subtleTextColor: 'rgb(148 163 184)',
  },
};

export const PANEL_THEME_STYLES = {
  deepSpace: {
    surfaceRgb: '7 19 34',
    borderRgb: '45 212 191',
    contentRgb: '226 232 240',
    accent: '#22d3ee',
    desktopGradient:
        'radial-gradient(circle at 20% 0%, rgba(14,165,233,0.24), transparent 38%), radial-gradient(circle at 90% 10%, rgba(16,185,129,0.15), transparent 34%), repeating-linear-gradient(0deg, rgba(148,163,184,0.05) 0 1px, transparent 1px 36px), repeating-linear-gradient(90deg, rgba(148,163,184,0.05) 0 1px, transparent 1px 36px), linear-gradient(180deg, #071122 0%, #0b1b31 100%)',
    headerBackground: 'rgba(2, 6, 23, 0.5)',
    taskbarBackground: 'rgba(2, 6, 23, 0.62)',
    chromeBorder: 'rgba(56, 189, 248, 0.22)',
  },
  sunsetGrid: {
    surfaceRgb: '36 18 32',
    borderRgb: '251 146 60',
    contentRgb: '255 237 213',
    accent: '#fb7185',
    desktopGradient:
        'radial-gradient(circle at 18% 0%, rgba(251,146,60,0.30), transparent 42%), radial-gradient(circle at 84% 6%, rgba(244,63,94,0.22), transparent 36%), repeating-linear-gradient(0deg, rgba(251,146,60,0.07) 0 1px, transparent 1px 32px), repeating-linear-gradient(90deg, rgba(251,146,60,0.07) 0 1px, transparent 1px 32px), linear-gradient(180deg, #2a1327 0%, #3f1d2f 45%, #4a261f 100%)',
    headerBackground: 'rgba(35, 10, 20, 0.58)',
    taskbarBackground: 'rgba(30, 8, 15, 0.7)',
    chromeBorder: 'rgba(251, 146, 60, 0.32)',
  },
  mintCircuit: {
    surfaceRgb: '6 31 30',
    borderRgb: '45 212 191',
    contentRgb: '204 251 241',
    accent: '#2dd4bf',
    desktopGradient:
        'radial-gradient(circle at 16% 4%, rgba(45,212,191,0.24), transparent 36%), radial-gradient(circle at 86% 0%, rgba(34,197,94,0.22), transparent 36%), repeating-linear-gradient(0deg, rgba(45,212,191,0.06) 0 1px, transparent 1px 34px), repeating-linear-gradient(90deg, rgba(45,212,191,0.06) 0 1px, transparent 1px 34px), linear-gradient(180deg, #082827 0%, #07312d 100%)',
    headerBackground: 'rgba(3, 20, 20, 0.56)',
    taskbarBackground: 'rgba(3, 20, 20, 0.66)',
    chromeBorder: 'rgba(45, 212, 191, 0.28)',
  },
  auroraGridLight: {
    surfaceRgb: '248 250 252',
    borderRgb: '148 163 184',
    contentRgb: '15 23 42',
    accent: '#2563eb',
    desktopGradient:
        'radial-gradient(circle at 20% 0%, rgba(186,230,253,0.4), transparent 42%), radial-gradient(circle at 80% 10%, rgba(226,232,240,0.6), transparent 36%), repeating-linear-gradient(0deg, rgba(148,163,184,0.08) 0 1px, transparent 1px 36px), repeating-linear-gradient(90deg, rgba(148,163,184,0.08) 0 1px, transparent 1px 36px), linear-gradient(180deg, #f8fafc 0%, #e2e8f0 100%)',
    headerBackground: 'rgba(255, 255, 255, 0.84)',
    taskbarBackground: 'rgba(241, 245, 249, 0.9)',
    chromeBorder: 'rgba(148, 163, 184, 0.6)',
  },
  auroraGridDark: {
    surfaceRgb: '15 23 42',
    borderRgb: '71 85 105',
    contentRgb: '226 232 240',
    accent: '#60a5fa',
    desktopGradient:
        'radial-gradient(circle at 20% 0%, rgba(37,99,235,0.18), transparent 42%), radial-gradient(circle at 85% 0%, rgba(30,41,59,0.46), transparent 36%), repeating-linear-gradient(0deg, rgba(100,116,139,0.08) 0 1px, transparent 1px 34px), repeating-linear-gradient(90deg, rgba(100,116,139,0.08) 0 1px, transparent 1px 34px), linear-gradient(180deg, #0f172a 0%, #111827 100%)',
    headerBackground: 'rgba(15, 23, 42, 0.84)',
    taskbarBackground: 'rgba(2, 6, 23, 0.82)',
    chromeBorder: 'rgba(71, 85, 105, 0.62)',
  },
  cobaltHazeLight: {
    surfaceRgb: '255 255 255',
    borderRgb: '203 213 225',
    contentRgb: '17 24 39',
    accent: '#1976d2',
    desktopGradient:
        'radial-gradient(circle at 16% 0%, rgba(147,197,253,0.32), transparent 40%), radial-gradient(circle at 82% 8%, rgba(224,242,254,0.58), transparent 34%), linear-gradient(180deg, #ffffff 0%, #f8fafc 100%)',
    headerBackground: 'rgba(255, 255, 255, 0.9)',
    taskbarBackground: 'rgba(248, 250, 252, 0.94)',
    chromeBorder: 'rgba(148, 163, 184, 0.55)',
  },
  cobaltHazeDark: {
    surfaceRgb: '18 18 18',
    borderRgb: '63 81 181',
    contentRgb: '241 245 249',
    accent: '#90caf9',
    desktopGradient:
        'radial-gradient(circle at 20% 0%, rgba(144,202,249,0.18), transparent 38%), radial-gradient(circle at 84% 8%, rgba(30,41,59,0.52), transparent 34%), linear-gradient(180deg, #121212 0%, #1f2937 100%)',
    headerBackground: 'rgba(23, 23, 23, 0.86)',
    taskbarBackground: 'rgba(10, 10, 10, 0.84)',
    chromeBorder: 'rgba(99, 102, 241, 0.52)',
  },
  obsidianFrostLight: {
    surfaceRgb: '255 255 255',
    borderRgb: '214 211 209',
    contentRgb: '15 23 42',
    accent: '#0f172a',
    desktopGradient:
        'radial-gradient(circle at 18% 0%, rgba(226,232,240,0.66), transparent 40%), radial-gradient(circle at 82% 10%, rgba(241,245,249,0.86), transparent 34%), linear-gradient(180deg, #ffffff 0%, #f8fafc 100%)',
    headerBackground: 'rgba(255, 255, 255, 0.9)',
    taskbarBackground: 'rgba(248, 250, 252, 0.92)',
    chromeBorder: 'rgba(214, 211, 209, 0.72)',
  },
  obsidianFrostDark: {
    surfaceRgb: '9 9 11',
    borderRgb: '63 63 70',
    contentRgb: '244 244 245',
    accent: '#fafafa',
    desktopGradient:
        'radial-gradient(circle at 16% 0%, rgba(82,82,91,0.18), transparent 42%), radial-gradient(circle at 84% 10%, rgba(24,24,27,0.56), transparent 34%), linear-gradient(180deg, #09090b 0%, #18181b 100%)',
    headerBackground: 'rgba(9, 9, 11, 0.86)',
    taskbarBackground: 'rgba(9, 9, 11, 0.9)',
    chromeBorder: 'rgba(63, 63, 70, 0.62)',
  },
  tideGlassLight: {
    surfaceRgb: '248 250 252',
    borderRgb: '148 163 184',
    contentRgb: '17 24 39',
    accent: '#0078d4',
    desktopGradient:
        'radial-gradient(circle at 18% 0%, rgba(191,219,254,0.38), transparent 40%), radial-gradient(circle at 86% 10%, rgba(186,230,253,0.46), transparent 34%), linear-gradient(180deg, #f8fafc 0%, #e2e8f0 100%)',
    headerBackground: 'rgba(255, 255, 255, 0.88)',
    taskbarBackground: 'rgba(241, 245, 249, 0.94)',
    chromeBorder: 'rgba(148, 163, 184, 0.58)',
  },
  tideGlassDark: {
    surfaceRgb: '20 23 34',
    borderRgb: '51 65 85',
    contentRgb: '226 232 240',
    accent: '#60cdff',
    desktopGradient:
        'radial-gradient(circle at 16% 0%, rgba(96,205,255,0.2), transparent 42%), radial-gradient(circle at 84% 10%, rgba(30,41,59,0.52), transparent 34%), linear-gradient(180deg, #141722 0%, #111827 100%)',
    headerBackground: 'rgba(20, 23, 34, 0.86)',
    taskbarBackground: 'rgba(15, 23, 42, 0.84)',
    chromeBorder: 'rgba(51, 65, 85, 0.66)',
  },
} satisfies Record<string, PanelThemeTokens>;

const themeCatalog: Record<PanelThemeName, PanelThemeDefinition> = {
  'deep-space': {
    name: 'deep-space',
    label: 'Deep Space',
    category: 'spectrum-lab',
    mode: 'dark',
    tokens: PANEL_THEME_STYLES.deepSpace,
  },
  'sunset-grid': {
    name: 'sunset-grid',
    label: 'Sunset Grid',
    category: 'spectrum-lab',
    mode: 'dark',
    tokens: PANEL_THEME_STYLES.sunsetGrid,
  },
  'mint-circuit': {
    name: 'mint-circuit',
    label: 'Mint Circuit',
    category: 'spectrum-lab',
    mode: 'dark',
    tokens: PANEL_THEME_STYLES.mintCircuit,
  },
  'aurora-grid-light': {
    name: 'aurora-grid-light',
    label: 'Aurora Grid Light',
    category: 'aurora-grid',
    mode: 'light',
    tokens: PANEL_THEME_STYLES.auroraGridLight,
  },
  'aurora-grid-dark': {
    name: 'aurora-grid-dark',
    label: 'Aurora Grid Dark',
    category: 'aurora-grid',
    mode: 'dark',
    tokens: PANEL_THEME_STYLES.auroraGridDark,
  },
  'cobalt-haze-light': {
    name: 'cobalt-haze-light',
    label: 'Cobalt Haze Light',
    category: 'cobalt-haze',
    mode: 'light',
    tokens: PANEL_THEME_STYLES.cobaltHazeLight,
  },
  'cobalt-haze-dark': {
    name: 'cobalt-haze-dark',
    label: 'Cobalt Haze Dark',
    category: 'cobalt-haze',
    mode: 'dark',
    tokens: PANEL_THEME_STYLES.cobaltHazeDark,
  },
  'obsidian-frost-light': {
    name: 'obsidian-frost-light',
    label: 'Obsidian Frost Light',
    category: 'obsidian-frost',
    mode: 'light',
    tokens: PANEL_THEME_STYLES.obsidianFrostLight,
  },
  'obsidian-frost-dark': {
    name: 'obsidian-frost-dark',
    label: 'Obsidian Frost Dark',
    category: 'obsidian-frost',
    mode: 'dark',
    tokens: PANEL_THEME_STYLES.obsidianFrostDark,
  },
  'tide-glass-light': {
    name: 'tide-glass-light',
    label: 'Tide Glass Light',
    category: 'tide-glass',
    mode: 'light',
    tokens: PANEL_THEME_STYLES.tideGlassLight,
  },
  'tide-glass-dark': {
    name: 'tide-glass-dark',
    label: 'Tide Glass Dark',
    category: 'tide-glass',
    mode: 'dark',
    tokens: PANEL_THEME_STYLES.tideGlassDark,
  },
};

const builtinThemes: Record<PanelThemeName, PanelThemeTokens> =
    Object.fromEntries(
        Object.entries(themeCatalog)
            .map(([name, definition]) => [name, definition.tokens]),
        ) as Record<PanelThemeName, PanelThemeTokens>;

export function listPanelThemes(): readonly PanelThemeName[] {
  return Object.keys(builtinThemes) as PanelThemeName[];
}

export function listPanelThemeCategories(): readonly PanelThemeCategory[] {
  return [
    'aurora-grid', 'cobalt-haze', 'obsidian-frost', 'tide-glass', 'spectrum-lab'
  ];
}

export function listPanelThemesByCategory(category: PanelThemeCategory):
    readonly PanelThemeName[] {
  return listPanelThemes().filter(
      (name) => themeCatalog[name].category === category);
}

export function getPanelThemeMeta(name: PanelThemeName): PanelThemeMeta {
  const {tokens: _tokens, ...meta} = themeCatalog[name];
  return meta;
}

export function resolvePanelTheme(
    category: PanelThemeCategory, mode: PanelThemeMode): PanelThemeName {
  const exact = listPanelThemes().find(
      (name) => themeCatalog[name].category === category &&
          themeCatalog[name].mode === mode,
  );

  if (exact) {
    return exact;
  }

  const firstInCategory = listPanelThemes().find(
      (name) => themeCatalog[name].category === category);
  return firstInCategory ?? 'aurora-grid-light';
}

export function getPanelTheme(name: PanelThemeName): PanelThemeTokens {
  return builtinThemes[name];
}

export function applyPanelTheme(
    theme: PanelThemeName|PanelThemeTokens,
    target: HTMLElement = document.documentElement,
    ): void {
  const resolved = typeof theme === 'string' ? builtinThemes[theme] : theme;
  const typography =
      typeof theme === 'string' ? PANEL_THEME_TYPography[theme] : {
        fontFamily: '"Segoe UI", system-ui, sans-serif',
        headerTextColor: 'rgb(226 232 240)',
        subtleTextColor: 'rgb(148 163 184)',
      };

  target.style.setProperty(
      PANEL_THEME_VARIABLES.surfaceRgb, resolved.surfaceRgb);
  target.style.setProperty(PANEL_THEME_VARIABLES.borderRgb, resolved.borderRgb);
  target.style.setProperty(
      PANEL_THEME_VARIABLES.contentRgb, resolved.contentRgb);
  target.style.setProperty(PANEL_THEME_VARIABLES.accent, resolved.accent);
  target.style.setProperty(
      PANEL_THEME_VARIABLES.desktopGradient, resolved.desktopGradient);
  target.style.setProperty(
      PANEL_THEME_VARIABLES.headerBackground, resolved.headerBackground);
  target.style.setProperty(
      PANEL_THEME_VARIABLES.taskbarBackground, resolved.taskbarBackground);
  target.style.setProperty(
      PANEL_THEME_VARIABLES.chromeBorder, resolved.chromeBorder);
  target.style.setProperty(
      PANEL_THEME_VARIABLES.fontFamily, typography.fontFamily);
  target.style.setProperty(
      PANEL_THEME_VARIABLES.headerTextColor, typography.headerTextColor);
  target.style.setProperty(
      PANEL_THEME_VARIABLES.subtleTextColor, typography.subtleTextColor);
}

export function resetPanelTheme(target: HTMLElement = document.documentElement):
    void {
  target.style.removeProperty(PANEL_THEME_VARIABLES.surfaceRgb);
  target.style.removeProperty(PANEL_THEME_VARIABLES.borderRgb);
  target.style.removeProperty(PANEL_THEME_VARIABLES.contentRgb);
  target.style.removeProperty(PANEL_THEME_VARIABLES.accent);
  target.style.removeProperty(PANEL_THEME_VARIABLES.desktopGradient);
  target.style.removeProperty(PANEL_THEME_VARIABLES.headerBackground);
  target.style.removeProperty(PANEL_THEME_VARIABLES.taskbarBackground);
  target.style.removeProperty(PANEL_THEME_VARIABLES.chromeBorder);
  target.style.removeProperty(PANEL_THEME_VARIABLES.fontFamily);
  target.style.removeProperty(PANEL_THEME_VARIABLES.headerTextColor);
  target.style.removeProperty(PANEL_THEME_VARIABLES.subtleTextColor);
}
