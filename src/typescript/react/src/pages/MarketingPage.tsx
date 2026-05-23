export function MarketingPage() {
    return (
        <main
            style={{
                minHeight: "100dvh",
                background:
                    "radial-gradient(circle at 10% 10%, rgba(16, 185, 129, 0.16), transparent 32%), radial-gradient(circle at 92% 18%, rgba(245, 158, 11, 0.2), transparent 30%), linear-gradient(140deg, #05070f 0%, #0b1324 55%, #121d37 100%)",
                color: "#f8fafc",
                display: "grid",
                placeItems: "center",
                padding: "36px 20px",
            }}
        >
            <section
                style={{
                    width: "min(100%, 1020px)",
                    border: "1px solid rgba(148, 163, 184, 0.2)",
                    borderRadius: 24,
                    padding: 32,
                    background: "rgba(8, 15, 31, 0.72)",
                    boxShadow: "0 24px 90px rgba(2, 6, 23, 0.5)",
                }}
            >
                <p
                    style={{
                        margin: 0,
                        textTransform: "uppercase",
                        letterSpacing: "0.14em",
                        color: "#a7f3d0",
                        fontWeight: 700,
                        fontSize: 12,
                    }}
                >
                    Banana Engineer
                </p>
                <h1 style={{ margin: "14px 0 10px", fontSize: "clamp(2.2rem, 5vw, 4rem)", lineHeight: 0.95 }}>
                    Steam Windows build is the primary deliverable.
                </h1>
                <p style={{ margin: 0, color: "#cbd5e1", lineHeight: 1.7, maxWidth: 760 }}>
                    The website is now a marketing channel. Download and launch the desktop client from Steam to access the full DirectX12 runtime. Steam sign-in and account session state are orchestrated through the new Neon-backed auth store.
                </p>

                <div style={{ display: "flex", gap: 14, flexWrap: "wrap", marginTop: 24 }}>
                    <a
                        href="https://store.steampowered.com/"
                        target="_blank"
                        rel="noreferrer"
                        style={{
                            display: "inline-flex",
                            alignItems: "center",
                            justifyContent: "center",
                            padding: "14px 18px",
                            borderRadius: 14,
                            background: "linear-gradient(135deg, #22c55e, #0f766e)",
                            color: "#fff",
                            textDecoration: "none",
                            fontWeight: 700,
                        }}
                    >
                        Download on Steam
                    </a>
                    <a
                        href="/login"
                        style={{
                            display: "inline-flex",
                            alignItems: "center",
                            justifyContent: "center",
                            padding: "14px 18px",
                            borderRadius: 14,
                            border: "1px solid rgba(148, 163, 184, 0.3)",
                            color: "#e2e8f0",
                            textDecoration: "none",
                            fontWeight: 700,
                        }}
                    >
                        Steam account login
                    </a>
                </div>
            </section>
        </main>
    );
}
