declare const _default: {
    presets: {
        darkMode: ["class"];
        theme: {
            container: {
                center: true;
                padding: string;
                screens: {
                    '2xl': string;
                };
            };
            extend: {
                colors: {
                    border: string;
                    input: string;
                    ring: string;
                    background: string;
                    foreground: string;
                    primary: {
                        DEFAULT: string;
                        foreground: string;
                    };
                    secondary: {
                        DEFAULT: string;
                        foreground: string;
                    };
                    destructive: {
                        DEFAULT: string;
                        foreground: string;
                    };
                    muted: {
                        DEFAULT: string;
                        foreground: string;
                    };
                    accent: {
                        DEFAULT: string;
                        foreground: string;
                    };
                    card: {
                        DEFAULT: string;
                        foreground: string;
                    };
                };
                borderRadius: {
                    lg: string;
                    md: string;
                    sm: string;
                };
                boxShadow: {
                    panel: string;
                };
            };
        };
        plugins: any[];
    }[];
    content: string[];
    theme: {
        extend: {
            fontFamily: {
                display: [string, string, string];
                body: [string, string, string];
            };
            boxShadow: {
                panel: string;
            };
        };
    };
    plugins: any[];
};
export default _default;
