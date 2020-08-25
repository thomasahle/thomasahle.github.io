function addLoadEvent(func) {
  var oldonload = window.onload;
  if (typeof window.onload != 'function') {
    window.onload = func;
  } else {
    window.onload = function() {
      if (oldonload) {
        oldonload();
      }
      func();
    }
  }
}
// External links
addLoadEvent(function() {
   for (let a of document.querySelectorAll('a[href^="http"]')) {
      // :not([href*="//' + location.host + '"])')
      a.addEventListener('click', event => {
         gtag('event', 'click', {
            'event_category': 'outbound',
            'event_label': a.href,
            'transport_type': 'beacon'
            /*'hitCallback': function() { document.location = a.href; }*/
         });
         //return false;
      });
   }
});
