# growweb

## Project setup
```
yarn install
```

### Compiles and hot-reloads for development
```
yarn serve
```

### Compiles and minifies for production
```
yarn build
```

### Lints and fixes files
```
yarn lint
```

### Customize configuration
See [Configuration Reference](https://cli.vuejs.org/config/).

### gzip as required and move
Execute the './scripts/cpydist' bash file. This will gzip all of the 'dist' files
and move them into the 'data' folder of the ESP32 project.
Any existing files in the destination folder will be overwritten.

ctrl+h = cpydist (bashfile). Copy and gzip dist files to ESP32
SPIFF data folder.

### 'public' / 'dist' folder
a.bin = app.js
b.bin = favicon.ico
