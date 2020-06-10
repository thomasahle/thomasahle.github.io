window.onload = function () {
   for (let div of document.querySelectorAll('.toggle-item')) {
      div.onclick = function() {
          gtag('event', 'toggle', {'event_label': div.id});
         // Other options: event_category, event_label, value (numeric)
          this.querySelector('.abstract').classList.toggle('open');
      }
   }
   let img = document.getElementById('me');
   img.onmouseover = function() {
     img.setAttribute('src', '/static/water.jpg');
     gtag('event', 'hover_me', {'value': true});
   }
   img.onmouseout = function() {
     img.setAttribute('src', '/static/potrait.jpg');
     gtag('event', 'hover_me', {'value': false});
   }
}
