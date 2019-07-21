import Vue from 'vue'

//import $ from 'jquery'
import '../assets/app.css'
import 'bootstrap/dist/css/bootstrap.css'

import App from './app.vue'
new Vue({
  el: '#app',
  render: h => h(App),  
})