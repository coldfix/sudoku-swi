var e={},n=[],r=e=>e,t=n.map,l=Array.isArray,o="undefined"!=typeof requestAnimationFrame?requestAnimationFrame:setTimeout,i=e=>{var n="";if("string"==typeof e)return e;if(l(e))for(var r,t=0;t<e.length;t++)(r=i(e[t]))&&(n+=(n&&" ")+r);else for(var t in e)e[t]&&(n+=(n&&" ")+t);return n},a=(e,n)=>{for(var r in{...e,...n})if("function"==typeof(l(n[r]=e[r])?n[r][0]:n[r]));else if(e[r]!==n[r])return!0},u=e=>null==e?e:e.key,d=(e,n,r,t,l,o)=>{if("key"===n);else if("style"===n)for(var a in{...r,...t})r=null==t||null==t[a]?"":t[a],"-"===a[0]?e[n].setProperty(a,r):e[n][a]=r;else"o"===n[0]&&"n"===n[1]?((e.tag||(e.tag={}))[n=n.slice(2)]=t)?r||e.addEventListener(n,l):e.removeEventListener(n,l):!o&&"list"!==n&&"form"!==n&&n in e?e[n]=null==t?"":t:null==t||!1===t||"class"===n&&!(t=i(t))?e.removeAttribute(n):e.setAttribute(n,t)},f=(e,n,r)=>{var t=e.props,l=3===e.tag?document.createTextNode(e.type):(r=r||"svg"===e.type)?document.createElementNS("http://www.w3.org/2000/svg",e.type,{is:t.is}):document.createElement(e.type,{is:t.is});for(var o in t)d(l,o,null,t[o],n,r);for(var i=0;i<e.children.length;i++)l.appendChild(f(e.children[i]=p(e.children[i]),n,r));return e.node=l},s=(e,n,r,t,l,o)=>{if(r===t);else if(null!=r&&3===r.tag&&3===t.tag)r.type!==t.type&&(n.nodeValue=t.type);else if(null==r||r.type!==t.type)n=e.insertBefore(f(t=p(t),l,o),n),null!=r&&e.removeChild(r.node);else{var i,a,m,v,y=r.props,c=t.props,h=r.children,g=t.children,x=0,w=0,C=h.length-1,k=g.length-1;for(var A in o=o||"svg"===t.type,{...y,...c})("value"===A||"selected"===A||"checked"===A?n[A]:y[A])!==c[A]&&d(n,A,y[A],c[A],l,o);for(;w<=k&&x<=C&&null!=(m=u(h[x]))&&m===u(g[w]);)s(n,h[x].node,h[x],g[w]=p(g[w++],h[x++]),l,o);for(;w<=k&&x<=C&&null!=(m=u(h[C]))&&m===u(g[k]);)s(n,h[C].node,h[C],g[k]=p(g[k--],h[C--]),l,o);if(x>C)for(;w<=k;)n.insertBefore(f(g[w]=p(g[w++]),l,o),(a=h[x])&&a.node);else if(w>k)for(;x<=C;)n.removeChild(h[x++].node);else{var N={},E={};for(A=x;A<=C;A++)null!=(m=h[A].key)&&(N[m]=h[A]);for(;w<=k;)m=u(a=h[x]),v=u(g[w]=p(g[w],a)),E[m]||null!=v&&v===u(h[x+1])?(null==m&&n.removeChild(a.node),x++):null==v||1===r.tag?(null==m&&(s(n,a&&a.node,a,g[w],l,o),w++),x++):(m===v?(s(n,a.node,a,g[w],l,o),E[v]=!0,x++):null!=(i=N[v])?(s(n,n.insertBefore(i.node,a&&a.node),i,g[w],l,o),E[v]=!0):s(n,a&&a.node,null,g[w],l,o),w++);for(;x<=C;)null==u(a=h[x++])&&n.removeChild(a.node);for(var A in N)null==E[A]&&n.removeChild(N[A].node)}}return t.node=n},p=(e,n)=>!0!==e&&!1!==e&&e?"function"==typeof e.tag?((!n||null==n.memo||((e,n)=>{for(var r in e)if(e[r]!==n[r])return!0;for(var r in n)if(e[r]!==n[r])return!0})(n.memo,e.memo))&&((n=e.tag(e.memo)).memo=e.memo),n):e:text(""),m=n=>3===n.nodeType?text(n.nodeValue,n):v(n.nodeName.toLowerCase(),e,t.call(n.childNodes,m),n,null,1),v=(e,n,r,t,l,o)=>({type:e,props:n,children:r,node:t,key:l,tag:o});export var memo=(e,n)=>({tag:e,memo:n});export var text=(r,t)=>v(r,e,n,t,null,3);export var h=(e,r,t)=>v(e,r,l(t)?t:null==t?n:[t],null,r.key);export var app=e=>{var n,t,i=e.view,u=e.node,d=e.subscriptions,f=u&&m(u),p=[],v=e=>{t!==e&&(t=e,d&&(p=((e,n,r)=>{for(var t,l,o=[],i=0;i<e.length||i<n.length;i++)t=e[i],l=n[i],o.push(l&&!0!==l?!t||l[0]!==t[0]||a(l[1],t[1])?[l[0],l[1],l[0](r,l[1]),t&&t[2]()]:t:t&&t[2]());return o})(p,d(t),y)),i&&!n&&o(h,n=!0))},y=(e.middleware||r)((e,n)=>"function"==typeof e?y(e(t,n)):l(e)?"function"==typeof e[0]?y(e[0],e[1]):e.slice(1).map(e=>e&&!0!==e&&e[0](y,e[1]),v(e[0])):v(e)),c=function(e){y(this.tag[e.type],e)},h=()=>u=s(u.parentNode,u,f,f=i(t),c,n=!1);y(e.init)};
//# sourceMappingURL=hyperapp.js.map
