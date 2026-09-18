const {chromium}=require('playwright');
const fs=require('fs'),assert=require('assert/strict');
const out=__dirname;fs.mkdirSync(out,{recursive:true});
const url=process.env.PAGE_URL || 'http://127.0.0.1:8765/blog/adversarial-examples-for-hashes/';
const stubAnalytics=page=>page.route(/https:\/\/[^/]*(?:google-analytics|googletagmanager)\.com\//,route=>route.fulfill({status:200,contentType:'text/javascript',body:''}));
(async()=>{const browser=await chromium.launch({headless:true});const report={widths:[],errors:[],analyticsStubbed:true};
for(const width of [375,768,1200,1620]){
 const page=await browser.newPage({viewport:{width,height:1000},deviceScaleFactor:1});
 await stubAnalytics(page);
 page.on('pageerror',e=>report.errors.push({width,type:'pageerror',message:e.message}));
 page.on('console',m=>{if(m.type()==='error')report.errors.push({width,type:'console',message:m.text()})});
 page.on('response',r=>{if(r.status()>=400)report.errors.push({width,type:'http',status:r.status(),url:r.url().replace('http://127.0.0.1:8765/','/')})});
 page.on('requestfailed',r=>report.errors.push({width,type:'requestfailed',url:r.url(),error:r.failure()}));
 await page.goto(url,{waitUntil:'networkidle'});await page.locator('#bits-vs-speed svg').waitFor();
 const result={width,hosts:[],profiles:[]};
 assert.equal(await page.locator('#hash-inspect option[value="chain-v3"]').count(),1);
 assert.equal(await page.locator('#hash-inspect option[value="chain256"],#hash-inspect option[value="chain-v2"]').count(),0);
 for(const [host,speed] of [['m2','26.26'],['xeon','28.31']]){
  if(host==='xeon'){
   await page.click('[data-host="xeon"]');await page.waitForFunction(()=>document.querySelector('#bits-vs-speed').dataset.motion==='running');result.animation=true;
   await page.waitForFunction(()=>document.querySelector('#bits-vs-speed').dataset.motion==='idle');
  }
  await page.selectOption('#hash-inspect','chain-v3');
  assert.match(await page.locator('.inspector-speed').innerText(),new RegExp(speed.replace('.','\\.')));
  assert.equal(await page.locator('.inspector-score').innerText(),'≥ 63 bits');
  if(await page.locator('.inspector-qualifications').getAttribute('open')===null)await page.locator('.inspector-qualifications summary').click();
  assert.match(await page.locator('.inspector-evidence').innerText(),/Frobenius/);
  assert.match(await page.locator('.inspector-key').innerText(),/64 independently random bytes/);
  assert.match(await page.locator('.inspector-scope').innerText(),/streaming/);
  assert.equal(await page.locator('#bits-vs-speed [data-point-id="chain-v3"]').count(),1);
  assert.equal(await page.locator('#bits-vs-speed [data-point-id="chain256"],#bits-vs-speed [data-point-id="chain-v2"]').count(),0);
  await page.locator('.figure-stage').screenshot({path:`${out}/${width}-${host}-chart.png`});
  if(host==='m2')await page.locator('.figure-inspector').screenshot({path:`${out}/${width}-v3-profile.png`});
  result.hosts.push({host,speed,score:63,chainhashPoint:'chain-v3'});
 }
 for(const id of ['umash','highway','xxh3-64']){
  await page.selectOption('#hash-inspect',id);if(await page.locator('.inspector-qualifications').getAttribute('open')===null)await page.locator('.inspector-qualifications summary').click();
  result.profiles.push({id,kind:await page.locator('.inspector-kind').innerText(),notes:await page.locator('.inspector-evidence').innerText()});
  assert((await page.locator('.inspector-key').innerText()).length>10);
 }
 const point=page.locator('#bits-vs-speed [data-point-id="chain-v3"]');await point.focus();await page.keyboard.press('Enter');
 assert.match(await page.locator('.inspector-name').innerText(),/ChainHash v3/);await page.keyboard.press('ArrowRight');
 assert.notEqual(await page.evaluate(()=>document.activeElement.dataset.pointId),'chain-v3');await page.keyboard.press('Escape');assert(await page.locator('.figure-inspector').isHidden());result.keyboard=true;
 await page.evaluate(()=>document.querySelector('.figure-stage').scrollIntoView());
 result.layout=await page.evaluate(()=>({noOverflow:document.documentElement.scrollWidth<=innerWidth,tocLinks:[...document.querySelectorAll('[data-toc-list] a')].filter(a=>!document.getElementById(a.hash.slice(1))).map(a=>a.hash),styles:[...document.styleSheets].map(x=>x.href).filter(Boolean).map(x=>new URL(x).pathname)}));
 assert(result.layout.noOverflow);assert.equal(result.layout.tocLinks.length,0);
 const toc=width<1200?page.locator('.toc-mobile'):page.locator('.toc-desktop');
 if(await toc.isVisible()){
  if(width<1200)await toc.locator('summary').click();
  const link=toc.locator('a[href="#appendix-chainhash"]');await link.click();
  assert.equal(new URL(page.url()).hash,'#appendix-chainhash');result.toc=true;
 }
 await page.locator('#appendix-chainhash').scrollIntoViewIfNeeded();await page.screenshot({path:`${out}/${width}-appendix.png`});
 assert.equal(await page.locator('#proof-table tr[data-hash-id="chain-v3"] td').count(),15);
 assert.match(await page.locator('#proof-table tr[data-hash-id="chain-v3"]').innerText(),/155.14/);
 report.widths.push(result);await page.close();
}
const fallback=await browser.newPage({javaScriptEnabled:false,viewport:{width:375,height:900}});await fallback.goto(url);const img=fallback.locator('#bits-vs-speed img');assert(await img.isVisible());assert(await img.evaluate(e=>e.complete&&e.naturalWidth>0));report.noScriptFallback=true;await fallback.close();
const reduced=await browser.newPage({reducedMotion:'reduce'});await stubAnalytics(reduced);await reduced.goto(url);await reduced.locator('#bits-vs-speed svg').waitFor();await reduced.click('[data-host="xeon"]');await reduced.waitForFunction(()=>document.querySelector('[data-host="xeon"]').getAttribute('aria-pressed')==='true');assert.equal(await reduced.locator('#bits-vs-speed').getAttribute('data-motion'),'idle');report.reducedMotion=true;await reduced.close();
fs.writeFileSync(`${out}/checks.json`,JSON.stringify(report,null,2)+'\n');await browser.close();console.log(JSON.stringify(report,null,2));assert.equal(report.errors.length,0);
})().catch(e=>{console.error(e);process.exit(1)});
