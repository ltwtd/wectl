export module wectl.web.js_gl;
import <string>;
import wectl.web.gl_vert;
import wectl.web.gl_eye;

export namespace wectl::web::js {
	std::string get_webgl_engine() {
		return "</script>\n<script id=\"wectl-vs\" type=\"x-shader/x-vertex\">\n" + wectl::web::gl::get_vertex_shader() +
		       "\n</script>\n<script id=\"wectl-fs\" type=\"x-shader/x-fragment\">\n" + wectl::web::gl::get_eye_shader() +
		       "\n</script>\n<script>\n" + R"JS((function(){
	const addC=(id,z,blend)=>{let c=document.getElementById(id);if(!c){c=document.createElement('canvas');c.id=id;document.body.prepend(c);}Object.assign(c.style,{position:'fixed',inset:'0',width:'100vw',height:'100vh',zIndex:z,pointerEvents:'none',mixBlendMode:blend||'normal'});return c;};
	const glC=addC('gl-canvas','0'),fxC=addC('fx-canvas','1','screen'),fx=fxC.getContext('2d');
	glC.after(fxC);
	let gl=null;try{gl=glC.getContext('webgl2',{antialias:true,powerPreference:'high-performance'});}catch(e){}

	let mode=0,tNX=.5,tNY=.5,sNX=.5,sNY=.5,tRX=innerWidth*.5,tRY=innerHeight*.5,sRX=tRX,sRY=tRY;
	document.querySelectorAll('.mode-btn').forEach(b=>b.onclick=()=>{document.querySelectorAll('.mode-btn').forEach(x=>x.classList.remove('active'));b.classList.add('active');mode=+b.dataset.mode||0;});
	onmousemove=e=>{tRX=e.clientX;tRY=e.clientY;tNX=tRX/Math.max(1,innerWidth);tNY=1-tRY/Math.max(1,innerHeight);};

	const bez=(x1,y1,x2,y2)=>t=>{let x=t,u,d;for(let i=0;i<6;i++){u=1-x;d=3*u*u*x*x1+3*u*x*x*x2+x*x*x-t;x-=d/(3*u*u*x1+6*u*x*(x2-x1)+3*x*x*(1-x2)||1e-5);}u=1-x;return 3*u*u*x*y1+3*u*x*x*y2+x*x*x;};
	const ease=bez(.22,1,.36,1);
	let zoom=1,zFrom=1,zTo=1,zStart=0,zDur=2200,zOut=false,nextZ=performance.now()+3e3+Math.random()*4e3;

	let uRes,uTime,uMouse,uScroll,uZoom,uMode,glOk=false;
	if(gl){
		const comp=(t,s)=>{const sh=gl.createShader(t);gl.shaderSource(sh,s);gl.compileShader(sh);return gl.getShaderParameter(sh,gl.COMPILE_STATUS)?sh:null;};
		const vs=comp(gl.VERTEX_SHADER,document.getElementById('wectl-vs').textContent.trim()),fs=comp(gl.FRAGMENT_SHADER,document.getElementById('wectl-fs').textContent.trim());
		if(vs&&fs){
			const p=gl.createProgram();gl.attachShader(p,vs);gl.attachShader(p,fs);gl.linkProgram(p);
			if(gl.getProgramParameter(p,gl.LINK_STATUS)){
				gl.useProgram(p);gl.bindBuffer(gl.ARRAY_BUFFER,gl.createBuffer());
				gl.bufferData(gl.ARRAY_BUFFER,new Float32Array([-1,-1,1,-1,-1,1,-1,1,1,-1,1,1]),gl.STATIC_DRAW);
				gl.enableVertexAttribArray(0);gl.vertexAttribPointer(0,2,gl.FLOAT,false,0,0);
				[uRes,uTime,uMouse,uScroll,uZoom,uMode]=['u_res','u_time','u_mouse','u_scroll','u_zoom','u_mode'].map(n=>gl.getUniformLocation(p,n));
				glOk=true;
			}
		}
	}

	// 420 сверхмелких точечных частиц вокруг чистого шара (без шлейфов)
	const parts=Array.from({length:420},()=>({
		a:Math.random()*6.283,
		rad:.24+Math.random()*.68,
		tilt:(Math.random()-.5)*1.1,
		spd:(.05+Math.random()*.14)*(Math.random()>.5?1:-1),
		wf:.4+Math.random()*1.4,
		wa:.010+Math.random()*.032,
		ph:Math.random()*6.283,
		r:.35+Math.random()*.85
	}));

	const sparks=[];
	let nextSp=performance.now()+300;

	const resize=()=>{
		const dpr=Math.max(devicePixelRatio||1,2);
		glC.width=fxC.width=Math.round(innerWidth*dpr);
		glC.height=fxC.height=Math.round(innerHeight*dpr);
		if(glOk)gl.viewport(0,0,glC.width,glC.height);
		fx.setTransform(dpr,0,0,dpr,0,0);
	};
	onresize=resize;resize();

	const t0=performance.now();
	let last=0;
	const loop=now=>{
		requestAnimationFrame(loop);
		if(now-last<16.5)return;
		last=now;

		sNX+=(tNX-sNX)*.032;sNY+=(tNY-sNY)*.032;sRX+=(tRX-sRX)*.032;sRY+=(tRY-sRY)*.032;
		if(now>=nextZ){zOut=!zOut;zFrom=zoom;zTo=zOut?1.38+Math.random()*.26:1;zStart=now;zDur=1800+Math.random()*900;nextZ=now+zDur+3e3+Math.random()*5e3;}
		zoom=now<=zStart+zDur?zFrom+(zTo-zFrom)*ease(Math.min(1,(now-zStart)/zDur)):zTo;

		const t=(now-t0)*.001,scr=window.__WECTL_SCROLL||0;
		if(glOk){
			gl.uniform2f(uRes,glC.width,glC.height);gl.uniform1f(uTime,t);gl.uniform2f(uMouse,sNX,sNY);
			gl.uniform1f(uScroll,scr);gl.uniform1f(uZoom,zoom);gl.uniform1i(uMode,mode);
			gl.drawArrays(gl.TRIANGLES,0,6);
		}

		const W=innerWidth,H=innerHeight,minD=Math.min(W,H);
		const px=(sRX-W*.5)*.075,py=(sRY-H*.5)*.075,cx=W*.5+px,cy=H*.44+py-scr*.06,R=(minD*.46)/zoom;
		fx.clearRect(0,0,W,H);

		for(let i=0;i<parts.length;i++){
			const p=parts[i],ang=p.a+t*p.spd+Math.sin(t*p.wf+p.ph)*.14;
			const rad=(p.rad+Math.cos(t*p.wf*.8+p.ph)*p.wa)*(minD*.95)/zoom;
			const lx=Math.cos(ang)*rad,ly=Math.sin(ang)*rad*.42,zDepth=Math.sin(ang);
			const sx=cx+lx*Math.cos(p.tilt)-ly*Math.sin(p.tilt)+px*(zDepth*.25);
			const sy=cy+lx*Math.sin(p.tilt)+ly*Math.cos(p.tilt)+py*(zDepth*.25);
			if(zDepth<0&&Math.hypot(sx-cx,sy-cy)<R*.98)continue;

			const lit=Math.max(.15,Math.min(1,(sx-cx)/R*.65+.60));
			const alpha=(.18+.52*lit)*(.65+.35*zDepth);
			fx.fillStyle='rgba(255,255,255,'+Math.max(.05,alpha)+')';
			fx.beginPath();fx.arc(sx,sy,(p.r*(.85+.3*zDepth))/Math.sqrt(zoom),0,6.283);fx.fill();
		}

		// Мелкие точечные белые искры без длинных хвостов
		if(now>=nextSp){
			const isA=Math.random()>.45;
			const ang=(isA?(t*.85):(-t*1.05))+(Math.random()>.5?0:3.14159);
			const tl=isA?-.22:.48,rx=isA?R*1.91:R*.52,ry=isA?R*.35:R*1.61;
			const lx=Math.cos(ang)*rx,ly=Math.sin(ang)*ry;
			const sx=cx+lx*Math.cos(tl)-ly*Math.sin(tl),sy=cy+lx*Math.sin(tl)+ly*Math.cos(tl);
			for(let i=0;i<6+Math.random()*8;i++){
				const sp=Math.random()*6.283,v=1.2+Math.random()*2.8;
				sparks.push({x:sx,y:sy,vx:Math.cos(sp)*v,vy:Math.sin(sp)*v,l:1,d:.025+Math.random()*.03,r:.5+Math.random()*.8});
			}
			nextSp=now+350+Math.random()*950;
		}

		for(let i=sparks.length-1;i>=0;i--){
			const p=sparks[i];p.x+=p.vx;p.y+=p.vy;p.vx*=.95;p.vy*=.95;p.l-=p.d;
			if(p.l<=0){sparks.splice(i,1);continue;}
			fx.fillStyle='rgba(255,255,255,'+(p.l*.95)+')';
			fx.beginPath();fx.arc(p.x,p.y,p.r*p.l,0,6.283);fx.fill();
		}
	};
	requestAnimationFrame(loop);
})();)JS";
	}
}

