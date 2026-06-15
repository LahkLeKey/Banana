import '../../../test-setup';

import {describe, expect, test} from 'bun:test';
import {render, screen} from '@testing-library/react';
import userEvent from '@testing-library/user-event';

import {
  HypersphereGeometryOverlay,
  type EpochGeometryPayload,
} from './HypersphereGeometryOverlay';

const EPOCH_ZERO_GEOMETRY: EpochGeometryPayload = {
  epoch: 0,
  mode: 'power',
  projectionMetadata: {
    method: 'pca',
    components: 2,
    explainedVariance: 0.93,
  },
  clusters: [
    {
      clusterId: 0,
      centroid: {x: -0.5, y: 0.1},
      inscribedRadius: 0.22,
      polygon: [
        {x: -1.0, y: -0.2},
        {x: -0.2, y: -0.3},
        {x: -0.1, y: 0.4},
        {x: -0.8, y: 0.5},
      ],
    },
    {
      clusterId: 1,
      centroid: {x: 0.6, y: -0.1},
      inscribedRadius: 0.18,
      polygon: [
        {x: 0.1, y: -0.4},
        {x: 1.1, y: -0.5},
        {x: 1.2, y: 0.3},
        {x: 0.2, y: 0.2},
      ],
    },
  ],
  tokens: [
    {tokenId: 'tok-a', x: -0.6, y: 0.2, clusterId: 0, weightedScore: 0.44},
    {tokenId: 'tok-b', x: -0.4, y: 0.0, clusterId: 0, weightedScore: 0.39},
    {tokenId: 'tok-c', x: 0.5, y: -0.2, clusterId: 1, weightedScore: 0.71},
    {tokenId: 'tok-d', x: 0.7, y: -0.1, clusterId: 1, weightedScore: 0.66},
  ],
};

describe('HypersphereGeometryOverlay', () => {
  test('renders one Voronoi cell and radius circle per cluster', () => {
    render(<HypersphereGeometryOverlay geometry={EPOCH_ZERO_GEOMETRY} />);

    expect(screen.getAllByTestId('overlay-voronoi-cell')).toHaveLength(
        EPOCH_ZERO_GEOMETRY.clusters.length);
    expect(screen.getAllByTestId('overlay-radius-circle')).toHaveLength(
        EPOCH_ZERO_GEOMETRY.clusters.length);
    expect(screen.getAllByTestId('overlay-token')).toHaveLength(
        EPOCH_ZERO_GEOMETRY.tokens.length);
  });

  test('shows projection metadata', () => {
    render(<HypersphereGeometryOverlay geometry={EPOCH_ZERO_GEOMETRY} />);

    const metadata = screen.getByTestId('projection-metadata');
    expect(metadata.textContent).toContain('Projection: pca');
    expect(metadata.textContent).toContain('Components 2');
    expect(metadata.textContent).toContain('Explained variance 0.93');
  });

  test('cluster selection highlights token scores panel', async () => {
    const user = userEvent.setup();
    render(<HypersphereGeometryOverlay geometry={EPOCH_ZERO_GEOMETRY} />);

    const cells = screen.getAllByTestId('overlay-voronoi-cell');
    await user.click(cells[0] as SVGPolygonElement);

    expect(screen.getByText('Cluster 0 tokens')).toBeTruthy();
    expect(screen.getByText('tok-a · score 0.440')).toBeTruthy();
    expect(screen.getByText('tok-b · score 0.390')).toBeTruthy();
  });

  test('rerenders on epoch switch without full page reload', () => {
    const nextEpochGeometry: EpochGeometryPayload = {
      ...EPOCH_ZERO_GEOMETRY,
      epoch: 1,
      clusters: [
        EPOCH_ZERO_GEOMETRY.clusters[0],
      ],
      tokens: [
        EPOCH_ZERO_GEOMETRY.tokens[0],
        EPOCH_ZERO_GEOMETRY.tokens[1],
      ],
    };

    const {rerender} = render(
        <HypersphereGeometryOverlay geometry={EPOCH_ZERO_GEOMETRY} />,
    );

    expect(screen.getByText('Epoch 0 · Mode power')).toBeTruthy();
    expect(screen.getAllByTestId('overlay-voronoi-cell')).toHaveLength(2);

    rerender(<HypersphereGeometryOverlay geometry={nextEpochGeometry} />);

    expect(screen.getByText('Epoch 1 · Mode power')).toBeTruthy();
    expect(screen.getAllByTestId('overlay-voronoi-cell')).toHaveLength(1);
    expect(screen.getAllByTestId('overlay-token')).toHaveLength(2);
  });
});
