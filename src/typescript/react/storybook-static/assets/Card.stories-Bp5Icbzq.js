import{j as e}from"./utils-GoNbCZUx.js";import{C as n,b as p,c as C,d as h,a as m,e as u}from"./card-CM2Gocz3.js";import{B as x}from"./button-ClBirkNZ.js";import"./iframe-BsMz5XWX.js";import"./preload-helper-C1FmrZbK.js";import"./index-C7AspgAy.js";const B={title:"UI/Card",component:n,parameters:{layout:"centered"},tags:["autodocs"],decorators:[l=>e.jsx("div",{className:"w-96",children:e.jsx(l,{})})]},r={render:()=>e.jsxs(n,{children:[e.jsxs(p,{children:[e.jsx(C,{children:"Ensemble Result"}),e.jsx(h,{children:"Prediction from the banana classifier"})]}),e.jsx(m,{children:e.jsx("p",{className:"text-sm text-muted-foreground",children:"Based on the image, the model ensemble determined this is a banana with high confidence."})}),e.jsx(u,{children:e.jsx(x,{size:"sm",variant:"outline",children:"New prediction"})})]})},a={render:()=>e.jsx(n,{children:e.jsx(m,{className:"pt-6",children:e.jsx("p",{className:"text-sm",children:"Simple card with only content."})})})};var t,s,d;r.parameters={...r.parameters,docs:{...(t=r.parameters)==null?void 0:t.docs,source:{originalSource:`{
  render: () => <Card>\r
            <CardHeader>\r
                <CardTitle>Ensemble Result</CardTitle>\r
                <CardDescription>Prediction from the banana classifier</CardDescription>\r
            </CardHeader>\r
            <CardContent>\r
                <p className="text-sm text-muted-foreground">\r
                    Based on the image, the model ensemble determined this is a banana with high confidence.\r
                </p>\r
            </CardContent>\r
            <CardFooter>\r
                <Button size="sm" variant="outline">\r
                    New prediction\r
                </Button>\r
            </CardFooter>\r
        </Card>
}`,...(d=(s=r.parameters)==null?void 0:s.docs)==null?void 0:d.source}}};var o,i,c;a.parameters={...a.parameters,docs:{...(o=a.parameters)==null?void 0:o.docs,source:{originalSource:`{
  render: () => <Card>\r
            <CardContent className="pt-6">\r
                <p className="text-sm">Simple card with only content.</p>\r
            </CardContent>\r
        </Card>
}`,...(c=(i=a.parameters)==null?void 0:i.docs)==null?void 0:c.source}}};const D=["Default","Minimal"];export{r as Default,a as Minimal,D as __namedExportsOrder,B as default};
