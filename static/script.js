window.onload = function () {
   for (let div of document.querySelectorAll('.toggle-item')) {
      div.onclick = function() {
          dataLayer.push({'event': 'toggle_'+div.id});
          this
            .querySelector('.abstract')
            .classList.toggle('open');
      }
   }
   let img = document.getElementById('me');
   img.onmouseover = function() {
     img.setAttribute('src', 'static/water.jpg');
     dataLayer.push({'event': 'hover_me'});
   }
   img.onmouseout = function() {
     img.setAttribute('src', 'static/potrait.jpg');
   }
}
