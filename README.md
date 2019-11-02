This is a benchmark between [cairo](https://www.cairographics.org/), [skia](https://skia.org/) and [vkvg](https://github.com/jpbruyere/vkvg) on the model of [caskbench](https://gitlab.com/bryceharrington/caskbench.git), but in c.
To be able to test the vulkan backend of skia, I had to patch the experimental c API of skia, the patched branch is set as submodule.

To build skia:
```
git submodule update --init --recursive
cd external/skia
python2 tools/git-sync-deps
bin/gn gen out/shared --args='is_official_build=true is_component_build=true skia_use_vulkan=true'
ninja -C out/shared
```

TODO:
The global test architecture is ok for the 3 libraries, now we could add more tests.
Maybe it would be nice to have cairo and vkvg also as submodule and tag the results with the respective git commit hash.
