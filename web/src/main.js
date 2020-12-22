import Vue from 'vue'
import App from './App.vue'

import Vuex from 'vuex'
import Router from 'vue-router';
import { MdButton, MdContent, MdTabs, MdCard, MdField, MdCheckbox, MdSwitch, MdSnackbar, MdApp } from 'vue-material/dist/components'
import 'vue-material/dist/vue-material.min.css'
import 'vue-material/dist/theme/default-dark.css'
// import 'vue-material/dist/theme/default.css'

Vue.use(MdButton)
Vue.use(MdContent)
Vue.use(MdTabs)
Vue.use(MdCard)
Vue.use(MdField)
Vue.use(MdCheckbox)
Vue.use(MdSwitch)
/** @todo Implement the router */
Vue.use(Router)
/** @todo Implement Vuex */
Vue.use(Vuex)
Vue.use(MdSnackbar)
Vue.use(MdApp)

Vue.config.productionTip = false

new Vue({
  render: h => h(App),
}).$mount('#app')

/** @todo Implement router */
export default new Router({
  mode: 'history',
  // routes: [
  //   {
  //     path: '/',
  //     component: Form1,
  //   },
  //   {
  //     path: '/form2',
  //     component: Form2,
  //   },
  // ]
});