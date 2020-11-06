module.exports = function (config, env) {
  if (env.isProd) {
    config.devtool = false; // disable sourcemaps
  }

  config.module.rules = config.module.rules.filter(
    (r) => !(r.loader && r.loader.indexOf("preact/async-loader") > -1)
  );
};
