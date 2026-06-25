const panelTailwindPreset = {
  theme: {
    extend: {
      colors: {
        panel: {
          surface:
              'rgb(var(--banana-panel-surface-rgb, 7 19 34) / <alpha-value>)',
          border:
              'rgb(var(--banana-panel-border-rgb, 20 184 166) / <alpha-value>)',
          accent: 'var(--banana-panel-accent, #22d3ee)',
          content:
              'rgb(var(--banana-panel-content-rgb, 226 232 240) / <alpha-value>)',
        },
      },
      boxShadow: {
        panel: '0 20px 60px rgba(0, 0, 0, 0.8)',
      },
    },
  },
};

export default panelTailwindPreset;
