import * as React from "react";
import { cva } from "class-variance-authority";

import { cn } from "../../lib/utils";

const labelVariants = cva("text-xs font-semibold uppercase tracking-[0.12em] text-muted-foreground");

const Label = React.forwardRef<HTMLLabelElement, React.ComponentPropsWithoutRef<"label">>(({ className, ...props }, ref) => (
    <label ref={ref} className={cn(labelVariants(), className)} {...props} />
));
Label.displayName = "Label";

export { Label };
