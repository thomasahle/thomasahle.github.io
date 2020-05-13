margin = {top: 10, right: 15, bottom: 50, left: 15};
width = 800;
class JoyPlot {
   constructor(graph) {
      this.graph = graph;
   }
   mount(svg) {
      this.svg = svg;
      svg.attr('width', width);
      this.gxAxis = svg.append("g");
      this.gyAxis = svg.append("g");
      svg.append('g').attr('id', 'rows');
      graph.dispatch.on('change', () => this.update());
   }
   update() {
      const series = [];
      this.graph.root.each(d => {
         while (series.length <= d.depth)
            series.push([]);
         while (series[d.depth].length <= d.data.value)
            series[d.depth].push(0);
         series[d.depth][d.data.value]++;
      });
      // Add extra row of zeros
      for (let row of series) {
         row.push(0);
         row.push(0);
      }
      series.shift();

      const transition = this.svg.transition()
         .duration(this.graph.duration);

      const step = 17;
      const overlap = 8;
      const max = d3.max(series, d => d3.max(d));
      const maxz = d3.max(series, (d,i) => overlap * step * d3.max(d)/max - i * step);
      // The real step formula is step = (stop - start) / Math.max(1, n - paddingInner + paddingOuter * 2)
      const len = series.length;
      const pad = 1;
      const margin_top = margin.top + len/Math.max(1,len-pad) * maxz;
      const height = series.length * step + margin_top + margin.bottom;
      this.svg.transition(transition).attr('height', height);
      const x = d3.scalePoint()
          .domain(d3.range(0, d3.max(series, d => d.length)))
          .range([margin.left, width - margin.right]);
      const y = d3.scalePoint()
          .domain(d3.range(1, series.length+1))
          .range([margin_top, height - margin.bottom]);
      const z = d3.scaleLinear()
          .domain([0, max]).nice()
          .range([0, -overlap * y.step()]);
      this.gxAxis
         .transition(transition)
         .call(g => g
           .attr("transform", `translate(0,${height - margin.bottom})`)
           .call(d3.axisBottom(x)
               .ticks(width / 80)
               .tickSizeOuter(0)));
      this.gyAxis
         .transition(transition)
         .call(g => g
             .attr("transform", `translate(${margin.left},0)`)
             .call(d3.axisLeft(y).tickSize(0).tickPadding(4))
             .call(g => g.select(".domain").remove()));
      const area = d3.area()
          .curve(d3.curveMonotoneX)
          .x((d, i) => x(i))
          .y0(0)
          .y1(d => z(d));
      const area_flat = d3.area()
          .curve(d3.curveBasis)
          .x((d, i) => x(i))
          .y0(0)
          .y1(0);
      let line = area.lineY1();
      let line_flat = area_flat.lineY1();

      const rows = this.svg.select('#rows')
         .selectAll('g')
         .data(series, (d,i) => i)
         .join(enter => {
            let g = enter.append('g')
               .attr("transform", (d,i) => `translate(0,${i > 0 ? y(i) : y(i+1)})`);
            g.append('path')
               .attr('class', 'area')
               .attr("d", d => area_flat(d));
            g.append('path')
               .attr('class', 'line')
               .attr("d", d => line_flat(d))
               .attr("stroke-opacity", "0") ;
            return g;
         });
      rows
         .transition(transition)
         .attr("transform", (d,i) => `translate(0,${y(i+1)})`);
      rows
         .select('path.area')
         .transition(transition)
         .attr("d", d => area(d));
      rows
         .select('path.line')
         .transition(transition)
         .attr("d", d => line(d))
         .attr("stroke-opacity", "1");

   }
}
