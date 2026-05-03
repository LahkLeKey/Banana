"""Tests for the embedded Python DS scripts used in Banana notebook cells.

Covers:
  - Scaffold output contracts (BANANA_PLOTLY, BANANA_TABLE, gated/deferred markers)
  - Notebook 01 urllib-based HTTP helpers (pp, call_endpoint, get/post/patch/delete)
  - BROWSER_HTTP_HELPER (Pyodide-style) with a mocked `js` module
  - Public notebook .ipynb fixtures (valid JSON, required nbformat fields)
"""

from __future__ import annotations

import io
import json
import sys
import types
from contextlib import redirect_stdout
from pathlib import Path
from typing import Any
from unittest.mock import MagicMock, patch

import pytest

REPO_ROOT = Path(__file__).resolve().parents[2]
NOTEBOOKS_DIR = REPO_ROOT / "src" / "typescript" / "react" / "public" / "notebooks"

# ---------------------------------------------------------------------------
# Scaffold source strings
# Mirror the .join("\n") output from dsConstants.ts — tested by running them.
# ---------------------------------------------------------------------------

VIZ_BAR_SCAFFOLD = "\n".join([
    "import json",
    "figure = {",
    "    'data': [{'type': 'bar', 'x': ['banana', 'ripeness', 'chat', 'trucks'], 'y': [42, 31, 18, 25], 'marker': {'color': ['#3b82f6','#10b981','#f59e0b','#ef4444']}}],",
    "    'layout': {'title': {'text': 'Requests by Endpoint Group'}, 'xaxis': {'title': 'group'}, 'yaxis': {'title': 'count'}, 'paper_bgcolor': 'rgba(0,0,0,0)', 'plot_bgcolor': 'rgba(0,0,0,0)'}",
    "}",
    "print('BANANA_PLOTLY::' + json.dumps(figure))",
])

VIZ_LINE_SCAFFOLD = "\n".join([
    "import json",
    "figure = {",
    "    'data': [{'type': 'scatter', 'mode': 'lines+markers', 'x': ['run-1','run-2','run-3','run-4','run-5'], 'y': [0.79, 0.84, 0.86, 0.88, 0.90], 'name': 'accuracy', 'line': {'color': '#3b82f6'}}],",
    "    'layout': {'title': {'text': 'Accuracy Trend'}, 'xaxis': {'title': 'run'}, 'yaxis': {'title': 'accuracy', 'range': [0, 1]}, 'paper_bgcolor': 'rgba(0,0,0,0)', 'plot_bgcolor': 'rgba(0,0,0,0)'}",
    "}",
    "print('BANANA_PLOTLY::' + json.dumps(figure))",
])

TABLE_SCAFFOLD = "\n".join([
    "import json",
    "table = {",
    "    'columns': ['lane', 'samples', 'accuracy'],",
    "    'rows': [",
    "        ['banana', 512, 0.94],",
    "        ['not-banana', 498, 0.91],",
    "        ['ripeness', 321, 0.88]",
    "    ]",
    "}",
    "print('BANANA_TABLE::' + json.dumps(table))",
])

VEGA_GATE_SCAFFOLD = "\n".join([
    "import json",
    "payload = {",
    "    'title': 'Vega-Lite contract sample',",
    "    'summary': 'Declarative analytics specs are preserved and surfaced, but not rendered in the active wave.',",
    "    'detail': 'Use this marker to capture the spec contract safely while Plotly remains the only live chart renderer.'",
    "}",
    "print('BANANA_VEGA::' + json.dumps(payload))",
])

BOKEH_GATE_SCAFFOLD = "\n".join([
    "import json",
    "payload = {",
    "    'title': 'Bokeh runtime gate',",
    "    'summary': 'Streaming and Bokeh bundle payloads are recognized as gated runtime options.',",
    "    'detail': 'The notebook records this intent without loading a second interactive stack into the browser surface.'",
    "}",
    "print('BANANA_BOKEH::' + json.dumps(payload))",
])

WIDGET_GATE_SCAFFOLD = "\n".join([
    "import json",
    "payload = {",
    "    'title': 'Widget interop gate',",
    "    'summary': 'ipywidgets-style controls remain gated until state replay and security boundaries are implemented.',",
    "    'detail': 'Use the marker to document a notebook requirement without pretending the browser host can execute widget traffic today.'",
    "}",
    "print('BANANA_WIDGET::' + json.dumps(payload))",
    "print('BANANA_BQPLOT::' + json.dumps({'title': 'bqplot gate', 'summary': 'Widget-driven plotting stays behind the same gate.'}))",
])

