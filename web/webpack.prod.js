const   merge = require('webpack-merge')
      , path = require('path')
      , webpack = require("webpack")
      , common = require('./webpack.common.js')
      , shell = require('shelljs')
      , UglifyJsPlugin = require('uglifyjs-webpack-plugin')
      , glob = require('glob')
      , MiniCssExtractPlugin = require('mini-css-extract-plugin')
      , PurgecssPlugin = require('purgecss-webpack-plugin')

const PATHS = {
  src: path.join(__dirname, 'src'),
  assets: path.join(__dirname, 'assets'),
}

class CreateHeaderPlugin {
  constructor(name, stage = 'done') {
     this.name = name;
     this.stage = stage;
  }

  copy(varName, src, dst){
    // shell.echo('static const char '+varName+'[] PROGMEM = R"=====(').to('../include/' + src + '.h')
    // shell.cat('./dist/'+src).toEnd('../include/' + src + '.h')
    // shell.echo(')=====";').toEnd('../include/' + src + '.h')
    shell.cp('./dist/'+src, '../data/'+src)
  }
  
  apply(compiler) {
    compiler.hooks[this.stage].tap(this.name, () => {
        this.copy('INDEX_HTML', 'index.html');
        this.copy('MAIN_JS', 'main.js');        
        this.copy('MAIN_CSS', 'main.css');      
        this.copy('STYLES_CSS', 'styles.css');        
        this.copy('MAIN1_JS', '1.main.js');        
    });
  }
}

module.exports = merge(common, {
  mode: 'production',
  optimization: {
    splitChunks: {
      cacheGroups: {
        styles: {
          name: 'styles',
          test: /\.css$/,
          chunks: 'all',
          enforce: true
        }
      }
    },
    usedExports: true,
    minimizer: [
      new UglifyJsPlugin({
        uglifyOptions: {
          sourceMap: true,
          compress: {
            warnings: false
          },
          output: {
            comments: false
          }
        }
      })
    ]
  },
  plugins:[    
    new PurgecssPlugin({
      paths: glob.sync(`${PATHS.src}/**/*`,  { nodir: true }),
    }),
    new CreateHeaderPlugin("CreateHeaders")
  ]
});