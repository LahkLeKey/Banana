import { useEffect, useState } from "react";
import { BananaBadge, RipenessLabel } from "@banana/ui";

const API_BASE_URL = import.meta.env.VITE_BANANA_API_BASE_URL ?? "";

export function App() {
    const [banana, setBanana] = useState<number | null>(null);

    useEffect(() => {
        if (!API_BASE_URL) return;
        fetch(`${API_BASE_URL}/banana?purchases=3&multiplier=2`)
            .then((r) => r.json())
            .then((d: { banana: number }) => setBanana(d.banana))
            .catch(() => setBanana(null));
    }, []);

    return (
        <main className="mx-auto max-w-xl space-y-4 p-6">
            <h1 className="text-2xl font-semibold">Banana v2</h1>
            <p className="text-sm text-gray-600">API base: {API_BASE_URL || "<unset>"}</p>
            <div className="flex items-center gap-2">
                <BananaBadge count={banana ?? 0}>(today)</BananaBadge>
                <RipenessLabel value="yellow" />
            </div>
        </main>
    );
}
