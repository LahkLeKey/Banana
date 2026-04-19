import * as React from "react";
import {
    StyleSheet,
    Text,
    View,
    type StyleProp,
    type TextStyle,
    type ViewProps,
    type ViewStyle
} from "react-native";

import { bananaUiTheme } from "../../theme";

export type BadgeVariant = "default" | "secondary" | "outline" | "success" | "warning" | "destructive";

export interface BadgeProps extends Omit<ViewProps, "style"> {
    children?: React.ReactNode;
    variant?: BadgeVariant;
    style?: StyleProp<ViewStyle>;
    textStyle?: StyleProp<TextStyle>;
    className?: string;
}

const variantContainerStyles: Record<BadgeVariant, ViewStyle> = {
    default: {
        backgroundColor: bananaUiTheme.colors.primary
    },
    secondary: {
        backgroundColor: bananaUiTheme.colors.secondary
    },
    outline: {
        backgroundColor: "transparent",
        borderColor: bananaUiTheme.colors.border,
        borderWidth: 1
    },
    success: {
        backgroundColor: "#0E8A54"
    },
    warning: {
        backgroundColor: "#F2B01E"
    },
    destructive: {
        backgroundColor: bananaUiTheme.colors.destructive
    }
};

const variantTextStyles: Record<BadgeVariant, TextStyle> = {
    default: {
        color: bananaUiTheme.colors.primaryForeground
    },
    secondary: {
        color: bananaUiTheme.colors.secondaryForeground
    },
    outline: {
        color: bananaUiTheme.colors.foreground
    },
    success: {
        color: "#F7FFF6"
    },
    warning: {
        color: "#3F2600"
    },
    destructive: {
        color: bananaUiTheme.colors.destructiveForeground
    }
};

function Badge({ children, variant = "default", style, textStyle, className: _className, ...props }: BadgeProps): JSX.Element {
    const renderedContent =
        typeof children === "string" || typeof children === "number" ? (
            <Text style={[styles.textBase, variantTextStyles[variant], textStyle]}>{children}</Text>
        ) : (
            children
        );

    return (
        <View style={[styles.base, variantContainerStyles[variant], style]} {...props}>
            {renderedContent}
        </View>
    );
}

const styles = StyleSheet.create({
    base: {
        alignItems: "center",
        alignSelf: "flex-start",
        borderRadius: 999,
        flexDirection: "row",
        justifyContent: "center",
        minHeight: 24,
        paddingHorizontal: bananaUiTheme.spacing.md,
        paddingVertical: 2
    },
    textBase: {
        fontSize: 11,
        fontWeight: "700",
        textTransform: "uppercase"
    }
});

export { Badge };
