const SELECTED_CHARACTER_STORAGE_KEY = 'banana-selected-character';

export function readSelectedCharacter(): string {
  if (typeof window === 'undefined') {
    return '';
  }

  return (window.sessionStorage.getItem(SELECTED_CHARACTER_STORAGE_KEY) ?? '')
      .trim();
}

export function hasSelectedCharacter(): boolean {
  return readSelectedCharacter().length > 0;
}

export function storeSelectedCharacter(name: string): void {
  if (typeof window === 'undefined') {
    return;
  }

  window.sessionStorage.setItem(SELECTED_CHARACTER_STORAGE_KEY, name.trim());
}

export function clearSelectedCharacter(): void {
  if (typeof window === 'undefined') {
    return;
  }

  window.sessionStorage.removeItem(SELECTED_CHARACTER_STORAGE_KEY);
}
