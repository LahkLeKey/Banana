import { useEffect, useMemo, useState } from "react";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "../components/ui/card";
import { Button } from "../components/ui/button";
import { Textarea } from "../components/ui/textarea";
import { fetchEnsembleVerdict, resolveApiBaseUrl } from "../lib/api";
import { trackEvent } from "../lib/analytics";
import {
    analyzeQuizBenchmark,
    type HumanGrade,
    type QuizAttempt,
    type QuizTrend,
} from "../lib/quizBenchmark";

type QuizQuestion = {
    id: string;
    prompt: string;
    expectedLabel: string;
};

type QuizAttemptRow = QuizAttempt & {
    questionId: string;
    prompt: string;
    expectedLabel: string;
    predictedLabel: string;
    graderNotes: string;
};

const QUIZ_HISTORY_KEY = "banana.quizQa.history.v1";

const quizQuestions: QuizQuestion[] = [
    {
        id: "q-001",
        prompt: "A ripe yellow banana with a few brown spots.",
        expectedLabel: "banana",
    },
    {
        id: "q-002",
        prompt: "A steel keyboard with RGB lights.",
        expectedLabel: "not-banana",
    },
    {
        id: "q-003",
        prompt: "A banana smoothie in a clear cup.",
        expectedLabel: "banana",
    },
];

function readHistory(): QuizAttemptRow[] {
    if (typeof window === "undefined") {
        return [];
    }

    try {
        const raw = window.localStorage.getItem(QUIZ_HISTORY_KEY);
        if (!raw) {
            return [];
        }

        const parsed = JSON.parse(raw) as QuizAttemptRow[];
        if (!Array.isArray(parsed)) {
            return [];
        }

        return parsed;
    } catch {
        return [];
    }
}

function writeHistory(history: QuizAttemptRow[]): void {
    if (typeof window === "undefined") {
        return;
    }
    window.localStorage.setItem(QUIZ_HISTORY_KEY, JSON.stringify(history));
}

function trendLabel(trend: QuizTrend): string {
    switch (trend) {
        case "meets-benchmark":
            return "Meets benchmark";
        case "growing":
            return "Growing toward benchmark";
        case "regressed-below-floor":
            return "Regressed below floor";
        case "insufficient-data":
            return "Insufficient data";
        default:
            return "Stable";
    }
}

