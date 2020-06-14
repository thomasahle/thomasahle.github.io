console.log(window.location.hash);
window.onload = function () {
   for (let div of document.querySelectorAll('.toggle-item')) {
      div.onclick = function() {
          // Other options: event_category, event_label, value (numeric)
          gtag('event', 'toggle', {'event_label': div.id});

          this.classList.toggle('open');

          window.history.replaceState(null, null, '#'+div.id);
          // This jumps too much
          //window.location.hash = '#'+div.id;
      }
      if ('#'+div.id == window.location.hash)
         div.classList.add('open');
   }
   let img = document.getElementById('me');
   img.onmouseover = function() {
     img.setAttribute('src', '/static/water.jpg');
     gtag('event', 'hover_me', {'event_label': 'on'});
   }
   img.onmouseout = function() {
     img.setAttribute('src', '/static/potrait.jpg');
     gtag('event', 'hover_me', {'event_label': 'off'});
   }
}
