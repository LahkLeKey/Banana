#include "navigation.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

NavGrid *nav_grid_create(float cell_size, float origin_x, float origin_z) {
    NavGrid *g = malloc(sizeof(NavGrid));
    g->cell_size  = cell_size;
    g->origin[0]  = origin_x;
    g->origin[1]  = origin_z;
    memset(g->passable, 1, sizeof(g->passable)); /* all open by default */
    return g;
}

void nav_grid_set_blocked(NavGrid *g, int col, int row, int blocked) {
    if (col < 0 || col >= NAV_GRID_W || row < 0 || row >= NAV_GRID_H) return;
    g->passable[col][row] = blocked ? 0 : 1;
}

void nav_grid_destroy(NavGrid *g) { free(g); }

/* ── A* pathfinding ───────────────────────────────────────────────────────── */

typedef struct { int col, row; int g, h, f; int parent_col, parent_row; int open, closed; } ANode;

static int heuristic(int c1, int r1, int c2, int r2) {
    int dc = c1-c2, dr = r1-r2;
    return (int)sqrtf((float)(dc*dc + dr*dr)) * 10;
}

int nav_find_path(const NavGrid *g,
                   float start_x, float start_z,
                   float goal_x,  float goal_z,
                   NavPath *out_path) {
    out_path->count = 0;

    int sc = (int)((start_x - g->origin[0]) / g->cell_size);
    int sr = (int)((start_z - g->origin[1]) / g->cell_size);
    int gc = (int)((goal_x  - g->origin[0]) / g->cell_size);
    int gr = (int)((goal_z  - g->origin[1]) / g->cell_size);

    /* Clamp to grid */
    #define CLAMP_GRID(v, max) if (v < 0) v = 0; if (v >= max) v = max-1
    CLAMP_GRID(sc, NAV_GRID_W); CLAMP_GRID(sr, NAV_GRID_H);
    CLAMP_GRID(gc, NAV_GRID_W); CLAMP_GRID(gr, NAV_GRID_H);

    static ANode nodes[NAV_GRID_W][NAV_GRID_H];
    memset(nodes, 0, sizeof(nodes));

    nodes[sc][sr].g = 0;
    nodes[sc][sr].h = heuristic(sc, sr, gc, gr);
    nodes[sc][sr].f = nodes[sc][sr].h;
    nodes[sc][sr].open = 1;
    nodes[sc][sr].col  = sc; nodes[sc][sr].row = sr;
    nodes[sc][sr].parent_col = -1; nodes[sc][sr].parent_row = -1;

    int dx[] = {0,0,1,-1,1,1,-1,-1};
    int dz[] = {1,-1,0,0,1,-1,1,-1};

    for (int iter = 0; iter < NAV_GRID_W * NAV_GRID_H * 4; iter++) {
        /* Find open node with lowest f */
        int best_f = 1<<30, bc = -1, br = -1;
        for (int c = 0; c < NAV_GRID_W; c++)
            for (int r = 0; r < NAV_GRID_H; r++)
                if (nodes[c][r].open && nodes[c][r].f < best_f) {
                    best_f = nodes[c][r].f; bc = c; br = r;
                }
        if (bc < 0) break;
        if (bc == gc && br == gr) {
            /* Reconstruct path */
            int cc = gc, cr = gr;
            while (cc >= 0 && out_path->count < 64) {
                float wx = g->origin[0] + (cc + 0.5f) * g->cell_size;
                float wz = g->origin[1] + (cr + 0.5f) * g->cell_size;
                out_path->waypoints[out_path->count][0] = wx;
                out_path->waypoints[out_path->count][1] = 0.f;
                out_path->waypoints[out_path->count][2] = wz;
                out_path->count++;
                int nc = nodes[cc][cr].parent_col;
                int nr = nodes[cc][cr].parent_row;
                cc = nc; cr = nr;
            }
            /* Reverse */
            for (int i = 0, j = out_path->count-1; i < j; i++, j--) {
                float tmp[3];
                memcpy(tmp,                   out_path->waypoints[i], 12);
                memcpy(out_path->waypoints[i],out_path->waypoints[j], 12);
                memcpy(out_path->waypoints[j],tmp,                    12);
            }
            out_path->current_index = 0;
            return 1;
        }
        nodes[bc][br].open   = 0;
        nodes[bc][br].closed = 1;

        for (int d = 0; d < 8; d++) {
            int nc = bc + dx[d], nr = br + dz[d];
            if (nc < 0 || nc >= NAV_GRID_W || nr < 0 || nr >= NAV_GRID_H) continue;
            if (!g->passable[nc][nr] || nodes[nc][nr].closed) continue;
            int cost = d < 4 ? 10 : 14;
            int ng = nodes[bc][br].g + cost;
            if (!nodes[nc][nr].open || ng < nodes[nc][nr].g) {
                nodes[nc][nr].g = ng;
                nodes[nc][nr].h = heuristic(nc, nr, gc, gr);
                nodes[nc][nr].f = ng + nodes[nc][nr].h;
                nodes[nc][nr].parent_col = bc;
                nodes[nc][nr].parent_row = br;
                nodes[nc][nr].open = 1;
                nodes[nc][nr].col  = nc; nodes[nc][nr].row = nr;
            }
        }
    }
    return 0; /* no path found */
}

int nav_move_along_path(NavPath *path, float *position, float speed, float dt) {
    if (path->current_index >= path->count) return 1;
    float *wp = path->waypoints[path->current_index];
    float dx = wp[0] - position[0];
    float dz = wp[2] - position[2];
    float dist = sqrtf(dx*dx + dz*dz);
    float step = speed * dt;
    if (dist <= step) {
        position[0] = wp[0]; position[2] = wp[2];
        path->current_index++;
    } else {
        position[0] += (dx/dist) * step;
        position[2] += (dz/dist) * step;
    }
    return (path->current_index >= path->count) ? 1 : 0;
}
