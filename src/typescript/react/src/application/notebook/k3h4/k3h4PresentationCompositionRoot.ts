import {createK3h4PresentationMapperAdapter,} from '../../../infrastructure/notebook/k3h4/k3h4PresentationMapperAdapter';

import {createK3h4PresentationApplicationService, type K3h4PresentationApplicationService,} from './k3h4PresentationApplicationService';

/** Composes the notebook presentation mapper service for k3h4 screens. */
export function composeK3h4PresentationApplicationService():
    K3h4PresentationApplicationService {
  const mapperPort = createK3h4PresentationMapperAdapter();
  return createK3h4PresentationApplicationService(mapperPort);
}