SPECIALIZED_SURFACE_SCAFFOLD = "\n".join([
    "import json",
    "print('BANANA_GEO::' + json.dumps({'title': 'Geospatial surface deferred', 'summary': 'Map and tile flows are explicitly deferred beyond the current release wave.'}))",
    "print('BANANA_NETWORK::' + json.dumps({'title': 'Network graph surface deferred', 'summary': 'Relationship views need a dedicated host and are not bundled into the default notebook runner.'}))",
    "print('BANANA_3D::' + json.dumps({'title': '3D surface deferred', 'summary': 'WebGL-heavy notebook views remain outside the current DS release boundary.'}))",
])

# Pyodide-style browser helper (uses `from js import XMLHttpRequest`)
BROWSER_HTTP_HELPER = "\n".join([
    "import json",
    "from js import XMLHttpRequest",
    "",
    "API_BASE = 'https://api.banana.engineer'",
    "",
    "def pp(obj):",
    "    if isinstance(obj, str):",
    "        try:",
    "            obj = json.loads(obj)",
    "        except Exception:",
    "            print(obj)",
    "            return",
    "    print(json.dumps(obj, indent=2))",
    "",
    "def call_endpoint(method, path, payload=None, headers=None):",
    "    url = path if str(path).startswith('http') else f\"{API_BASE}{path}\"",
    "    try:",
    "        xhr = XMLHttpRequest.new()",
    "        xhr.open(method.upper(), url, False)",
    "        xhr.setRequestHeader('Accept', 'application/json')",
    "        if headers:",
    "            for k, v in headers.items():",
    "                xhr.setRequestHeader(str(k), str(v))",
    "        body = None",
    "        if payload is not None:",
    "            xhr.setRequestHeader('Content-Type', 'application/json')",
    "            body = json.dumps(payload)",
    "        xhr.send(body)",
    "        return {'ok': 200 <= int(xhr.status) < 300, 'status': int(xhr.status), 'body': str(xhr.responseText)}",
    "    except Exception as ex:",
    "        return {'ok': False, 'status': None, 'body': str(ex)}",
    "",
    "def get(path, **kw):",
    "    return call_endpoint('GET', path, **kw)",
    "",
    "def post(path, **kw):",
    "    return call_endpoint('POST', path, **kw)",
])


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _exec_capture(code: str, globs: dict[str, Any] | None = None) -> str:
    """Execute *code* and return everything written to stdout."""
    buf = io.StringIO()
    ns: dict[str, Any] = globs.copy() if globs else {}
    with redirect_stdout(buf):
        exec(code, ns)  # noqa: S102
    return buf.getvalue()


def _marker_json(output: str, prefix: str) -> Any:
    """Extract and parse the JSON payload after *prefix* on the first matching line."""
    for line in output.splitlines():
        if line.startswith(prefix):
            return json.loads(line[len(prefix):])
    raise AssertionError(f"No line starting with {prefix!r} found in output:\n{output}")


# ---------------------------------------------------------------------------
# Scaffold output contracts
# ---------------------------------------------------------------------------

class TestVizBarScaffold:
    def test_emits_banana_plotly_marker(self) -> None:
        out = _exec_capture(VIZ_BAR_SCAFFOLD)
        assert "BANANA_PLOTLY::" in out

    def test_payload_is_valid_json(self) -> None:
        out = _exec_capture(VIZ_BAR_SCAFFOLD)
        fig = _marker_json(out, "BANANA_PLOTLY::")
        assert isinstance(fig, dict)

    def test_payload_has_data_array(self) -> None:
        out = _exec_capture(VIZ_BAR_SCAFFOLD)
        fig = _marker_json(out, "BANANA_PLOTLY::")
        assert isinstance(fig.get("data"), list)
        assert len(fig["data"]) > 0

    def test_trace_type_is_bar(self) -> None:
        out = _exec_capture(VIZ_BAR_SCAFFOLD)
        fig = _marker_json(out, "BANANA_PLOTLY::")
        assert fig["data"][0]["type"] == "bar"

    def test_payload_has_layout(self) -> None:
        out = _exec_capture(VIZ_BAR_SCAFFOLD)
        fig = _marker_json(out, "BANANA_PLOTLY::")
        assert "layout" in fig


