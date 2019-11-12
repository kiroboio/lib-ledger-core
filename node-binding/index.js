const logic = require('./create_sync_balance.js')
const binding = require('bindings')('ledger-core')
const services = require('./messages/services_pb.js')
const https = require('https');
const URL = require('url').URL;
const ubinder = require('../core/lib/ubinder/src/node/ubinder');
const axios = require('axios');


function OnRequest(data, callback) {
    var serviceReq = services.ServiceRequest.deserializeBinary(data);
    if (serviceReq.getType() == services.ServiceRequestType.HTTP_REQ) {
        var httpReq = services.HttpRequest.deserializeBinary(serviceReq.getRequestBody());
        const method = httpReq.getMethod();
        const headersMap = httpReq.getHeadersMap();
        let dataStr = httpReq.getBody();
        const headers = {};
        headersMap.forEach((v, k) => {
            headers[k] = v;
        });
        let res;
        const param = {
            method,
            headers
        };
        param.url = httpReq.getUrl();
        if (dataStr != "") {
            param.data = dataStr;
        }
        console.log(param);
        axios(param)
          .then((resp) => {
              var serviceResp = new services.ServiceResponse();
              var respMessage = new services.HttpResponse();
              respMessage.setCode(resp.status);
              respMessage.setBody(JSON.stringify(resp.data));
              serviceResp.setResponseBody(respMessage.serializeBinary());
              callback(serviceResp.serializeBinary());
              })
          .catch((err) => {
              console.log(err);
              var serviceResp = new services.ServiceResponse();
              serviceResp.setError(err.message);
              callback(serviceResp.serializeBinary());
          });
    }
}

function OnNotification(data) {

}


callbacker = new ubinder.Callbacker(binding, OnNotification, OnRequest)

run_test = function() {
    run_test_logic(callbacker, console.log, "data");
}