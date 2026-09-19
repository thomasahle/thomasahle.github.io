// Run with PLAYWRIGHT_MODULE set if Playwright is installed outside this checkout.
// Serve the website root on localhost:8765, or set PAGE_URL.
const {chromium}=require(process.env.PLAYWRIGHT_MODULE || 'playwright');
const fs=require('fs'),assert=require('assert/strict');
const out=__dirname;
const url=process.env.PAGE_URL || 'http://127.0.0.1:8765/blog/adversarial-examples-for-hashes/';
(async()=>{
 const browser=await chromium.launch({headless:true});
 const report={widths:[],errors:[],analytics:"Google Tag Manager script replaced with an empty local response to exclude tracking; page and chart assets load normally."};
 for(const width of [375,768,1200,1620]){
  const page=await browser.newPage({viewport:{width,height:1000},deviceScaleFactor:1});
  await page.route('https://www.googletagmanager.com/**',route=>route.fulfill({status:200,contentType:'application/javascript',body:''}));
  page.on('pageerror',e=>report.errors.push({width,type:'pageerror',message:e.message}));
  page.on('console',m=>{if(m.type()==='error')report.errors.push({width,type:'console',message:m.text()})});
  page.on('response',r=>{if(r.status()>=400)report.errors.push({width,type:'http',status:r.status(),url:r.url()})});
  page.on('requestfailed',r=>report.errors.push({width,type:'requestfailed',url:r.url(),error:r.failure()}));
  await page.goto(url,{waitUntil:'networkidle'});
  await page.locator('#bits-vs-speed svg').waitFor();
  const result={width,hosts:[],profiles:[]};
  for(const host of ['m2','xeon']){
   if(host==='xeon'){
    await page.click('[data-host="xeon"]');
    await page.waitForFunction(()=>document.querySelector('#bits-vs-speed').dataset.motion==='running');result.animation=true;
    await page.waitForFunction(()=>document.querySelector('#bits-vs-speed').dataset.motion==='idle');
   }
   for(const [id,claimed] of [['umash',55],['umash128',83]]){
    await page.selectOption('#hash-inspect',id);
    assert.equal(await page.locator('.inspector-score').innerText(),`${claimed} (claimed) bits`);
    assert.equal(await page.locator('.figure-inspector').getAttribute('data-kind'),'claim');
    assert.match(await page.locator('.inspector-summary').innerText(),/53\.38/);
    await page.locator('.inspector-qualifications summary').click();
    assert.match(await page.locator('.inspector-evidence').innerText(),/Both joint cases are closed/);
    assert.match(await page.locator('.inspector-evidence').innerText(),/46\.96/);
    assert.match(await page.locator('.inspector-evidence').innerText(),/255/);
    assert.match(await page.locator('.inspector-scope').innerText(),/53\.38/);
    assert.match(await page.locator('.inspector-key').innerText(),/34 random OH words/);
    result.profiles.push({host,id,claimed,certified:53.38});
    if(host==='m2' && id==='umash')await page.locator('.figure-inspector').screenshot({path:`${out}/${width}-umash-profile.png`});
   }
   await page.locator('.figure-stage').screenshot({path:`${out}/${width}-${host}-chart.png`});
   result.hosts.push(host);
  }
  // Exercise a proved and a measured profile as well as the two claims.
  for(const [id,kind] of [['chain-v3','proof'],['highway','witness']]){
   await page.selectOption('#hash-inspect',id);
   assert.equal(await page.locator('.figure-inspector').getAttribute('data-kind'),kind);
   await page.locator('.inspector-qualifications summary').click();
   for(const field of ['evidence','key','scope'])assert((await page.locator(`.inspector-${field}`).innerText()).length>10);
   result.profiles.push({id,kind,expanded:true});
  }
  const point=page.locator('#bits-vs-speed [data-point-id="umash"]');
  await point.hover();assert(await page.locator('.figure-peek').isVisible());
  assert.match(await page.locator('.figure-peek').innerText(),/55 \(claimed\)/);result.hover=true;
  await point.focus();await page.keyboard.press('Enter');
  assert.equal(await page.locator('.inspector-name').innerText(),'UMASH-64');
  await page.keyboard.press('ArrowRight');assert.notEqual(await page.evaluate(()=>document.activeElement.dataset.pointId),'umash');
  await page.keyboard.press('Escape');assert(await page.locator('.figure-inspector').isHidden());result.keyboard=true;
  result.layout=await page.evaluate(()=>({noOverflow:document.documentElement.scrollWidth<=innerWidth,brokenToc:[...document.querySelectorAll('[data-toc-list] a')].filter(a=>!document.getElementById(a.hash.slice(1))).map(a=>a.hash),figureCss:[...document.styleSheets].some(x=>x.href && new URL(x.href).pathname.endsWith('/figure/chart.css')),figureScript:[...document.scripts].some(x=>x.src && new URL(x.src).pathname.endsWith('/figure/chart.js'))}));
  assert(result.layout.noOverflow);assert.equal(result.layout.brokenToc.length,0);assert(result.layout.figureCss&&result.layout.figureScript);
  const toc=width<1200?page.locator('.toc-mobile'):page.locator('.toc-desktop');
  assert(await toc.isVisible());if(width<1200)await toc.locator('summary').click();
  await toc.locator('a[href="#appendix-umash"]').click();
  assert.equal(new URL(page.url()).hash,'#appendix-umash');result.toc=true;
  await page.locator('#appendix-umash').scrollIntoViewIfNeeded();
  await page.screenshot({path:`${out}/${width}-appendix.png`});
  const pinned=page.locator('#proof-table [data-pinned-id="umash"]');
  assert.match(await pinned.innerText(),/37\.9999/);assert.match(await pinned.getAttribute('title'),/3125/);
  const pinned128=page.locator('#proof-table [data-pinned-id="umash128"]');
  assert.match(await pinned128.getAttribute('title'),/37\.9999 certified/);result.pinnedBounds=true;
  for(const id of ['umash','umash128'])assert.match(await page.locator(`#proof-table tr[data-hash-id="${id}"]`).innerText(),/53\.38/);
  report.widths.push(result);await page.close();
 }
 const fallback=await browser.newPage({javaScriptEnabled:false,viewport:{width:375,height:900}});
 await fallback.goto(url);const img=fallback.locator('#bits-vs-speed img');assert(await img.isVisible());assert(await img.evaluate(e=>e.complete&&e.naturalWidth>0));
 await img.screenshot({path:`${out}/375-static-fallback.png`});report.noScriptFallback=true;await fallback.close();
 const reduced=await browser.newPage({reducedMotion:'reduce'});await reduced.goto(url);await reduced.locator('#bits-vs-speed svg').waitFor();
 await reduced.click('[data-host="xeon"]');await reduced.waitForFunction(()=>document.querySelector('[data-host="xeon"]').getAttribute('aria-pressed')==='true');
 assert.equal(await reduced.locator('#bits-vs-speed').getAttribute('data-motion'),'idle');report.reducedMotion=true;await reduced.close();
 fs.writeFileSync(`${out}/checks.json`,JSON.stringify(report,null,2)+'\n');await browser.close();
 console.log(JSON.stringify(report,null,2));assert.equal(report.errors.length,0);
})().catch(e=>{console.error(e);process.exit(1)});
