/*
https://plotly.com/javascript/
*/

const BROKER_URL = 'broker.hivemq.com';
const BROKER_PORT = 8000;
const BROKER_CLIENT_ID = 'cliente_cperren';
const BROKER_PATH = '/mqtt';
const BROKER_TOPIC_SUBSCRIBE = 'cperren/central_meteo'
const BROKER_TOPIC_PUBLISH = 'cperren/oficina_ideas/iluminacion'

const header_mqtt = document.getElementById('header_mqtt');
const btn_iluminacion = document.getElementById('btn_iluminacion');
const subscribe_field = document.getElementById('subscribe_topic_field');
const publish_field = document.getElementById('publish_topic_field');

const client = new Paho.MQTT.Client(BROKER_URL, BROKER_PORT, BROKER_PATH, BROKER_CLIENT_ID);

let readingsCounter = 0;
let data = { x: [], y: [] }
let broker_conectado = false;
let iluminacion_activa = false;

header_mqtt.addEventListener('click', (event) => {
    event.preventDefault();

    if (subscribe_field.value === '' || publish_field.value === '') {
        alert('Completar campos de suscripción y publicación');
    } else {
        header_mqtt.innerHTML = `<h1>Conectando con servidor...</h1>`;
        mqtt_broker_connect();
    }
});

btn_iluminacion.addEventListener('click', (event) => {
    event.preventDefault();
    if (broker_conectado) {
        iluminacion_activa ? client.send(BROKER_TOPIC_PUBLISH, "0") : client.send(BROKER_TOPIC_PUBLISH, "1");
    }
});

client.onConnectionLost = (responseObject) => {
    if (responseObject.errorCode !== 0) {
        console.error('Desconectado!:', responseObject.errorMessage);
        publish_field.disabled = false;
        subscribe_field.disabled = false;
        mqtt_broker_connect();
    }
};

client.onMessageArrived = (message) => {
    // message.destinationName
    const jsonData = JSON.parse(message.payloadString);

    if (jsonData.iluminacion == 1 && !iluminacion_activa) {
        iluminacion_activa = true;
        btn_iluminacion.innerHTML = 'Desactivar iluminacion';
        btn_iluminacion.classList.add('btn-danger');
        btn_iluminacion.classList.remove('btn-success');
    } else if (jsonData.iluminacion == 0 && iluminacion_activa) {
        iluminacion_activa = false;
        btn_iluminacion.innerHTML = 'Activar iluminacion';
        btn_iluminacion.classList.add('btn-success');
        btn_iluminacion.classList.remove('btn-danger');
    }

    data.x.push(++readingsCounter);
    data.y.push(jsonData.temperatura);
    Plotly.newPlot('chart', [data]);
};

const mqtt_broker_connect = () => {
    try {
        client.connect({
            onSuccess: () => {
                broker_conectado = true;
                header_mqtt.classList.add('alert-success');
                header_mqtt.classList.remove('alert-danger');
                header_mqtt.innerHTML = `<h1>Cliente conectado con servidor ${BROKER_URL}</h1>`;
                btn_iluminacion.innerHTML = 'Activar iluminacion';
                btn_iluminacion.classList.add('btn-success');
                btn_iluminacion.classList.remove('btn-danger');
                publish_field.disabled = true;
                subscribe_field.disabled = true;
                
                client.subscribe(subscribe_field.value);
            },
            onFailure: (error) => {
                broker_conectado = false;
                // console.error('MQTT error:', error.errorMessage);
                header_mqtt.classList.add('alert-danger');
                header_mqtt.classList.remove('alert-success');
                header_mqtt.innerHTML = `<h1>Cliente desconectado (${error.errorMessage})</h1>`;
                publish_field.disabled = false;
                subscribe_field.disabled = false;
                mqtt_broker_connect();
            },
            useSSL: false
        });
    } catch(err) {
        broker_conectado = false;
        header_mqtt.classList.add('alert-danger');
        header_mqtt.classList.remove('alert-success');
        header_mqtt.innerHTML = err.message;
        publish_field.disabled = false;
        subscribe_field.disabled = false;
        console.error(err);
    }
}