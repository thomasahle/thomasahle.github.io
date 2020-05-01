// Can get some visual ideas from here
// https://observablehq.com/@mbostock/tree-of-life
//
function link(curve) {
  var radius = null,
     angle = null;
  function link(d) {
     context = d3.path();
     curve(context, radius(d.source), angle(d.source), radius(d.target), angle(d.target));
     return context;
  }
  link.radius = function(f) {
     radius = f;
     return link;
  };
  link.angle = function(f) {
     angle = f;
     return link;
  };
  return link;
}

function curveArc(context, r0, a0, r1, a1) {
  var p0 = d3.pointRadial(a0, r0),
      p1 = d3.pointRadial(a0, (r0 + r1) / 2),
      p2 = d3.pointRadial(a1, (r0 + r1) / 2),
      p3 = d3.pointRadial(a1, r1);
   /*if (r0 == r1 && a0 == a1) {
      console.log([r0, a0, r1, a1]);
      return;
   }*/
   //console.log([a0, a1, (a1-a0), (a1-a0+2*Math.PI)%(2*Math.PI)]);
  context.moveTo(p0[0], p0[1]);
  context.lineTo(p1[0], p1[1]);
   //if (Math.abs(a0 - a1) > 1e-3)
      // We can make a simple a test, since the tree can never wrap all the way around
     context.arc(0, 0, (r0+r1)/2, a0-Math.PI/2, a1-Math.PI/2, a1<a0);
  context.moveTo(p2[0], p2[1]);
  context.lineTo(p3[0], p3[1]);
}

function linkRadial2() {
   return link(curveArc);
}


class RadialTree {
   constructor() {
      this.radius = 300;
      this.tree = d3.tree().size([2*Math.PI, this.radius]);
      this.duration = 1000;
   }
   reset(init_degree, p, t, d) {
      this.p = p;
      this.t = t;
      this.d = d;
      this.root = d3.hierarchy({
         name: '',
         value: 0,
         id: 0,
         alive: true
      });
      this.id_counter = 1;
      this.root.x0 = 0;
      this.root.y0 = 0;
      for (let i = 0; i < init_degree; i++) {
         this.insert(this.root, {name: '', value: 0, alive: true});
      }
      this.total_size = 1;
   }
   insert(par, data) {
      data.id = this.id_counter++;
      const newNode = d3.hierarchy(data);
      newNode.depth = par.depth + 1;
      newNode.parent = par;
      // Walk up the tree, updating the heights of ancestors as needed.
      // Could perhaps be done also with newNode.decendants().each.
      for(let height = 1, anc = par; anc != null; height++, anc=anc.parent) {
         anc.height = Math.max(anc.height, height);
      }
      if (!par.children)
         par.children = [];
      par.children.push(newNode);
   }
   step() {
      const level = this.root.height;
      const level_size = Math.ceil(Math.pow(this.d, level)/this.total_size);
      this.total_size *= level_size;
      this.root.each(node => {
         if (node.depth == level && node.data.alive) {
            for (let i = 0; i < level_size; i++) {
               const val = node.data.value + (Math.random() < this.p ? 1 : 0);
               const sig = Math.pow(this.t*(1-this.t)*level, 1/3);
               this.insert(node, {
                  name: val+'/'+level,
                  value: val,
                  alive: this.t >= this.p ? val >= this.t*level-sig : val <= this.t*level+sig,
                  id: this.id_counter++
               });
            }
         }
      });
   }
   isAlive() {
      const level = this.root.height;
      let res = false;
      this.root.each(node => {
         if (node.depth == level && node.data.alive)
            res = true;
      });
      return res;
   }
   mount(svg) {
      this.svg = svg;
      this.gLink = svg.append("g")
          .attr("fill", "none")
          .attr("stroke", "#555")
          .attr("stroke-opacity", 0.4)
          .attr("stroke-width", 1.5);
      this.gNode = svg.append("g")
          .attr("cursor", "pointer")
          .attr("pointer-events", "all");
   }
   update() {
      this.tree(this.root);
      const links = this.root.links();

      // Compute the new tree layout.
      //const size = size_radial();
      const size = {width: this.radius*2, height: this.radius*2};

      const transition = this.svg.transition()
         .duration(this.duration)
      // Animate resize of svg element to fit new data
         //.attr("viewBox", [-margin.left, - margin.top, size.width, size.height])
         .attr("viewBox", [-size.width/2, -size.height/2, size.width, size.height])
         .attr('width', size.width)
         .attr('height', size.height)
      // Some sort of hack to make the window realize it needs to resize?
         .tween("resize", window.ResizeObserver ? null : () => () => this.svg.dispatch("toggle"));


      // Update the links. They take the id of their target.
      const link = this.gLink.selectAll("path")
         .data(links, d => d.target.data.id);

      // Helper function for beizer paths
      // const diagonal = d3.linkVertical().x(d => d.x).y(d => d.y);
      const diagonal = linkRadial2().angle(d => d.x).radius(d => d.y);

      // Enter any new links at the parent's previous position.
      const linkEnter = link.enter().append("path");

      // Transition links to their new position.
      // Note: each link is an object that defines source and target properties
      link.merge(linkEnter).transition(transition)
         // We need to storre the old positions before they get updated at the end
         // of the function. It turns out attrTween is too late.
         .attr('d', li => {
            li.source.x00 = li.source.x0;
            li.source.y00 = li.source.y0;
            // If there is no x0 (the newly entered items)i
            // we make it pop out of the parent.
            li.target.x00 = li.target.x0 || li.source.x0;
            li.target.y00 = li.target.y0 || li.source.y0;
         })
         .attrTween("d", d => t => diagonal(
               {source: {x: (1-t)*d.source.x00 + t*d.source.x,
                         y: (1-t)*d.source.y00 + t*d.source.y},
                target: {x: (1-t)*d.target.x00 + t*d.target.x,
                         y: (1-t)*d.target.y00 + t*d.target.y}})
         );
         //.attr("d", diagonal);

      // We just use those for reset.
      // But actually, the new structure doesn't require resets...
      link.exit().remove();

      // Stash the old positions for transition.
      this.root.each(d => {
         d.x0 = d.x;
         d.y0 = d.y;
      });
   }
}

