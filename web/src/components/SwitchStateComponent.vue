<template>
  <div class="card">
    <div class="card-body">
        <h5 class="card-title">{{ name }}</h5>
        <toggle-button :value="false"
               color="#82C7EB"
               :sync="true"
               :labels="true"
               v-model="enabled"
               @change="onChangeEventHandler"/>
        
        <div class="alert alert-danger" role="alert" v-if="hasAlert">
            {{ alert }}
        </div>
         <div class="alert alert-info" role="alert" v-if="hasInfo">
            {{ info }}
        </div>
    </div>
  </div>
</template>
<style bound>
  div.card {
    margin-top:15px;
    margin-bottom:15px
  }
</style>
<script>
    import { ToggleButton } from 'vue-js-toggle-button'   
    import axios from 'axios'

    export default {
      components: {
        ToggleButton
      },
      props: {
          name: {
            type: String,
            required: true
          },
          buttonid: {
            type: String,
            required: true
          }
      },
      data () {
        return {
            enabled: false,
            lastResponse: {},
            alert:'',
            info: '',
            alertTimer: null,
            infoTimer: null,
            hasAlert: false,
            hasInfo: false,
            interval: null
        }
      },
      created() {
        this.updateState() // first run
        this.interval = setInterval(this.updateState, 5000)
      },
      beforeDestroy() {
        if (this.interval) {
          clearIntervall(this.interval)
          this.interval = undefined
        }
      },
      methods: {
        clearAlert : function(){
            if (this.alertTimer!=null){
                clearTimeout(this.alertTimer);
                this.alertTimer = null;
            }
            this.hasAlert = false;
            this.alert = '';
        },
        pushAlert : function(message){
            this.hasAlert = true;
            this.alert = message;
            this.alertTimer = setTimeout(() => this.hasAlert = false, 5000);
            console.log(message)
        },

        clearInfo : function(){
            if (this.infoTimer!=null){
                clearTimeout(this.infoTimer);
                this.infoTimer = null;
            }
            this.hasInfo = false;
            this.info = '';
        },
        pushInfo : function(message){
            this.hasInfo = true;
            this.info = message;
            this.infoTimer = setTimeout(() => this.hasInfo = false, 5000);
            console.log(message)
        },
        onChangeEventHandler: function (theSwitch) {
          let uri = '/'+(theSwitch.value?'on':'off')+'/'+this.buttonid;
          axios.get(uri)
            .then(response => {
                //console.log(response)
                this.lastResponse = response.data
                this.enabled = this.lastResponse[0].on
                this.info = "Did Process Request"
                //this.hasInfo = true;
            })
            .catch(error => {                        
                this.pushAlert(error)
            })
        },
        updateState: function(){
          let uri = '/state/'+this.buttonid;
          axios.get(uri)
            .then(response => {
                //console.log(response)
                this.lastResponse = response.data
                this.enabled = this.lastResponse[0].on
            })
            .catch(error => {                        
                console.error(error);
            })
        }
      }
    }
</script>