class TestVizLineScaffold:
    def test_emits_banana_plotly_marker(self) -> None:
        out = _exec_capture(VIZ_LINE_SCAFFOLD)
        assert "BANANA_PLOTLY::" in out

    def test_payload_is_valid_json(self) -> None:
        out = _exec_capture(VIZ_LINE_SCAFFOLD)
        fig = _marker_json(out, "BANANA_PLOTLY::")
        assert isinstance(fig, dict)

    def test_trace_type_is_scatter(self) -> None:
        out = _exec_capture(VIZ_LINE_SCAFFOLD)
        fig = _marker_json(out, "BANANA_PLOTLY::")
        assert fig["data"][0]["type"] == "scatter"

    def test_y_values_are_floats_in_zero_one(self) -> None:
        out = _exec_capture(VIZ_LINE_SCAFFOLD)
        fig = _marker_json(out, "BANANA_PLOTLY::")
        for val in fig["data"][0]["y"]:
            assert 0.0 <= float(val) <= 1.0


class TestTableScaffold:
    def test_emits_banana_table_marker(self) -> None:
        out = _exec_capture(TABLE_SCAFFOLD)
        assert "BANANA_TABLE::" in out

    def test_payload_is_valid_json(self) -> None:
        out = _exec_capture(TABLE_SCAFFOLD)
        table = _marker_json(out, "BANANA_TABLE::")
        assert isinstance(table, dict)

    def test_payload_has_columns_and_rows(self) -> None:
        out = _exec_capture(TABLE_SCAFFOLD)
        table = _marker_json(out, "BANANA_TABLE::")
        assert isinstance(table.get("columns"), list)
        assert isinstance(table.get("rows"), list)

    def test_row_count_matches_expected(self) -> None:
        out = _exec_capture(TABLE_SCAFFOLD)
        table = _marker_json(out, "BANANA_TABLE::")
        assert len(table["rows"]) == 3

    def test_column_count_matches_header(self) -> None:
        out = _exec_capture(TABLE_SCAFFOLD)
        table = _marker_json(out, "BANANA_TABLE::")
        for row in table["rows"]:
            assert len(row) == len(table["columns"])


class TestVegaGateScaffold:
    def test_emits_banana_vega_marker(self) -> None:
        out = _exec_capture(VEGA_GATE_SCAFFOLD)
        assert "BANANA_VEGA::" in out

    def test_payload_is_valid_json(self) -> None:
        out = _exec_capture(VEGA_GATE_SCAFFOLD)
        payload = _marker_json(out, "BANANA_VEGA::")
        assert isinstance(payload, dict)

    def test_payload_has_title_and_summary(self) -> None:
        out = _exec_capture(VEGA_GATE_SCAFFOLD)
        payload = _marker_json(out, "BANANA_VEGA::")
        assert payload.get("title")
        assert payload.get("summary")


class TestBokehGateScaffold:
    def test_emits_banana_bokeh_marker(self) -> None:
        out = _exec_capture(BOKEH_GATE_SCAFFOLD)
        assert "BANANA_BOKEH::" in out

    def test_payload_is_valid_json(self) -> None:
        out = _exec_capture(BOKEH_GATE_SCAFFOLD)
        payload = _marker_json(out, "BANANA_BOKEH::")
        assert isinstance(payload, dict)


class TestWidgetGateScaffold:
    def test_emits_banana_widget_marker(self) -> None:
        out = _exec_capture(WIDGET_GATE_SCAFFOLD)
        assert "BANANA_WIDGET::" in out

    def test_also_emits_bqplot_marker(self) -> None:
        out = _exec_capture(WIDGET_GATE_SCAFFOLD)
        assert "BANANA_BQPLOT::" in out

    def test_both_payloads_are_valid_json(self) -> None:
        out = _exec_capture(WIDGET_GATE_SCAFFOLD)
        widget = _marker_json(out, "BANANA_WIDGET::")
        bqplot = _marker_json(out, "BANANA_BQPLOT::")
        assert isinstance(widget, dict)
        assert isinstance(bqplot, dict)


