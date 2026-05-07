# Contract: shadcn Baseline (048)

This contract is internal to the React app (`src/typescript/react`) and does not change any HTTP, IPC, or `@banana/ui` contract.

## Component contracts (`src/components/ui/`)

### Button

```ts
type ButtonVariant = "default" | "destructive" | "outline" | "ghost" | "link";
type ButtonSize = "default" | "sm" | "lg" | "icon";

interface ButtonProps extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  variant?: ButtonVariant;
  size?: ButtonSize;
  asChild?: boolean;
}
```

Defaults: `variant="default"`, `size="default"`, `type` defaults to `"button"` (callers must opt into `type="submit"`).

### Input

```ts
interface InputProps extends React.InputHTMLAttributes<HTMLInputElement> {}
```

Always renders a visible bordered input even when `value` is empty.

### Textarea

```ts
interface TextareaProps extends React.TextareaHTMLAttributes<HTMLTextAreaElement> {}
```

### Card / CardHeader / CardTitle / CardContent / CardFooter

```ts
interface CardProps extends React.HTMLAttributes<HTMLDivElement> {}
```

`Card` always renders a bordered, non-empty container with `data-slot="card"` for tests.

### Alert / AlertTitle / AlertDescription

```ts
type AlertVariant = "default" | "destructive";
interface AlertProps extends React.HTMLAttributes<HTMLDivElement> {
  variant?: AlertVariant;
}
```

## ErrorBoundary contract

```ts
interface ErrorBoundaryProps {
  children: React.ReactNode;
  fallback?: (error: Error, reset: () => void) => React.ReactNode;
}
```

- MUST render a non-null fallback when `componentDidCatch` fires.
- Default fallback: shadcn `Card` + `Alert` (variant `destructive`) + retry `Button`.
- Resets via `reset()` which clears boundary state and re-renders `children`.

## Reducer contract

See `data-model.md`. The reducer:
- MUST be pure.
- MUST drop stale `resolve`/`reject` actions whose `requestId` does not match the in-flight `submitting.requestId`.
- MUST return a state whose `kind` is one of the four declared discriminators.
- MUST preserve `lastResult` across `submitting`/`error` transitions when one was previously set.

## Non-regression invariants

- `App` MUST return non-null JSX for every reachable `SubmitState` value.
- The document body MUST contain at least one `data-slot="card"` element after every reducer transition.
- Submit handlers MUST NOT trigger a navigation: `window.location.href` MUST remain unchanged across submit success, submit error, and thrown render errors caught by the boundary.
