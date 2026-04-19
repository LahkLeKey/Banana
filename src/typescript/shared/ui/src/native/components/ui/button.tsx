import * as React from "react";
import {
    Pressable,
    StyleSheet,
    Text,
    type PressableProps,
    type StyleProp,
    type TextStyle,
    type ViewStyle
} from "react-native";

import { bananaUiTheme } from "../../theme";

export type ButtonVariant = "default" | "secondary" | "outline" | "ghost";
export type ButtonSize = "default" | "sm" | "lg";

export interface ButtonProps extends Omit<PressableProps, "style"> {
    children?: React.ReactNode;
    variant?: ButtonVariant;
    size?: ButtonSize;
    style?: StyleProp<ViewStyle>;
    textStyle?: StyleProp<TextStyle>;
    className?: string;
}

const variantContainerStyles: Record<ButtonVariant, ViewStyle> = {
    default: {
        backgroundColor: bananaUiTheme.colors.primary
    },
    secondary: {
        backgroundColor: bananaUiTheme.colors.secondary
    },
    outline: {
        backgroundColor: bananaUiTheme.colors.card,
        borderColor: bananaUiTheme.colors.input,
        borderWidth: 1
    },
    ghost: {
        backgroundColor: "transparent"
    }
};

const variantTextStyles: Record<ButtonVariant, TextStyle> = {
    default: {
        color: bananaUiTheme.colors.primaryForeground
    },
    secondary: {
        color: bananaUiTheme.colors.secondaryForeground
    },
    outline: {
        color: bananaUiTheme.colors.foreground
    },
    ghost: {
        color: bananaUiTheme.colors.foreground
    }
};

const sizeStyles: Record<ButtonSize, ViewStyle> = {
    default: {
        minHeight: 40,
        paddingHorizontal: bananaUiTheme.spacing.lg,
        paddingVertical: bananaUiTheme.spacing.sm
    },
    sm: {
        minHeight: 36,
        paddingHorizontal: bananaUiTheme.spacing.md,
        paddingVertical: bananaUiTheme.spacing.xs
    },
    lg: {
        minHeight: 44,
        paddingHorizontal: bananaUiTheme.spacing.xl,
        paddingVertical: bananaUiTheme.spacing.sm
    }
};

function Button({
    children,
    variant = "default",
    size = "default",
    style,
    textStyle,
    disabled,
    className: _className,
    ...props
}: ButtonProps): JSX.Element {
    const renderedContent =
        typeof children === "string" || typeof children === "number" ? (
            <Text style={[styles.textBase, variantTextStyles[variant], textStyle]}>{children}</Text>
        ) : (
            children
        );

    return (
        <Pressable
            accessibilityRole={props.accessibilityRole ?? "button"}
            disabled={disabled}
            style={({ pressed }) => [
                styles.base,
                sizeStyles[size],
                variantContainerStyles[variant],
                pressed && !disabled ? styles.pressed : undefined,
                disabled ? styles.disabled : undefined,
                style
            ]}
            {...props}
        >
            {renderedContent}
        </Pressable>
    );
}

const styles = StyleSheet.create({
    base: {
        alignItems: "center",
        borderRadius: bananaUiTheme.radius.md,
        flexDirection: "row",
        justifyContent: "center"
    },
    textBase: {
        fontSize: 14,
        fontWeight: "600"
    },
    pressed: {
        opacity: 0.9,
        transform: [{ scale: 0.99 }]
    },
    disabled: {
        opacity: 0.55
    }
});

export { Button };
