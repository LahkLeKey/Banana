export type K3h4GeometryPoint2d = {
  readonly x: number; readonly y: number;
};

export type K3h4ClusterGeometry2d = {
  readonly clusterId: number; readonly centroid: K3h4GeometryPoint2d; readonly inscribedRadius: number; readonly polygon: ReadonlyArray<
                                                                                                                     K3h4GeometryPoint2d>;
};

export type K3h4TokenGeometry2d = {
  readonly tokenId: string; readonly x: number; readonly y: number; readonly clusterId: number; readonly weightedScore:
                                                                                                             number;
};

export type K3h4ProjectionMetadata = {
  readonly method: string; readonly components: number; readonly explainedVariance:
                                                                     number;
};

export type K3h4EpochGeometryResponse = {
  readonly contractVersion: number; readonly sessionId: string; readonly epoch: number; readonly mode: 'multiplicative' | 'power'; readonly clusters: ReadonlyArray<K3h4ClusterGeometry2d>; readonly tokens: ReadonlyArray<K3h4TokenGeometry2d>; readonly projectionMetadata:
                                                                                                                                                                                                                                                              K3h4ProjectionMetadata;
};