class TestSpecializedSurfaceScaffold:
    def test_emits_banana_geo_marker(self) -> None:
        out = _exec_capture(SPECIALIZED_SURFACE_SCAFFOLD)
        assert "BANANA_GEO::" in out

    def test_emits_banana_network_marker(self) -> None:
        out = _exec_capture(SPECIALIZED_SURFACE_SCAFFOLD)
        assert "BANANA_NETWORK::" in out

    def test_emits_banana_3d_marker(self) -> None:
        out = _exec_capture(SPECIALIZED_SURFACE_SCAFFOLD)
        assert "BANANA_3D::" in out

    def test_all_payloads_are_valid_json(self) -> None:
        out = _exec_capture(SPECIALIZED_SURFACE_SCAFFOLD)
        for prefix in ("BANANA_GEO::", "BANANA_NETWORK::", "BANANA_3D::"):
            payload = _marker_json(out, prefix)
            assert isinstance(payload, dict)
            assert payload.get("title")
            assert payload.get("summary")


# ---------------------------------------------------------------------------
# Notebook 01 urllib-based HTTP helpers
# ---------------------------------------------------------------------------

# Extract cells from 01-platform-setup.ipynb
def _load_notebook_cells(name: str) -> list[str]:
    nb = json.loads((NOTEBOOKS_DIR / name).read_text(encoding="utf-8"))
    return [
        "".join(cell["source"]) if isinstance(cell["source"], list) else cell["source"]
        for cell in nb.get("cells", [])
        if cell.get("cell_type") == "code"
    ]


class TestNotebook01Helpers:
    """Tests for the urllib-based helper logic in 01-platform-setup.ipynb."""

    @pytest.fixture(autouse=True)
    def _setup(self) -> None:
        cells = _load_notebook_cells("01-platform-setup.ipynb")
        # Cell 0: config (pp + API_BASE), Cell 1: HTTP helpers
        assert len(cells) >= 2, "Expected at least 2 code cells in notebook 01"
        self._config_cell = cells[0]
        self._helper_cell = cells[1]

    def _exec_helpers(self) -> dict[str, Any]:
        ns: dict[str, Any] = {}
        buf = io.StringIO()
        with redirect_stdout(buf):
            exec(self._config_cell, ns)  # noqa: S102
            exec(self._helper_cell, ns)  # noqa: S102
        return ns

    def test_pp_prints_indented_json_for_dict(self) -> None:
        ns = self._exec_helpers()
        out = io.StringIO()
        with redirect_stdout(out):
            ns["pp"]({"key": "value"})
        parsed = json.loads(out.getvalue())
        assert parsed == {"key": "value"}

    def test_pp_parses_json_string(self) -> None:
        ns = self._exec_helpers()
        out = io.StringIO()
        with redirect_stdout(out):
            ns["pp"]('{"x": 1}')
        parsed = json.loads(out.getvalue())
        assert parsed == {"x": 1}

    def test_pp_prints_raw_string_on_non_json(self) -> None:
        ns = self._exec_helpers()
        out = io.StringIO()
        with redirect_stdout(out):
            ns["pp"]("not-json")
        assert out.getvalue().strip() == "not-json"

    def test_call_endpoint_returns_ok_on_200(self) -> None:
        ns = self._exec_helpers()

        class _FakeResponse:
            status = 200
            def read(self) -> bytes:
                return b'{"ok": true}'
            def __enter__(self): return self
            def __exit__(self, *_): pass

        with patch("urllib.request.urlopen", return_value=_FakeResponse()):
            result = ns["call_endpoint"]("GET", "/health")

        assert result["ok"] is True
        assert result["status"] == 200

    def test_call_endpoint_returns_not_ok_on_http_error(self) -> None:
        import urllib.error
        ns = self._exec_helpers()

        err = urllib.error.HTTPError("/health", 404, "Not Found", {}, None)
        err.read = lambda: b"missing"  # type: ignore[method-assign]
        with patch("urllib.request.urlopen", side_effect=err):
            result = ns["call_endpoint"]("GET", "/health")

        assert result["ok"] is False
        assert result["status"] == 404

    def test_call_endpoint_returns_not_ok_on_network_error(self) -> None:
        ns = self._exec_helpers()

        with patch("urllib.request.urlopen", side_effect=OSError("timeout")):
            result = ns["call_endpoint"]("GET", "/health")

        assert result["ok"] is False
        assert result["status"] is None
        assert "timeout" in result["body"]

    def test_get_uses_get_method(self) -> None:
        ns = self._exec_helpers()
        calls: list[Any] = []

        class _FakeResp:
            status = 200
            def read(self) -> bytes: return b"{}"
            def __enter__(self): return self
            def __exit__(self, *_): pass

        def _fake_urlopen(req: Any) -> _FakeResp:
            calls.append(req.get_method())
            return _FakeResp()

        with patch("urllib.request.urlopen", side_effect=_fake_urlopen):
            ns["get"]("/test")

        assert calls == ["GET"]

    def test_post_uses_post_method(self) -> None:
        ns = self._exec_helpers()
        calls: list[Any] = []

        class _FakeResp:
            status = 200
            def read(self) -> bytes: return b"{}"
            def __enter__(self): return self
            def __exit__(self, *_): pass

        def _fake_urlopen(req: Any) -> _FakeResp:
            calls.append(req.get_method())
            return _FakeResp()

        with patch("urllib.request.urlopen", side_effect=_fake_urlopen):
            ns["post"]("/test", payload={"a": 1})

        assert calls == ["POST"]

    def test_api_base_is_production_url(self) -> None:
        ns = self._exec_helpers()
        assert ns["API_BASE"] == "https://api.banana.engineer"


