<template>
    <div class="container">
        <div class="row">
            <div class="col-6">
                <SwitchState name="Kugellampen" buttonid="0"/>
            </div>
            <div class="col-6">
                <SwitchState name="Ringlichter" buttonid="1"/>
            </div>
        </div>
        <div class="row">
            <div class="col-6">
                <SwitchState name="Steckdose Gartenhaus" buttonid="2"/>
            </div>
            <div class="col-6">
                <SwitchState name="Steckdose Wohnzimmer" buttonid="3"/>
            </div>
        </div>
        <div class="row">
            <div class="col-12">
                <button type="button" class="btn btn-warning" style="width:100%" v-on:click="startOTA">Enable OTA-Mode</button>

                <div class="alert alert-danger" role="alert" v-if="hasAlert">
                    {{ info }}
                </div>
            </div>
        </div>
    </div>  

</template>

<script>
    import SwitchState from './components/SwitchStateComponent.vue'
    import axios from 'axios'

    export default {
        components: {
            SwitchState
        }, 
        data: function(){ return {
            message: "Test",
            info : '',
            hasAlert: false,
            alertTimer:null,            
        }},
        methods: {
            clearAlert : function(){
                if (this.alertTimer!=null){
                    clearTimeout(this.alertTimer);
                    this.alertTimer = null;
                }
                this.hasAlert = false;
                this.info = '';
            },
            pushAlert : function(message){
                this.hasAlert = true;
                this.info = message;
                this.alertTimer = setTimeout(() => this.hasAlert = false, 5000);
                console.log(message)
            },
            startOTA: function () {                
                axios
                    .get('/ota')
                    .then(response => {
                        //console.log(response)
                        this.info = response.data
                    })
                    .catch(error => {                        
                        this.pushAlert(error)
                    })
            }
        }
    }
</script>