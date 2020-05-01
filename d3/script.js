// Can get some visual ideas from here
// https://observablehq.com/@mbostock/tree-of-life
//

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
      const nodes = this.root.descendants();
      const links = this.root.links();
      const node = this.gNode.selectAll('g').data(nodes, d => d.data.id);

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

      const nodeEnter = this.svg.enter().append('g')
         .attr("transform", d => {
            if (d.parent)
               return `translate(${d.parent.x0},${d.parent.y0})`;
            return `translate(0,0)`;
         })
         .attr("fill-opacity", 0)
         .attr("stroke-opacity", 0);

      // Transition nodes to their new position.
      // Can I just do nodeEnter.merge(node)? Er det grimt?
      node.merge(nodeEnter)
      // Inherit timing from main transition
         .transition(transition)
         .attr("transform", d => `
              rotate(${d.x * 180 / Math.PI - 90})
              translate(${d.y},0)
            `)
         //.attr("transform", d => `translate(${d.x},${d.y})`)
         .attr("fill-opacity", 1)
         .attr("stroke-opacity", 1);


      // Update the links…
      const link = this.gLink.selectAll("path")
         .data(links, d => d.target.data.id);

      // Helper function for beizer paths
      // const diagonal = d3.linkVertical().x(d => d.x).y(d => d.y);
      const diagonal = d3.linkRadial().angle(d => d.x).radius(d => d.y);

      // Enter any new links at the parent's previous position.
      const linkEnter = link.enter().append("path")
         .attr("d", d => {
            // Start with a path that just goes from a point to itself.
            // This is a dummy link object, since it has a source and a target.
            const o = {x: d.source.x0, y: d.source.y0};
            return diagonal({source: o, target: o});
         });

      // Transition links to their new position.
      // Note: each link is an object that defines source and target properties
      link.merge(linkEnter).transition(transition)
         .attr("d", diagonal);

      // We just use those for reset.
      // But actually, the new structure doesn't require resets...
      node.exit().remove();
      link.exit().remove();

      // Stash the old positions for transition.
      this.root.each(d => {
         d.x0 = d.x;
         d.y0 = d.y;
      });
   }
}

