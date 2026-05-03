import{I as h}from"./input-BB26yZI8.js";import"./utils-GoNbCZUx.js";import"./iframe-BsMz5XWX.js";import"./preload-helper-C1FmrZbK.js";const I={title:"UI/Input",component:h,parameters:{layout:"centered"},tags:["autodocs"],argTypes:{type:{control:"select",options:["text","number","email","password","file"]},disabled:{control:"boolean"},placeholder:{control:"text"}}},e={args:{placeholder:"Paste an image URL…"}},a={args:{value:"https://example.com/banana.jpg",readOnly:!0}},r={args:{placeholder:"Disabled input",disabled:!0}},s={args:{type:"file",accept:"image/*"}};var t,o,n;e.parameters={...e.parameters,docs:{...(t=e.parameters)==null?void 0:t.docs,source:{originalSource:`{
  args: {
    placeholder: "Paste an image URL…"
  }
}`,...(n=(o=e.parameters)==null?void 0:o.docs)==null?void 0:n.source}}};var p,c,l;a.parameters={...a.parameters,docs:{...(p=a.parameters)==null?void 0:p.docs,source:{originalSource:`{
  args: {
    value: "https://example.com/banana.jpg",
    readOnly: true
  }
}`,...(l=(c=a.parameters)==null?void 0:c.docs)==null?void 0:l.source}}};var d,i,m;r.parameters={...r.parameters,docs:{...(d=r.parameters)==null?void 0:d.docs,source:{originalSource:`{
  args: {
    placeholder: "Disabled input",
    disabled: true
  }
}`,...(m=(i=r.parameters)==null?void 0:i.docs)==null?void 0:m.source}}};var u,g,b;s.parameters={...s.parameters,docs:{...(u=s.parameters)==null?void 0:u.docs,source:{originalSource:`{
  args: {
    type: "file",
    accept: "image/*"
  }
}`,...(b=(g=s.parameters)==null?void 0:g.docs)==null?void 0:b.source}}};const S=["Default","WithValue","Disabled","FileInput"];export{e as Default,r as Disabled,s as FileInput,a as WithValue,S as __namedExportsOrder,I as default};
