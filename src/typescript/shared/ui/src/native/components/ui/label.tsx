import * as React from "react";
import {
    StyleSheet,
    Text,
    type StyleProp,
    type TextProps,
    type TextStyle
} from "react-native";

import { bananaUiTheme } from "../../theme";

export interface LabelProps extends Omit<TextProps, "style"> {
    style?: StyleProp<TextStyle>;
    className?: string;
}

const Label = React.forwardRef<Text, LabelProps>(({ style, className: _className, ...props }, ref) => (
    <Text ref={ref} style={[styles.base, style]} {...props} />
));
Label.displayName = "Label";

const styles = StyleSheet.create({
    base: {
        color: bananaUiTheme.colors.mutedForeground,
        fontSize: 12,
        fontWeight: "700",
        letterSpacing: 0.7,
        textTransform: "uppercase"
    }
});

export { Label };
