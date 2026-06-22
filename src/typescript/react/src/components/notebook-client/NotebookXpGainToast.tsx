type NotebookXpGainToastProps = {
    readonly message: string;
    readonly bottomInset: number;
};

export function NotebookXpGainToast({ message, bottomInset }: NotebookXpGainToastProps) {
    if (!message) {
        return null;
    }

    return (
        <div
            style={{
                position: 'absolute',
                left: 12,
                bottom: bottomInset + 14,
                zIndex: 6,
                maxWidth: 'min(420px, calc(100% - 24px))',
                borderRadius: 12,
                border: '1px solid rgba(251, 191, 36, 0.6)',
                background: 'linear-gradient(135deg, rgba(120, 53, 15, 0.9), rgba(146, 64, 14, 0.82))',
                color: '#fef3c7',
                padding: '10px 12px',
                fontSize: 12,
                letterSpacing: '0.05em',
                textTransform: 'uppercase',
                fontWeight: 700,
                boxShadow: '0 14px 28px rgba(120, 53, 15, 0.38)',
            }}
        >
            {message}
        </div>
    );
}
