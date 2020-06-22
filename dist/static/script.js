console.log(window.location.hash);
window.onload = function () {
   /* Show hide abstracts */
   for (let div of document.querySelectorAll('.toggle-item')) {
      div.onclick = function() {
          // Other options: event_category, event_label, value (numeric)
          gtag('event', 'toggle', {'event_label': div.id});
          this.classList.toggle('open');
          window.history.replaceState(null, null, '#'+div.id);
      }
      if ('#'+div.id == window.location.hash) {
         div.classList.add('open');

      }
      // TODO: If hash paper is not featured, we should enable show-all
   }

   /* Show hide featured */
   let toggle = () => {
      for (let div of document.querySelectorAll('.publications')) {
         div.classList.toggle('toggled');
      }
   }
   let checkbox = document.getElementById('show-all-checkbox');
   checkbox.addEventListener('change', event => {
      gtag('event', 'toggle-show-all', {'event_label': event.target.checked ? 'on' : 'off' });
      toggle();
   });
   // If hash, we should show all, in case the hashed paper is hidden
   if (window.location.hash)
      checkbox.checked = true;
   if (!checkbox.checked)
      toggle();

   /* Mouse over me */
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