export function QuizQaPage() {
    const [selectedQuestionId, setSelectedQuestionId] = useState(quizQuestions[0]?.id ?? "");
    const [questionText, setQuestionText] = useState(quizQuestions[0]?.prompt ?? "");
    const [expectedLabel, setExpectedLabel] = useState(quizQuestions[0]?.expectedLabel ?? "banana");
    const [history, setHistory] = useState<QuizAttemptRow[]>([]);
    const [runningQuiz, setRunningQuiz] = useState(false);
    const [runError, setRunError] = useState<string | null>(null);

    useEffect(() => {
        setHistory(readHistory());
    }, []);

    const benchmarkSummary = useMemo(() => analyzeQuizBenchmark(history), [history]);

    const selectedQuestion = quizQuestions.find((item) => item.id === selectedQuestionId) ?? null;

    function syncQuestionSelection(questionId: string) {
        const question = quizQuestions.find((item) => item.id === questionId);
        setSelectedQuestionId(questionId);
        if (question) {
            setQuestionText(question.prompt);
            setExpectedLabel(question.expectedLabel);
        }
    }

    function upsertHistory(next: QuizAttemptRow[]) {
        setHistory(next);
        writeHistory(next);
    }

    async function runQuizAgainstModel() {
        if (!questionText.trim()) {
            return;
        }

        setRunningQuiz(true);
        setRunError(null);
        try {
            const base = resolveApiBaseUrl();
            const verdict = await fetchEnsembleVerdict(base, questionText.trim());
            const predictedLabel = verdict.label;
            const autoScore = predictedLabel === expectedLabel ? 1 : 0;

            const nextAttempt: QuizAttemptRow = {
                id: `attempt-${Date.now()}`,
                answeredAtUtc: new Date().toISOString(),
                autoScore,
                humanGrade: "ungraded",
                questionId: selectedQuestion?.id ?? "custom",
                prompt: questionText.trim(),
                expectedLabel,
                predictedLabel,
                graderNotes: "",
            };

            const nextHistory = [nextAttempt, ...history].slice(0, 200);
            upsertHistory(nextHistory);

            trackEvent("banana_quiz_run", {
                expectedLabel,
                predictedLabel,
                autoScore,
            });
        } catch (error) {
            setRunError(error instanceof Error ? error.message : String(error));
        } finally {
            setRunningQuiz(false);
        }
    }

    function addMockAttempt() {
        const seed = history.length + 1;
        const autoScore = seed % 3 === 0 ? 0 : 1;
        const mock: QuizAttemptRow = {
            id: `mock-${Date.now()}`,
            answeredAtUtc: new Date().toISOString(),
            autoScore,
            humanGrade: "ungraded",
            questionId: selectedQuestion?.id ?? "mock",
            prompt: questionText.trim() || "Mock prompt",
            expectedLabel,
            predictedLabel: autoScore === 1 ? expectedLabel : expectedLabel === "banana" ? "not-banana" : "banana",
            graderNotes: "",
        };

        const nextHistory = [mock, ...history].slice(0, 200);
        upsertHistory(nextHistory);

        trackEvent("banana_quiz_mock_attempt", {
            autoScore,
        });
    }

    function updateHumanGrade(id: string, humanGrade: HumanGrade) {
        const nextHistory = history.map((item) =>
            item.id === id
                ? {
                    ...item,
                    humanGrade,
                }
                : item
        );
        upsertHistory(nextHistory);

        trackEvent("banana_quiz_grade_update", {
            humanGrade,
        });
    }

    function updateGraderNotes(id: string, notes: string) {
        const nextHistory = history.map((item) =>
            item.id === id
                ? {
                    ...item,
                    graderNotes: notes,
                }
                : item
        );
        upsertHistory(nextHistory);
    }

    return (
        <div className="space-y-4" data-testid="quiz-qa-page">
            <Card>
                <CardHeader>
                    <CardTitle>Model Quiz QA</CardTitle>
                    <CardDescription>
                        Run repeatable quiz checks against the model, track benchmark trend, and grade past answers with a human-in-the-loop workflow.
                    </CardDescription>
                </CardHeader>
            </Card>

            <Card data-testid="quiz-benchmark-card">
                <CardHeader>
                    <CardTitle className="text-base">Benchmark and Non-Regression Gate</CardTitle>
                    <CardDescription>
                        Target benchmark: {(benchmarkSummary.benchmarkTarget * 100).toFixed(0)}% · Regression floor: {(benchmarkSummary.regressionFloor * 100).toFixed(0)}%
                    </CardDescription>
                </CardHeader>
                <CardContent className="space-y-2 text-sm">
                    <div className="rounded border px-3 py-2" data-testid="quiz-benchmark-status">
                        <p className="font-medium">{trendLabel(benchmarkSummary.trend)}</p>
                        <p className="text-muted-foreground">
                            Gate pass: {benchmarkSummary.gatePass ? "yes" : "no"}
                        </p>
                    </div>
                    <div className="grid gap-2 md:grid-cols-2">
                        <div className="rounded border px-3 py-2" data-testid="quiz-current-score">
                            Current average: {(benchmarkSummary.currentAverage * 100).toFixed(2)}%
                        </div>
                        <div className="rounded border px-3 py-2" data-testid="quiz-previous-score">
                            Previous average: {benchmarkSummary.previousAverage == null ? "n/a" : `${(benchmarkSummary.previousAverage * 100).toFixed(2)}%`}
                        </div>
                    </div>
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Run Quiz Against Model</CardTitle>
                </CardHeader>
                <CardContent className="space-y-3">
                    <label className="text-sm" htmlFor="quiz-question-select">Quiz question</label>
                    <select
                        id="quiz-question-select"
                        value={selectedQuestionId}
                        onChange={(event) => syncQuestionSelection(event.target.value)}
                        className="w-full rounded border bg-background px-3 py-2 text-sm"
                        data-testid="quiz-question-select"
                    >
                        {quizQuestions.map((question) => (
                            <option key={question.id} value={question.id}>{question.id} · {question.expectedLabel}</option>
                        ))}
                    </select>

                    <Textarea
                        value={questionText}
                        onChange={(event) => setQuestionText(event.target.value)}
                        rows={3}
                        data-testid="quiz-question-prompt"
                    />

                    <label className="text-sm" htmlFor="quiz-expected-label">Expected label</label>
                    <select
                        id="quiz-expected-label"
                        value={expectedLabel}
                        onChange={(event) => setExpectedLabel(event.target.value)}
                        className="w-full rounded border bg-background px-3 py-2 text-sm"
                        data-testid="quiz-expected-label"
                    >
                        <option value="banana">banana</option>
                        <option value="not-banana">not-banana</option>
                    </select>

                    <div className="flex flex-wrap gap-2">
                        <Button data-testid="quiz-run-model-btn" onClick={runQuizAgainstModel} disabled={runningQuiz || !questionText.trim()}>
                            {runningQuiz ? "Running..." : "Run Model Quiz"}
                        </Button>
                        <Button type="button" variant="outline" data-testid="quiz-add-mock-btn" onClick={addMockAttempt}>
                            Add Mock Attempt
                        </Button>
                    </div>

                    {runError ? <p className="text-sm text-destructive" data-testid="quiz-run-error">{runError}</p> : null}
                </CardContent>
            </Card>

            <Card>
                <CardHeader>
                    <CardTitle className="text-base">Past Quiz Answers (Human Grading)</CardTitle>
                </CardHeader>
                <CardContent className="space-y-3" data-testid="quiz-history-list">
                    {history.length === 0 ? (
                        <p className="text-sm text-muted-foreground">No quiz attempts yet.</p>
                    ) : (
                        history.map((item) => (
                            <div key={item.id} className="rounded border p-3 space-y-2" data-testid="quiz-history-item">
                                <div className="grid gap-1 text-sm md:grid-cols-2">
                                    <p><span className="font-medium">Expected:</span> {item.expectedLabel}</p>
                                    <p><span className="font-medium">Predicted:</span> {item.predictedLabel}</p>
                                    <p><span className="font-medium">Auto score:</span> {(item.autoScore * 100).toFixed(0)}%</p>
                                    <p><span className="font-medium">Answered:</span> {item.answeredAtUtc}</p>
                                </div>
                                <p className="text-xs text-muted-foreground">{item.prompt}</p>
                                <div className="grid gap-2 md:grid-cols-[180px_1fr]">
                                    <select
                                        value={item.humanGrade}
                                        onChange={(event) => updateHumanGrade(item.id, event.target.value as HumanGrade)}
                                        className="rounded border bg-background px-2 py-1 text-sm"
                                        data-testid="quiz-human-grade-select"
                                    >
                                        <option value="ungraded">ungraded</option>
                                        <option value="pass">pass</option>
                                        <option value="fail">fail</option>
                                    </select>
                                    <input
                                        value={item.graderNotes}
                                        onChange={(event) => updateGraderNotes(item.id, event.target.value)}
                                        placeholder="grader notes"
                                        className="rounded border bg-background px-2 py-1 text-sm"
                                        data-testid="quiz-grader-notes"
                                    />
                                </div>
                            </div>
                        ))
                    )}
                </CardContent>
            </Card>
        </div>
    );
}
