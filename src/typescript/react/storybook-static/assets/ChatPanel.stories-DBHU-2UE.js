import{j as e}from"./utils-GoNbCZUx.js";import{B as f}from"./button-ClBirkNZ.js";import{T as b}from"./textarea-CjJeoL_0.js";import"./iframe-BsMz5XWX.js";import"./preload-helper-C1FmrZbK.js";import"./index-C7AspgAy.js";const{fn:j}=__STORYBOOK_MODULE_TEST__;function y({ready:n=!0,messages:o=[],onSend:h=j()}){return e.jsxs("div",{className:"flex w-96 flex-col space-y-3","data-testid":"chat-panel-demo",children:[e.jsx("div",{className:"min-h-32 rounded-lg border p-3","data-testid":"chat-messages",children:o.length===0?e.jsx("p",{className:"text-xs text-muted-foreground",children:"No chat messages yet."}):e.jsx("ul",{className:"space-y-2",children:o.map(s=>e.jsx("li",{className:s.role==="user"?"text-right":"text-left",children:e.jsx("span",{className:`inline-block rounded-md px-3 py-1 text-sm ${s.role==="user"?"bg-primary text-primary-foreground":"bg-muted text-muted-foreground"}`,children:s.content})},s.id))})}),e.jsxs("form",{className:"flex gap-2",onSubmit:s=>{s.preventDefault(),h()},children:[e.jsx(b,{placeholder:"Ask about the banana…",rows:2,disabled:!n,className:"flex-1"}),e.jsx(f,{type:"submit",disabled:!n,size:"sm",children:"Send"})]})]})}const T={title:"Panels/ChatPanel",component:y,parameters:{layout:"centered"},tags:["autodocs"]},a={},t={args:{messages:[{id:"1",role:"user",content:"Is this a ripe banana?"},{id:"2",role:"assistant",content:"Yes, the coloration suggests it is ripe."}]}},r={args:{ready:!1}};var i,c,l;a.parameters={...a.parameters,docs:{...(i=a.parameters)==null?void 0:i.docs,source:{originalSource:"{}",...(l=(c=a.parameters)==null?void 0:c.docs)==null?void 0:l.source}}};var d,m,p;t.parameters={...t.parameters,docs:{...(d=t.parameters)==null?void 0:d.docs,source:{originalSource:`{
  args: {
    messages: [{
      id: "1",
      role: "user",
      content: "Is this a ripe banana?"
    }, {
      id: "2",
      role: "assistant",
      content: "Yes, the coloration suggests it is ripe."
    }]
  }
}`,...(p=(m=t.parameters)==null?void 0:m.docs)==null?void 0:p.source}}};var u,x,g;r.parameters={...r.parameters,docs:{...(u=r.parameters)==null?void 0:u.docs,source:{originalSource:`{
  args: {
    ready: false
  }
}`,...(g=(x=r.parameters)==null?void 0:x.docs)==null?void 0:g.source}}};const B=["Empty","WithMessages","Unavailable"];export{a as Empty,r as Unavailable,t as WithMessages,B as __namedExportsOrder,T as default};
