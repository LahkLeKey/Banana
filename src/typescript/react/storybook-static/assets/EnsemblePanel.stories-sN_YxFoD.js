import{j as e}from"./utils-GoNbCZUx.js";import{B as E}from"./button-ClBirkNZ.js";import{I as _}from"./input-BB26yZI8.js";import{C as v,a as y}from"./card-CM2Gocz3.js";import{A,a as I,b as T}from"./alert-CaZWDtq3.js";import"./iframe-BsMz5XWX.js";import"./preload-helper-C1FmrZbK.js";import"./index-C7AspgAy.js";const{fn:B}=__STORYBOOK_MODULE_TEST__;function N({loading:o=!1,error:n=null,verdict:c=null,onSubmit:P=B()}){return e.jsxs("div",{className:"w-96 space-y-4","data-testid":"ensemble-panel-demo",children:[e.jsxs("form",{className:"space-y-2",onSubmit:S=>{S.preventDefault(),P()},"data-testid":"ensemble-form",children:[e.jsx(_,{placeholder:"Paste an image URL…",disabled:o}),e.jsx(E,{type:"submit",disabled:o,className:"w-full",children:o?"Predicting…":"Predict ensemble"})]}),n!==null&&e.jsxs(A,{variant:"destructive","data-testid":"ensemble-error-alert",children:[e.jsx(I,{children:"Prediction failed"}),e.jsx(T,{children:n})]}),c!==null&&e.jsx(v,{"data-testid":"ensemble-verdict-surface",children:e.jsx(y,{className:"pt-6",children:e.jsx("p",{className:"font-medium text-foreground","data-testid":"ensemble-verdict-copy",children:c})})})]})}const K={title:"Panels/EnsemblePanel",component:N,parameters:{layout:"centered"},tags:["autodocs"]},r={},s={args:{loading:!0}},a={args:{verdict:"Banana (confidence: 0.97)"}},t={args:{error:"The API could not process the image. Please try again."}};var d,i,l;r.parameters={...r.parameters,docs:{...(d=r.parameters)==null?void 0:d.docs,source:{originalSource:"{}",...(l=(i=r.parameters)==null?void 0:i.docs)==null?void 0:l.source}}};var m,p,u;s.parameters={...s.parameters,docs:{...(m=s.parameters)==null?void 0:m.docs,source:{originalSource:`{
  args: {
    loading: true
  }
}`,...(u=(p=s.parameters)==null?void 0:p.docs)==null?void 0:u.source}}};var g,f,x;a.parameters={...a.parameters,docs:{...(g=a.parameters)==null?void 0:g.docs,source:{originalSource:`{
  args: {
    verdict: "Banana (confidence: 0.97)"
  }
}`,...(x=(f=a.parameters)==null?void 0:f.docs)==null?void 0:x.source}}};var h,b,j;t.parameters={...t.parameters,docs:{...(h=t.parameters)==null?void 0:h.docs,source:{originalSource:`{
  args: {
    error: "The API could not process the image. Please try again."
  }
}`,...(j=(b=t.parameters)==null?void 0:b.docs)==null?void 0:j.source}}};const M=["Idle","Loading","VerdictSuccess","VerdictError"];export{r as Idle,s as Loading,t as VerdictError,a as VerdictSuccess,M as __namedExportsOrder,K as default};
