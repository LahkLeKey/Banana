// Slice 023 -- preload happy-dom globals BEFORE RTL imports document.
import {GlobalRegistrator} from '@happy-dom/global-registrator';

if (!(globalThis as unknown as {document?: Document}).document) {
  GlobalRegistrator.register({url: 'http://app.example/'});
}
(globalThis as unknown as {
  IS_REACT_ACT_ENVIRONMENT: boolean
}).IS_REACT_ACT_ENVIRONMENT = true;
