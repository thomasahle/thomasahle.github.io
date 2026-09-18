# Verification modules

Built with Emscripten 4.0.21, `-O2 -std=c11`, from the unchanged programs in
[`../verify/`](../verify/). Each `.js` file is Emscripten glue for the corresponding
`.wasm`. The manifest records sizes and SHA-256 hashes of original C sources.

The added adapters and their generator are under [`source/`](source/). To reproduce
the build layout, copy `source/wrappers/`, `source/build_demos.py`, and
`source/build/selected-records.json` into a working directory, and copy `../verify/`
there as `verify/`. Also supply the `page/`, `post-demo.js`, and `demo.css` files
from the handoff, or compile each wrapper directly with the flags in the generator.
No original C implementation is edited.

Licenses and copyright notices remain in each original C program and
[`../verify/README.md`](../verify/README.md). In particular, pengyhash is
GPLv3-or-later. Keep matching sources, build instructions and notices available
when distributing these binaries.

Full validation results, API documentation, and limitations are in `DEMOS.md` in
the handoff directory. rapidhash v1 uses its original reference vector because
the supplied package has no SMHasher3 registration for v1.
