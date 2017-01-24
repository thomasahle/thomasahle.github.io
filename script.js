window.onload = function () {
   for (var div of document.querySelectorAll('.shown')) {
      div.onclick = function() {
          this.__toggle = !this.__toggle;
          var target = this.parentNode.querySelector('.abstract');
          if (this.__toggle) {
              target.style.display = 'block';
          } else {
               target.style.display = 'none';
          }
      }
   }
}
