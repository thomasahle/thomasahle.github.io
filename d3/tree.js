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
  var p0 = d3.pointRadial(a0+Math.PI/2, r0),
      p1 = d3.pointRadial(a0+Math.PI/2, (r0 + r1) / 2),
      p2 = d3.pointRadial(a1+Math.PI/2, (r0 + r1) / 2),
      p3 = d3.pointRadial(a1+Math.PI/2, r1);

  context.moveTo(p0[0], p0[1]);
  context.lineTo(p1[0], p1[1]);
  context.arc(0, 0, (r0+r1)/2, a0, a1, a1<a0);
  context.lineTo(p3[0], p3[1]);

  // context.moveTo(p0[0], p0[1]);
  // context.arc(0, 0, r0, a0-Math.PI/2, a1-Math.PI/2, a1<a0);
  // context.lineTo(p3[0], p3[1]);
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
      const r = this.radius
      const p = 5;
      this.svg = svg
         .attr('width', 2 * r)
         .attr('height', 2 * r)
         .attr('viewBox', [-r-p, -r-p, 2*r+2*p, 2*r+2*p])
      this.g = svg.append("g")
   }
   update() {
      this.tree(this.root);
      const links = this.root.links();
      const nodes = this.root.descendants();

      // Compute the new tree layout.
      //const size = size_radial();
      const transition = this.svg.transition()
         .duration(this.duration)
      // Animate resize of svg element to fit new data
         //.attr("viewBox", [-margin.left, - margin.top, size.width, size.height])
      // Some sort of hack to make the window realize it needs to resize?
         .tween("resize", window.ResizeObserver ? null : () => () => this.svg.dispatch("toggle"));

      // Update the links. They take the id of their target.
      const link = this.g.selectAll("path")
         .data(links, d => d.target.data.id);

      // Helper function for beizer paths
      // const diagonal = d3.linkVertical().x(d => d.x).y(d => d.y);
      const diagonal = linkRadial2().angle(d => d.x).radius(d => d.y);

      // Enter any new links at the parent's previous position.
      const linkEnter = link.enter().append("path");

      // Transition links to their new position.
      // Note: each link is an object that defines source and target properties
      link.merge(linkEnter)
         // We hide root links, since they don't really exist
         .attr('visibility', d => d.source.depth == 0 ? 'hidden' : 'visible')
         .transition(transition)
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

      // We do the nodes after the links to have them on top
      const nodeUpdate = this.g.selectAll('g.node')
         .data(nodes, d => d.data.id);

      const nodeEnter = nodeUpdate.enter().append("g").classed('node', true)
         .attr('transform', d => `translate(
               ${d.parent ? d.parent.y0 * Math.cos(d.parent.x0) : 0},
               ${d.parent ? d.parent.y0 * Math.sin(d.parent.x0) : 0})`);

      // Make shadows
      nodeEnter.append('circle')
         .clone()
         .classed('shadow', true)
         .lower();

      nodeUpdate.merge(nodeEnter)
         .classed("inner", d => d.height > 0)
         .classed("alive", d => d.data.alive)
         .transition(transition)
         .attr('transform', d => `translate(
               ${d.y * Math.cos(d.x)},
               ${d.y * Math.sin(d.x)})`);

      // We just use those for reset.
      // But actually, the new structure doesn't require resets...
      link.exit().remove();
      nodeUpdate.exit().remove();

      // Stash the old positions for transition.
      this.root.each(d => {
         d.x0 = d.x;
         d.y0 = d.y;
      });
   }
}