# ---------------------------------------------------------------------------
# BROWSER_HTTP_HELPER with mocked `js` module (Pyodide compat)
# ---------------------------------------------------------------------------

def _make_js_mock(status: int = 200, response_text: str = "{}") -> types.ModuleType:
    """Build a fake `js` module with a controllable XMLHttpRequest."""
    xhr_instance = MagicMock()
    xhr_instance.status = status
    xhr_instance.responseText = response_text

    xhr_class = MagicMock()
    xhr_class.new.return_value = xhr_instance

    js_mod = types.ModuleType("js")
    js_mod.XMLHttpRequest = xhr_class  # type: ignore[attr-defined]
    return js_mod


class TestBrowserHttpHelper:
    """Tests for BROWSER_HTTP_HELPER using a mocked js/XMLHttpRequest."""

    def _exec_helper(self, js_mod: types.ModuleType) -> dict[str, Any]:
        sys.modules["js"] = js_mod
        try:
            ns: dict[str, Any] = {}
            exec(BROWSER_HTTP_HELPER, ns)  # noqa: S102
            return ns
        finally:
            sys.modules.pop("js", None)

    def test_defines_pp_function(self) -> None:
        ns = self._exec_helper(_make_js_mock())
        assert callable(ns.get("pp"))

    def test_defines_call_endpoint_function(self) -> None:
        ns = self._exec_helper(_make_js_mock())
        assert callable(ns.get("call_endpoint"))

    def test_defines_get_function(self) -> None:
        ns = self._exec_helper(_make_js_mock())
        assert callable(ns.get("get"))

    def test_defines_post_function(self) -> None:
        ns = self._exec_helper(_make_js_mock())
        assert callable(ns.get("post"))

    def test_api_base_is_production_url(self) -> None:
        ns = self._exec_helper(_make_js_mock())
        assert ns["API_BASE"] == "https://api.banana.engineer"

    def test_call_endpoint_returns_ok_on_200(self) -> None:
        js_mod = _make_js_mock(status=200, response_text='{"result": "ok"}')
        ns = self._exec_helper(js_mod)
        result = ns["call_endpoint"]("GET", "/health")
        assert result["ok"] is True
        assert result["status"] == 200

    def test_call_endpoint_returns_not_ok_on_500(self) -> None:
        js_mod = _make_js_mock(status=500, response_text="error")
        ns = self._exec_helper(js_mod)
        result = ns["call_endpoint"]("GET", "/health")
        assert result["ok"] is False
        assert result["status"] == 500

    def test_call_endpoint_opens_with_correct_method_and_url(self) -> None:
        js_mod = _make_js_mock()
        ns = self._exec_helper(js_mod)
        ns["call_endpoint"]("GET", "/ping")
        xhr = js_mod.XMLHttpRequest.new.return_value
        xhr.open.assert_called_once_with("GET", "https://api.banana.engineer/ping", False)

    def test_call_endpoint_sends_json_payload(self) -> None:
        js_mod = _make_js_mock()
        ns = self._exec_helper(js_mod)
        ns["call_endpoint"]("POST", "/infer", payload={"img": "abc"})
        xhr = js_mod.XMLHttpRequest.new.return_value
        sent_body = xhr.send.call_args[0][0]
        assert json.loads(sent_body) == {"img": "abc"}

    def test_call_endpoint_uses_absolute_url_unchanged(self) -> None:
        js_mod = _make_js_mock()
        ns = self._exec_helper(js_mod)
        ns["call_endpoint"]("GET", "https://other.example.com/path")
        xhr = js_mod.XMLHttpRequest.new.return_value
        xhr.open.assert_called_once_with("GET", "https://other.example.com/path", False)

    def test_call_endpoint_returns_not_ok_on_xhr_exception(self) -> None:
        js_mod = _make_js_mock()
        js_mod.XMLHttpRequest.new.side_effect = RuntimeError("network error")
        ns = self._exec_helper(js_mod)
        result = ns["call_endpoint"]("GET", "/health")
        assert result["ok"] is False
        assert result["status"] is None
        assert "network error" in result["body"]

    def test_get_convenience_calls_get_method(self) -> None:
        js_mod = _make_js_mock()
        ns = self._exec_helper(js_mod)
        ns["get"]("/health")
        xhr = js_mod.XMLHttpRequest.new.return_value
        xhr.open.assert_called_once()
        assert xhr.open.call_args[0][0] == "GET"

    def test_post_convenience_calls_post_method(self) -> None:
        js_mod = _make_js_mock()
        ns = self._exec_helper(js_mod)
        ns["post"]("/infer", payload={})
        xhr = js_mod.XMLHttpRequest.new.return_value
        assert xhr.open.call_args[0][0] == "POST"

    def test_pp_outputs_indented_json(self) -> None:
        js_mod = _make_js_mock()
        ns = self._exec_helper(js_mod)
        out = io.StringIO()
        with redirect_stdout(out):
            ns["pp"]({"hello": "world"})
        parsed = json.loads(out.getvalue())
        assert parsed == {"hello": "world"}


