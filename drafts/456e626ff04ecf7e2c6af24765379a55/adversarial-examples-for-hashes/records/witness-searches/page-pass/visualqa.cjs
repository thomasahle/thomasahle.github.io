const {chromium}=require('playwright');
const fs=require('fs'),path=require('path'),assert=require('assert/strict');
const base=process.env.QA_BASE||'http://127.0.0.1:8767/blog/adversarial-examples-for-hashes/';
const out=__dirname;
(async()=>{
 const browser=await chromium.launch({headless:true});
 const report={browser:browser.version(),widths:[],errors:[],httpErrors:[]};
 for(const width of [375,768,1200,1620]){
  const ctx=await browser.newContext({viewport:{width,height:1000},deviceScaleFactor:1});
  await ctx.route('**/*googletagmanager.com/**',r=>r.fulfill({status:200,body:''}));
  const page=await ctx.newPage();
  page.on('pageerror',e=>report.errors.push({width,message:e.message}));
  page.on('response',r=>{if(r.status()>=400&&r.url().startsWith(base))report.httpErrors.push({width,url:r.url(),status:r.status()})});
  await page.goto(base,{waitUntil:'networkidle'});
  await page.waitForSelector('#bits-vs-speed svg [data-point-id="muse-v2"]');
  await page.addStyleTag({content:'html{scroll-behavior:auto!important}'});
  const chart=page.locator('.feature-chart');
  await chart.scrollIntoViewIfNeeded();
  const record={width,noOverflow:await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth),profiles:[],demos:[]};
  assert(record.noOverflow,'document overflow '+width);
  for(const host of ['m2','xeon']){
   if(host==='xeon'){
    await page.locator('[data-host="xeon"]').click();
    await page.waitForFunction(()=>document.querySelector('#bits-vs-speed').dataset.motion==='running');
    record.animation=true;
    await page.waitForFunction(()=>document.querySelector('#bits-vs-speed').dataset.motion==='idle');
   }
   for(const id of ['xxh3-64','t1ha','ahash','muse-v2','umash128','chain-v3']){
    await page.locator('#hash-inspect').selectOption(id);
    const inspector=page.locator('.figure-inspector');await inspector.waitFor({state:'visible'});
    const details=inspector.locator('.inspector-qualifications');await details.locator('summary').click();
    const text=await inspector.innerText();
    assert(text.includes('Collision evidence')||text.includes('Proved bound')||text.includes('Claimed bound'));
    const expected={'xxh3-64':'12.47','t1ha':'29.19','ahash':'22.18','muse-v2':'19.45'}[id];
    if(expected)assert(text.includes(expected),id+' score');
    record.profiles.push({host,id,expanded:await details.getAttribute('open')!==null,score:await inspector.locator('.inspector-score').innerText()});
   }
   await page.locator('#hash-inspect').selectOption('xxh3-64');
   await chart.screenshot({path:path.join(out,`${width}-${host}-chart.png`)});
  }
  const point=page.locator('#bits-vs-speed [data-point-id="xxh3-64"]');
  await point.focus();await page.keyboard.press('ArrowRight');
  record.keyboardMoved=await page.evaluate(()=>document.activeElement.dataset.pointId!=='xxh3-64'&&!!document.activeElement.dataset.pointId);assert(record.keyboardMoved);
  await page.keyboard.press('Enter');assert(await page.locator('.figure-inspector').isVisible());
  await page.keyboard.press('Escape');assert(await page.locator('.figure-inspector').isHidden());record.escape=true;
  await point.hover({force:true});assert(await page.locator('.figure-peek').isVisible());record.hover=await page.locator('.figure-peek').innerText();assert(record.hover.includes('12.47'));
  for(const anchor of ['appendix-xxh3-64','appendix-t1ha2','appendix-ahash']){
   const demo=page.locator(`[data-demo="${anchor}"]`);await demo.scrollIntoViewIfNeeded();
   if(!(await demo.getAttribute('open')!==null))await demo.locator('summary').click();
   await page.waitForFunction(anchor=>document.querySelector(`[data-demo="${anchor}"]`).dataset.state==='ready',anchor,{timeout:30000});
   const txt=await demo.innerText();assert(!txt.includes('Unavailable'));
   record.demos.push({anchor,state:await demo.getAttribute('data-state'),text:txt.slice(0,1200)});
   if(width===375)await demo.screenshot({path:path.join(out,`${width}-${anchor}.png`)});
  }
  const toc=await page.evaluate(()=>[...document.querySelectorAll('[data-toc-list] a')].map(a=>({hash:a.hash,exists:!!document.getElementById(a.hash.slice(1))})));
  assert(toc.every(t=>t.exists));assert(toc.some(t=>t.hash==='#appendix-museair-v2'));record.tocLinks=toc.length;
  await page.locator('#appendix-museair-v2').scrollIntoViewIfNeeded();
  await page.screenshot({path:path.join(out,`${width}-muse-v2.png`)});
  report.widths.push(record);await ctx.close();
 }
 const nojs=await browser.newContext({javaScriptEnabled:false,viewport:{width:375,height:1000}});const page=await nojs.newPage();await page.goto(base,{waitUntil:'networkidle'});
 report.fallback=await page.locator('.feature-chart img').evaluateAll(imgs=>imgs.some(img=>img.complete&&img.naturalWidth>0&&img.getBoundingClientRect().height>0));assert(report.fallback);
 await page.locator('.feature-chart').screenshot({path:path.join(out,'375-static-fallback.png')});await nojs.close();
 const reduced=await browser.newContext({reducedMotion:'reduce',viewport:{width:1200,height:1000}});const rp=await reduced.newPage();await rp.goto(base,{waitUntil:'networkidle'});await rp.waitForSelector('#bits-vs-speed svg');await rp.locator('[data-host="xeon"]').click();await rp.waitForTimeout(100);report.reducedMotion=await rp.locator('#bits-vs-speed').getAttribute('data-motion');assert.notEqual(report.reducedMotion,'running');await reduced.close();
 await browser.close();fs.writeFileSync(path.join(out,'visualqa.json'),JSON.stringify(report,null,2)+'\n');assert.equal(report.errors.length,0);assert.equal(report.httpErrors.length,0);console.log('PASS',JSON.stringify({widths:report.widths.map(r=>r.width),fallback:report.fallback,errors:report.errors,httpErrors:report.httpErrors}));
})().catch(e=>{console.error(e);process.exit(1)});
