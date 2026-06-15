/** Projected 2D coordinate used by notebook-era geometry views. */
export type K3h4GeometryPoint2d = {
  readonly x: number; readonly y: number;
};

/** Polygon and centroid geometry for one projected k3h4 cluster. */
export type K3h4ClusterGeometry2d = {
  readonly clusterId: number; readonly centroid: K3h4GeometryPoint2d; readonly inscribedRadius: number; readonly polygon: ReadonlyArray<
                                                                                                                     K3h4GeometryPoint2d>;
};

/** One token projected into the 2D benchmark/epoch geometry view. */
export type K3h4TokenGeometry2d = {
  readonly tokenId: string; readonly x: number; readonly y: number; readonly clusterId: number; readonly weightedScore:
                                                                                                             number;
};

/** Projection metadata describing how the 2D geometry was derived. */
export type K3h4ProjectionMetadata = {
  readonly method: string; readonly components: number; readonly explainedVariance:
                                                                     number;
};

/** Full geometry payload returned for a recorded k3h4 epoch visualization. */
export type K3h4EpochGeometryResponse = {
  readonly contractVersion: number; readonly sessionId: string; readonly epoch: number; readonly mode: 'multiplicative' | 'power'; readonly clusters: ReadonlyArray<K3h4ClusterGeometry2d>; readonly tokens: ReadonlyArray<K3h4TokenGeometry2d>; readonly projectionMetadata:
                                                                                                                                                                                                                                                              K3h4ProjectionMetadata;
};