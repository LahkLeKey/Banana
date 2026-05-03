import{T as i}from"./textarea-CjJeoL_0.js";import"./utils-GoNbCZUx.js";import"./iframe-BsMz5XWX.js";import"./preload-helper-C1FmrZbK.js";const T={title:"UI/Textarea",component:i,parameters:{layout:"centered"},tags:["autodocs"],argTypes:{disabled:{control:"boolean"},placeholder:{control:"text"},rows:{control:{type:"range",min:2,max:10}}}},e={args:{placeholder:"Type a message…",rows:3}},a={args:{value:"Tell me about this banana.",readOnly:!0,rows:3}},r={args:{placeholder:"Sending…",disabled:!0,rows:3}};var s,o,t;e.parameters={...e.parameters,docs:{...(s=e.parameters)==null?void 0:s.docs,source:{originalSource:`{
  args: {
    placeholder: "Type a message…",
    rows: 3
  }
}`,...(t=(o=e.parameters)==null?void 0:o.docs)==null?void 0:t.source}}};var n,l,c;a.parameters={...a.parameters,docs:{...(n=a.parameters)==null?void 0:n.docs,source:{originalSource:`{
  args: {
    value: "Tell me about this banana.",
    readOnly: true,
    rows: 3
  }
}`,...(c=(l=a.parameters)==null?void 0:l.docs)==null?void 0:c.source}}};var d,p,m;r.parameters={...r.parameters,docs:{...(d=r.parameters)==null?void 0:d.docs,source:{originalSource:`{
  args: {
    placeholder: "Sending…",
    disabled: true,
    rows: 3
  }
}`,...(m=(p=r.parameters)==null?void 0:p.docs)==null?void 0:m.source}}};const w=["Default","WithContent","Disabled"];export{e as Default,r as Disabled,a as WithContent,w as __namedExportsOrder,T as default};