# ---------------------------------------------------------------------------
# Notebook .ipynb fixture validation
# ---------------------------------------------------------------------------

EXPECTED_NOTEBOOKS = [
    "01-platform-setup.ipynb",
    "02-banana-classifier-workbench.ipynb",
    "03-ripeness-ml-model-ops.ipynb",
    "04-logistics-trucks-harvest.ipynb",
    "05-chat-sessions-operator-audit.ipynb",
    "06-full-api-smoke-suite.ipynb",
    "07-visualization-workbench.ipynb",
]


@pytest.mark.parametrize("filename", EXPECTED_NOTEBOOKS)
def test_notebook_fixture_is_valid_json(filename: str) -> None:
    path = NOTEBOOKS_DIR / filename
    assert path.exists(), f"Notebook fixture missing: {filename}"
    data = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(data, dict)


@pytest.mark.parametrize("filename", EXPECTED_NOTEBOOKS)
def test_notebook_fixture_nbformat_is_4(filename: str) -> None:
    data = json.loads((NOTEBOOKS_DIR / filename).read_text(encoding="utf-8"))
    assert data.get("nbformat") == 4


@pytest.mark.parametrize("filename", EXPECTED_NOTEBOOKS)
def test_notebook_fixture_has_cells(filename: str) -> None:
    data = json.loads((NOTEBOOKS_DIR / filename).read_text(encoding="utf-8"))
    assert isinstance(data.get("cells"), list)
    assert len(data["cells"]) > 0


@pytest.mark.parametrize("filename", EXPECTED_NOTEBOOKS)
def test_notebook_fixture_cells_have_required_fields(filename: str) -> None:
    data = json.loads((NOTEBOOKS_DIR / filename).read_text(encoding="utf-8"))
    for i, cell in enumerate(data["cells"]):
        assert "cell_type" in cell, f"{filename} cell[{i}] missing cell_type"
        assert "source" in cell, f"{filename} cell[{i}] missing source"
        assert cell["cell_type"] in ("code", "markdown"), (
            f"{filename} cell[{i}] has unknown cell_type: {cell['cell_type']!r}"
        )


@pytest.mark.parametrize("filename", EXPECTED_NOTEBOOKS)
def test_notebook_fixture_has_metadata(filename: str) -> None:
    data = json.loads((NOTEBOOKS_DIR / filename).read_text(encoding="utf-8"))
    assert isinstance(data.get("metadata"), dict)
    # Notebooks may use kernelspec or language_info (or both) to declare the language.
    meta = data["metadata"]
    lang = (
        meta.get("kernelspec", {}).get("language")
        or meta.get("language_info", {}).get("name")
    )
    assert lang == "python", f"{filename} missing python language declaration in metadata"
