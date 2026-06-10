export type NotebookRouteMode =|'runtime'|'auth'|'marketing'|'portal'|'session'|
    'engine'|'menu'|'character';

export type RouteHudPreset = {
  mode: NotebookRouteMode; label: string; explorer: boolean; menu: boolean;
  status: boolean;
  operations: boolean;
};

export type RouteModeDescriptor = {
  title: string; description: string; badge: string;
  actions: Array<{label: string; href: string; primary?: boolean;}>;
};

export const routeModeDescriptors: Record<
    NotebookRouteMode, RouteModeDescriptor> = {
  runtime: {
    title: 'Runtime Deck',
    description:
        'Live notebook orchestration with explorer, diagnostics, and operations engaged.',
    badge: 'Runtime',
    actions: [
      {label: 'Notebook Manifest', href: '/notebooks/catalog-index.json'},
      {
        label: 'Notebook Payload',
        href: '/notebooks/native-c-catalog.ipynb',
        primary: true
      },
    ],
  },
  auth: {
    title: 'Authentication Deck',
    description:
        'Credential and session touchpoints are consolidated into this shell mode.',
    badge: 'Authentication',
    actions: [
      {label: 'Open Main Runtime', href: '/notebooks', primary: true},
      {label: 'Account Portal', href: '/account'},
    ],
  },
  marketing: {
    title: 'Campaign Deck',
    description:
        'Marketing route flows now project into the notebooks shell without page swaps.',
    badge: 'Marketing',
    actions: [
      {label: 'Return to Runtime', href: '/notebooks', primary: true},
      {label: 'Portal Route', href: '/download'},
    ],
  },
  portal: {
    title: 'Portal Deck',
    description:
        'Download and account aliases share one focused portal mode in this SPA.',
    badge: 'Portal',
    actions: [
      {label: 'Runtime Surface', href: '/notebooks', primary: true},
      {label: 'Login Alias', href: '/login'},
    ],
  },
  session: {
    title: 'Session Deck',
    description:
        'Session-room route behavior is staged inside the same notebook viewport shell.',
    badge: 'Session',
    actions: [
      {label: 'Open Runtime', href: '/notebooks', primary: true},
      {label: 'Main Menu Alias', href: '/game-main-menu'},
    ],
  },
  engine: {
    title: 'Engine Boot Deck',
    description:
        'Engine bootstrap aliases remain inside the same orchestration shell.',
    badge: 'Engine',
    actions: [
      {label: 'Runtime View', href: '/notebooks', primary: true},
      {label: 'Character Select', href: '/select-character'},
    ],
  },
  menu: {
    title: 'Main Menu Deck',
    description:
        'Main menu navigation is modeled as route-state in the notebooks shell.',
    badge: 'Main Menu',
    actions: [
      {label: 'Enter Runtime', href: '/notebooks', primary: true},
      {label: 'Login Alias', href: '/login'},
    ],
  },
  character: {
    title: 'Character Deck',
    description:
        'Character selection now maps to an in-shell mode for flow continuity.',
    badge: 'Character',
    actions: [
      {label: 'Open Runtime', href: '/notebooks', primary: true},
      {label: 'Main Menu Alias', href: '/game-main-menu'},
    ],
  },
};

export function resolveRouteHudPreset(pathname: string): RouteHudPreset {
  if (pathname === '/login') {
    return {
      mode: 'auth',
      label: 'Authentication',
      explorer: false,
      menu: true,
      status: true,
      operations: false
    };
  }

  if (pathname === '/marketing') {
    return {
      mode: 'marketing',
      label: 'Marketing',
      explorer: false,
      menu: true,
      status: true,
      operations: false
    };
  }

  if (pathname === '/download' || pathname === '/account') {
    return {
      mode: 'portal',
      label: 'Portal',
      explorer: false,
      menu: true,
      status: true,
      operations: false
    };
  }

  if (pathname === '/session-room') {
    return {
      mode: 'session',
      label: 'Session',
      explorer: false,
      menu: true,
      status: true,
      operations: true
    };
  }

  if (pathname === '/banana-engine') {
    return {
      mode: 'engine',
      label: 'Engine Boot',
      explorer: true,
      menu: true,
      status: false,
      operations: true
    };
  }

  if (pathname === '/game-main-menu') {
    return {
      mode: 'menu',
      label: 'Main Menu',
      explorer: false,
      menu: true,
      status: true,
      operations: false
    };
  }

  if (pathname === '/select-character') {
    return {
      mode: 'character',
      label: 'Character Select',
      explorer: true,
      menu: true,
      status: false,
      operations: false
    };
  }

  return {
    mode: 'runtime',
    label: 'Runtime',
    explorer: true,
    menu: true,
    status: true,
    operations: true
  };
}