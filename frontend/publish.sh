#!/bin/bash
yarn run build
cd ..
rm -rf data/*
cp -r frontend/build/* data/
rm -rf data/ssr-build
rm -rf data/mockServiceWorker.js
rm -rf data/preact_prerender_data.json
rm -rf data/sw-debug.js
rm -rf data/push-manifest.json
rm -rf data/manifest.json
rm -rf data/assets/icons
rm -rf data/assets/favicon.ico
