#ifndef BANANA_ENGINE_NAVIGATION_H
#define BANANA_ENGINE_NAVIGATION_H

#ifdef __cplusplus
extern "C"
{
#endif

#define NAV_GRID_W 32
#define NAV_GRID_H 32

    typedef struct NavGrid
    {
        unsigned char passable[NAV_GRID_W][NAV_GRID_H]; /* 1 = open, 0 = blocked */
        float cell_size;                                /* world units per cell */
        float origin[2];                                /* world-space x,z of grid [0][0] */
    } NavGrid;

    typedef struct NavPath
    {
        float waypoints[64][3]; /* world-space x,y,z waypoints */
        int count;
        int current_index;
    } NavPath;

    /* Build a nav grid (simple: everything passable by default). */
    NavGrid *nav_grid_create(float cell_size, float origin_x, float origin_z);
    void nav_grid_set_blocked(NavGrid *g, int col, int row, int blocked);
    void nav_grid_destroy(NavGrid *g);

    /* A* pathfinding from world-space start → goal.
     * Returns 1 on success, fills out_path. */
    int nav_find_path(const NavGrid *g, float start_x, float start_z, float goal_x, float goal_z,
                      NavPath *out_path);

    /* Advance along path at given speed.  Updates position[3] in-place.
     * Returns 1 when destination reached. */
    int nav_move_along_path(NavPath *path, float *position, float speed, float dt);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_NAVIGATION_H */
