import * as React from "react";
import {
    StyleSheet,
    Text,
    View,
    type StyleProp,
    type TextProps,
    type TextStyle,
    type ViewProps,
    type ViewStyle
} from "react-native";

import { bananaUiTheme } from "../../theme";

interface NativeViewProps extends Omit<ViewProps, "style"> {
    style?: StyleProp<ViewStyle>;
    className?: string;
}

interface NativeTextProps extends Omit<TextProps, "style"> {
    style?: StyleProp<TextStyle>;
    className?: string;
}

const Card = React.forwardRef<View, NativeViewProps>(({ style, className: _className, ...props }, ref) => (
    <View ref={ref} style={[styles.card, style]} {...props} />
));
Card.displayName = "Card";

const CardHeader = React.forwardRef<View, NativeViewProps>(({ style, className: _className, ...props }, ref) => (
    <View ref={ref} style={[styles.header, style]} {...props} />
));
CardHeader.displayName = "CardHeader";

const CardTitle = React.forwardRef<Text, NativeTextProps>(({ style, className: _className, ...props }, ref) => (
    <Text ref={ref} style={[styles.title, style]} {...props} />
));
CardTitle.displayName = "CardTitle";

const CardDescription = React.forwardRef<Text, NativeTextProps>(({ style, className: _className, ...props }, ref) => (
    <Text ref={ref} style={[styles.description, style]} {...props} />
));
CardDescription.displayName = "CardDescription";

const CardContent = React.forwardRef<View, NativeViewProps>(({ style, className: _className, ...props }, ref) => (
    <View ref={ref} style={[styles.content, style]} {...props} />
));
CardContent.displayName = "CardContent";

const styles = StyleSheet.create({
    card: {
        backgroundColor: bananaUiTheme.colors.card,
        borderColor: bananaUiTheme.colors.border,
        borderRadius: bananaUiTheme.radius.lg,
        borderWidth: 1,
        shadowColor: "#000000",
        shadowOffset: { width: 0, height: 8 },
        shadowOpacity: 0.07,
        shadowRadius: 16
    },
    header: {
        gap: bananaUiTheme.spacing.xs,
        paddingHorizontal: bananaUiTheme.spacing.xl,
        paddingTop: bananaUiTheme.spacing.xl,
        paddingBottom: bananaUiTheme.spacing.md
    },
    title: {
        color: bananaUiTheme.colors.foreground,
        fontSize: 22,
        fontWeight: "700"
    },
    description: {
        color: bananaUiTheme.colors.mutedForeground,
        fontSize: 14,
        lineHeight: 20
    },
    content: {
        paddingHorizontal: bananaUiTheme.spacing.xl,
        paddingBottom: bananaUiTheme.spacing.xl,
        paddingTop: bananaUiTheme.spacing.sm
    }
});

export { Card, CardContent, CardDescription, CardHeader, CardTitle };
