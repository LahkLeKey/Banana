import {useEffect, useMemo, useState} from 'react';

import {fetchNotebookDocumentWithSource, fetchNotebookManifestWithSource,} from './api';
import {buildCellLookup} from './indexing';
import type {NotebookClientLoadState} from './types';

const INITIAL_STATE: NotebookClientLoadState = {
  manifest: null,
  notebook: null,
  manifestError: null,
  notebookError: null,
  manifestSource: '',
  notebookSource: '',
  loading: true,
};

export function useNotebookClient() {
  const [state, setState] = useState<NotebookClientLoadState>(INITIAL_STATE);

  useEffect(() => {
    let cancelled = false;

    Promise
        .allSettled([
          fetchNotebookManifestWithSource(),
          fetchNotebookDocumentWithSource(),
        ])
        .then(([manifestResult, notebookResult]) => {
          if (cancelled) {
            return;
          }

          setState({
            manifest: manifestResult.status === 'fulfilled' ?
                manifestResult.value.payload :
                null,
            notebook: notebookResult.status === 'fulfilled' ?
                notebookResult.value.payload :
                null,
            manifestError: manifestResult.status === 'rejected' ?
                'Notebook manifest is unavailable from API. Check API runtime and notebook endpoint.' :
                null,
            notebookError: notebookResult.status === 'rejected' ?
                'Notebook payload is unavailable from API. Check API runtime and notebook endpoint.' :
                null,
            manifestSource: manifestResult.status === 'fulfilled' ?
                manifestResult.value.source :
                '',
            notebookSource: notebookResult.status === 'fulfilled' ?
                notebookResult.value.source :
                '',
            loading: false,
          });
        });

    return () => {
      cancelled = true;
    };
  }, []);

  const cellLookup =
      useMemo(() => buildCellLookup(state.notebook), [state.notebook]);

  return {
    ...state,
    cellLookup,
    notebookCellCount: state.notebook?.cells?.length ?? 0,
  };
}
