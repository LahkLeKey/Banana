import type {MutableRefObject} from 'react';

export function assignPanelContainerRef(
    ref: MutableRefObject<HTMLElement|null>, node: Element|null): void {
  ref.current = node as HTMLElement | null;
}
