module.exports = {
  configureWebpack: {
    optimization: {
      splitChunks: false
    },
    output: {
      filename: 'app.js',
      chunkFilename: 'app.js',
    }    
  },
  publicPath: ''
}