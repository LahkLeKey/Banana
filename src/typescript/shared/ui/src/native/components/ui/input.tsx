import * as React from "react";
import {
    StyleSheet,
    TextInput,
    type StyleProp,
    type TextInputProps,
    type TextStyle
} from "react-native";

import { bananaUiTheme } from "../../theme";

export interface InputProps extends Omit<TextInputProps, "style"> {
    style?: StyleProp<TextStyle>;
    className?: string;
}

const Input = React.forwardRef<TextInput, InputProps>(({ style, className: _className, ...props }, ref) => (
    <TextInput
        placeholderTextColor={bananaUiTheme.colors.mutedForeground}
        ref={ref}
        selectionColor={bananaUiTheme.colors.ring}
        style={[styles.base, style]}
        {...props}
    />
));
Input.displayName = "Input";

const styles = StyleSheet.create({
    base: {
        backgroundColor: bananaUiTheme.colors.background,
        borderColor: bananaUiTheme.colors.input,
        borderRadius: bananaUiTheme.radius.md,
        borderWidth: 1,
        color: bananaUiTheme.colors.foreground,
        fontSize: 14,
        minHeight: 42,
        paddingHorizontal: bananaUiTheme.spacing.md,
        paddingVertical: bananaUiTheme.spacing.sm
    }
});

export { Input };
