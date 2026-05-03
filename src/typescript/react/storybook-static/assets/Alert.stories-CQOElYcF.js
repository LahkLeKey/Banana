import{j as e}from"./utils-GoNbCZUx.js";import{A as s,a as d,b as m}from"./alert-CaZWDtq3.js";import"./index-C7AspgAy.js";import"./iframe-BsMz5XWX.js";import"./preload-helper-C1FmrZbK.js";const f={title:"UI/Alert",component:s,parameters:{layout:"centered"},tags:["autodocs"],decorators:[p=>e.jsx("div",{className:"w-96",children:e.jsx(p,{})})],argTypes:{variant:{control:"select",options:["default","destructive"]}}},r={render:()=>e.jsxs(s,{children:[e.jsx(d,{children:"Prediction ready"}),e.jsx(m,{children:"The ensemble has returned a verdict for your image."})]})},t={render:()=>e.jsxs(s,{variant:"destructive",children:[e.jsx(d,{children:"Prediction failed"}),e.jsx(m,{children:"The API could not process the image. Check your connection and try again."})]})};var n,o,a;r.parameters={...r.parameters,docs:{...(n=r.parameters)==null?void 0:n.docs,source:{originalSource:`{
  render: () => <Alert>\r
            <AlertTitle>Prediction ready</AlertTitle>\r
            <AlertDescription>The ensemble has returned a verdict for your image.</AlertDescription>\r
        </Alert>
}`,...(a=(o=r.parameters)==null?void 0:o.docs)==null?void 0:a.source}}};var i,c,l;t.parameters={...t.parameters,docs:{...(i=t.parameters)==null?void 0:i.docs,source:{originalSource:`{
  render: () => <Alert variant="destructive">\r
            <AlertTitle>Prediction failed</AlertTitle>\r
            <AlertDescription>\r
                The API could not process the image. Check your connection and try again.\r
            </AlertDescription>\r
        </Alert>
}`,...(l=(c=t.parameters)==null?void 0:c.docs)==null?void 0:l.source}}};const g=["Default","Destructive"];export{r as Default,t as Destructive,g as __namedExportsOrder,f as default};
