<template>
  <div id="app">
    <Dashboard />
  </div>
</template>

<script>
import Dashboard from './components/Dashboard.vue'

export default {
  name: 'App',
  components: {
    Dashboard
  },
  data() {
    return {
      connection: null
    }
  }
}
</script>

<style>
#app {
  /* text-align: center; */
}
</style>
