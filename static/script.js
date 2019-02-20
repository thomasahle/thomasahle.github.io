window.onload = function () {
   for (var div of document.querySelectorAll('.shown')) {
      div.onclick = function() {
          dataLayer.push({'event': 'toggle_'+div.id});
          this.__toggle = !this.__toggle;
          var target = this.parentNode.querySelector('.abstract');
          if (this.__toggle) {
              target.style.display = 'block';
          } else {
               target.style.display = 'none';
          }
      }
   }
   var img = document.getElementById('me');
   img.onmouseover = function() {
     img.setAttribute('src', 'static/water.jpg');
   }
   img.onmouseout = function() {
     img.setAttribute('src', 'static/thomas_farve.png');
   }
}
