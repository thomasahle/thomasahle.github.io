margin = {top: 10, right: 10, bottom: 0, left: 10};
left_width = 300;
right_width = 400;
width = left_width+right_width;
height = 400;
sim_center = [left_width + right_width/2, height/2];

class SimulationPlot {
   mount(svg, step_button, restart_button) {
      this.svg = svg;
      this.step_button = step_button;
      this.restart_button = restart_button;
      svg.attr('width', width).attr('height', height);
      this.box = svg.append("g");
      this.sim_g = svg.append('g')
         .attr('x', left_width)
         .attr('y', margin.top)
         .attr('width', right_width)
         .attr('height', height);
      this.n = 200;
      this.w = 50;
      this.t = .802246;
      this.D = 2;
      this.init_buttons();
   }
   restart() {
      const set_colors = d3.scaleSequential(d3.interpolateBlues).domain([-3, 2]);
      const uni_colors = d3.scaleSequential(d3.interpolateGreys).domain([-1, 5]);
      this.data = Array.from({ length: this.n }, (_, i) => {
         const set_item = Math.random() < this.w/this.n;
         return {
           r: 2 * (4 + Math.random()**2)
           ,color: set_item ? set_colors(Math.random()) : uni_colors(Math.random())
           ,is_set_item: set_item
           ,id: i
         }});
      this.update_uni();

      const initial_reps = 15;
      this.clone_step = false;
      this.step_button
            .attr('style', 'visibility:show')
            .html('<i class="material-icons">skip_next</i>'
                  + (this.clone_step ? 'Multiply' : 'Mutate'));
      this.reps = [];
      for (let i = 0; i < initial_reps; i++) {
         this.reps.push({
            objs: [],
            x: sim_center[0],
            y: sim_center[1],
            ox: sim_center[0],
            oy: sim_center[1],
            r: 15,
            id: Math.random(),
            val: 0,
            alive: true
         });
      }
      this.update_sim();
   }
   init_buttons() {
      this.restart_button.on('click', () => {
         // Remove alive class, so the reps don't shake
         this.sim_g.selectAll('.alive').classed('alive', false);
         this.restart();
      });
      let rep_sim = null;
      this.step_button.on('click', () => {
         for (const rep of this.reps)
            if (rep.sim)
               rep.sim.stop();
         const new_reps = [];
         if (this.clone_step) {
            for (const rep of this.reps) {
               if (!rep.alive)
                  continue;
               for (let i = 0; i < this.D; i++) {
                  const new_rep = {
                     objs: [],
                     x: rep.x,
                     y: rep.y,
                     ox: rep.x,
                     oy: rep.y,
                     id: Math.random(),
                     r: rep.r,
                     val: rep.val,
                     alive: true
                  };
                  for (const old_obj of rep.objs) {
                     const obj = {...old_obj};
                     obj.ox = obj.x;
                     obj.oy = obj.y;
                     new_rep.objs.push(obj);
                  }
                  new_reps.push(new_rep);
               }
            }
         }
         else {
            for (const rep of this.reps) {
               if (!rep.alive)
                  continue;
               const par = this.data[Math.floor(Math.random()*this.n)];
               rep.objs.push({
                  r: par.r,
                  color: par.color,
                  x: par.x - rep.x,
                  y: par.y - rep.y,
                  ox: par.x - rep.x,
                  oy: par.y - rep.y,
                  id: Math.random(),
               });
               rep.ox = rep.x;
               rep.oy = rep.y;
               rep.r = 10 + 10 * Math.sqrt(rep.objs.length);
               rep.val = rep.val + (par.is_set_item ? 1 : 0);
               rep.alive = Math.abs(rep.val - this.t*rep.objs.length)
                              <= Math.sqrt(this.t*(1-this.t)*rep.objs.length);
               new_reps.push(rep);
            }
         }
         this.reps = new_reps;
         this.clone_step = !this.clone_step;
         if (!this.reps.some(d => d.alive))
            this.step_button.attr('style', 'visibility:hidden');
         else
            this.step_button.html(
                  '<i class="material-icons">skip_next</i>'
                  + (this.clone_step ? 'Multiply' : 'Mutate'));
         this.update_sim();
      });
   }
   update_sim() {
      const rep_nodes = this.sim_g
         .selectAll('g')
         .data(this.reps, d => d.id)
         .join(
            enter => enter
               .append('g')
               .attr('class', 'rep')
               .attr('transform', d => `translate(${d.x}, ${d.y})`)
               .call(gs => gs
                  .append('circle')
                  .attr('class', 'rep_circle')
                  .attr('r', 0)
                  .attr('opacity', 0))
            ,
            update => update,
            exit => {
               // Mark nodes for dying, so they are no longer force animated
               exit.classed('dying', true);
               const alives = exit.filter('.alive');
               const not_alives = exit.filter(':not(.alive)');

               // Remove dead individuals
               not_alives
                  .selectAll('circle')
                  .attr('opacity', 1)
                  .transition()
                  .duration(1000)
                  .attr('r', 0)
                  .attr('opacity', 0)
                  .attr('cx', 0)
                  .attr('cy', 0);
               not_alives
                  .transition().delay(1000)
                  .remove();

               // Remove "alive" individuals that are branching
               const shake = 15;
               let shake_transition = alives;
               for (let i = 0; i < 5; i++)
                  shake_transition = shake_transition
                     .transition().duration(100)
                     .attr('transform', d => `translate(
                        ${d.x + shake*(Math.random()-.5)},
                        ${d.y + shake*(Math.random()-.5)})`);
               shake_transition
                  .transition().delay(500)
                  .remove();

               exit
                  .filter('.alive')
                  .selectAll('circle')
                  .attr('opacity', 1)
                  .transition().delay(400).duration(600)
                  .attr('opacity', 0)
                  .attr('r', 0)
                  .attr('cx', 0)
                  .attr('cy', 0);
            }
         )
         .classed('alive', d => d.alive)
      ;

      // Update circle size
      rep_nodes.selectAll('.rep_circle')
         .transition()
         .duration(1000)
         .attr('r', d => d.r)
         .attr('opacity', 1)
      ;

      const circles = rep_nodes
         .selectAll('circle.obj')
         .data(d => d.objs, d => d.id)
         .join(
            enter => {
               const circles = enter
                  .append('circle')
                  .attr('class', 'obj')
                  .attr('fill', d => d.color)
                  //.attr('r', d => d.r)
               // If the objects arrived by cloning, we are now no longer
               // in the clone step.
               if (!this.clone_step)
                  circles
                     .attr('r', 0)
                     .attr('opacity', 0)
                     .transition()
                     .duration(1000)
                     .attr('r', d => d.r)
                     .attr('opacity', 1)
               else
                  circles
                     .attr('stroke', '#183752')
                     .attr('stroke-width', '1px')
                     .attr('r', d => 1.5*d.r)
                     .transition()
                     .duration(1000)
                     .attr('stroke', 'transparent')
                     .attr('r', d => d.r)
               return circles;
            }
         )

      rep_nodes
         .each(outer_d => {
               outer_d.sim = d3.forceSimulation(outer_d.objs)
                .on("tick", function() {
                   let a = this.alpha();
                   a = Math.exp(-(Math.log(a)**4));
                   circles
                      .attr('cx', d => d.x * (1-a) + d.ox * a)
                      .attr('cy', d => d.y * (1-a) + d.oy * a);
                })
                .force("collide", d3.forceCollide().radius(d => 1 + d.r))
                .force("x", d3.forceX(0).strength(1e-1))
                .force("y", d3.forceY(0).strength(1e-1))
            }
          )

      if (this.rep_sim) this.rep_sim.stop();
      let sim_g = this.sim_g;
      this.rep_sim = d3.forceSimulation(this.reps)
          .on("tick", function() {
             let a = this.alpha();
             if (!this.clone_step)
                a = Math.exp(-(Math.log(a)**8)/8);
             // We filter out dying reps, since they are busy shaking
             sim_g.selectAll('g:not(.dying)')
                .attr('transform', function(d) {
                let x = d.x * (1-a) + d.ox * a;
                let y = d.y * (1-a) + d.oy * a;
                return `translate(${x}, ${y})`;
             });
          })
          .force("collide", d3.forceCollide().radius(d => 5 + d.r))
          .force("x", d3.forceX(sim_center[0]).strength(5e-2))
          .force("y", d3.forceY(sim_center[1]).strength(5e-2));

      if (this.reps.length == 0)
         this.restart();
   }
   update_uni() {
      const nodes = this.box.selectAll('circle')
         .data(this.data).join('circle')
         .attr('r', d => d.r)
         .attr('fill', d => d.color);

      function tick(nodes) {
          nodes.attr("cx", d => d.x).attr("cy", d => d.y);
        }
      this.rescale = isNaN(this.data[0].x);
      const box_center = [margin.left/2+left_width/2, (margin.top-margin.bottom+height)/2];
      const simulation = d3
          .forceSimulation(this.data)
          .on("tick", () => tick(nodes))
          .force("collide", d3.forceCollide().radius(d => 2 + d.r))
          .force("x", d3.forceX(box_center[0]).strength(0.007))
          .force("y", d3.forceY(box_center[1]).strength(0.001))
          .stop();

       // once the arrangement is initialized, scale and translate it
       const scale = 1;
       if (this.rescale) {
         for (const node of this.data) {
           node.x = node.x * scale + box_center[0];
           node.y = node.y * scale + box_center[1];
         }
       }

      // differ application of the forces
     simulation.restart();
   }
   update() {
      this.restart();
   }
}
