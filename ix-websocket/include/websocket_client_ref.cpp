#include "websocket_client_ref.h"

std::mutex websocket_client_ref_mutex;/* used in constructor, clear, clearAll */
std::map<int, websocket_client_ref *> websocket_client_list;
/* ref, find should not use mutex; it will creata a dead lock */

int websocket_client_ref::getId() {
    
    return _id;
}

websocket_client_ref::websocket_client_ref(PA_ObjectRef params) :
_method(new CUTF16String),
_worker(new CUTF16String) {

    this->setInitParams(params);
    
    _client = new ix::WebSocket();
    
    this->setParams(params);
    
    int i = 1;
    
    std::lock_guard<std::mutex> lock(websocket_client_ref_mutex);
    
    while (websocket_client_list.find(i) != websocket_client_list.end()) i++;
    
    this->_id = i;
    
    websocket_client_list.insert(std::map<int, websocket_client_ref *>::value_type(this->getId(), this));
    
    std::cout << "create client: " << _id << std::endl;
}

void websocket_client_ref::getMethod(CUTF16String *method) {
    
    method->assign(_method->c_str(), _method->length());
}

void websocket_client_ref::getWorker(CUTF16String *worker) {
    
    worker->assign(_worker->c_str(), _worker->length());
}

int websocket_client_ref::getProcess() {
    
    return _process;
}

int websocket_client_ref::getWindow() {
    
    return _window;
}

void websocket_client_ref::getParams(PA_ObjectRef params) {
    
    if(params)
    {
        ob_set_i(params, L"ref", _id);
        ob_set_s(params, L"url", _client->getUrl().c_str());
        ob_set_i(params, L"pingInterval", _client->getPingInterval());
//        ob_set_i(params, L"pingTimeout", _client->getPingTimeout());
        ob_set_b(params, L"isAutomaticReconnectionEnabled", _client->isAutomaticReconnectionEnabled());
        
        ix::WebSocketPerMessageDeflateOptions perMessageDeflateOptions = _client->getPerMessageDeflateOptions();
        
        bool enabled = perMessageDeflateOptions.enabled();
        bool clientNoContextTakeover = perMessageDeflateOptions.getClientNoContextTakeover();
        bool serverNoContextTakeover = perMessageDeflateOptions.getServerNoContextTakeover();
        uint8_t serverMaxWindowBits = perMessageDeflateOptions.getServerMaxWindowBits();
        uint8_t clientMaxWindowBits = perMessageDeflateOptions.getClientMaxWindowBits();
        
        PA_ObjectRef objOption = PA_CreateObject();
        ob_set_b(objOption, L"enabled", enabled);
        ob_set_b(objOption, L"clientNoContextTakeover", clientNoContextTakeover);
        ob_set_b(objOption, L"serverNoContextTakeover", serverNoContextTakeover);
        ob_set_i(objOption, L"serverMaxWindowBits", serverMaxWindowBits);
        ob_set_i(objOption, L"clientMaxWindowBits", clientMaxWindowBits);
        
        ob_set_o(params, L"perMessageDeflateOptions", objOption);
        
        ob_set_a(params, L"method", _method);
        ob_set_a(params, L"worker", _worker);
        ob_set_i(params, L"window", _window);
        ob_set_i(params, L"process", _process);
    }
}

void websocket_client_ref::setParams(PA_ObjectRef params) {
    
    if(params)
    {
        if(ob_is_defined(params, L"method"))    ob_get_a(params, L"method", _method);
        if(ob_is_defined(params, L"worker"))    ob_get_a(params, L"worker", _worker);
        if(ob_is_defined(params, L"window"))    _window = ob_get_n(params, L"window");
        if(ob_is_defined(params, L"process"))   _process = ob_get_n(params, L"process");
        
        CUTF8String _url;
        if(ob_get_s(params, L"url", &_url))
        {
            std::string url = std::string((const char *)_url.c_str(), _url.size());
            _client->setUrl(url);
        }
        
        if(ob_is_defined(params, L"pingInterval"))
        {
//            _client->setHeartBeatPeriod(ob_get_n(params, L"pingInterval"));
        }
        
        if(ob_is_defined(params, L"pingTimeout"))
        {
//            _client->setPingTimeout(ob_get_n(params, L"pingTimeout"));
        }
        
        if(ob_is_defined(params, L"enableAutomaticReconnection"))
        {
            if(ob_get_b(params, L"enableAutomaticReconnection"))
            {
                _client->enableAutomaticReconnection();
            }else
            {
                _client->disableAutomaticReconnection();
            }
        }
        
        if(ob_is_defined(params, L"enablePong"))
        {
            if(ob_get_b(params, L"enablePong"))
            {
                _client->enablePong();
            }else
            {
                _client->disablePong();
            }
        }
        
        if(ob_is_defined(params, L"perMessageDeflate"))
        {
            PA_ObjectRef objOption = ob_get_o(params, L"perMessageDeflate");
            if(objOption)
            {
                bool enabled = ob_get_b(objOption, L"enabled");
                bool clientNoContextTakeover = ob_get_b(objOption, L"clientNoContextTakeover");
                bool serverNoContextTakeover = ob_get_b(objOption, L"serverNoContextTakeover");
                uint8_t serverMaxWindowBits = ob_get_n(objOption, L"serverMaxWindowBits");
                uint8_t clientMaxWindowBits = ob_get_n(objOption, L"clientMaxWindowBits");
                
                ix::WebSocketPerMessageDeflateOptions options(enabled,
                                                          clientNoContextTakeover,
                                                          serverNoContextTakeover,
                                                          serverMaxWindowBits,
                                                          clientMaxWindowBits);
                _client->setPerMessageDeflateOptions(options);
            }
            
        }else
            if(ob_is_defined(params, L"disablePerMessageDeflate"))
            {
                if(ob_get_b(params, L"disablePerMessageDeflate"))
                {
                    _client->disablePerMessageDeflate();
                }else
                {
                    ix::WebSocketPerMessageDeflateOptions options;
                    _client->setPerMessageDeflateOptions(options);
                }
            }
        
    }
}

void websocket_client_ref::setInitParams(PA_ObjectRef params) {
    
    if(params)
    {

    }
}

ix::WebSocket *websocket_client_ref::ptr() {
    
    return _client;
}

void websocket_client_ref::send(PA_ObjectRef params, const std::string& data, PA_ObjectRef result) {

    ix::WebSocketSendInfo r = _client->send(data);
    
    ob_set_b(result, L"success", r.success);
    ob_set_b(result, L"compressionError", r.compressionError);
    ob_set_n(result, L"payloadSize", r.payloadSize);
    ob_set_n(result, L"wireSize", r.wireSize);
}

websocket_client_ref::~websocket_client_ref() {
    
    _client->stop();
    
    delete _client;
    delete _method;
    delete _worker;
    
    std::cout << "delete client: " << _id << std::endl;
}

websocket_client_ref *websocket_client_ref::ref(int i) {
    
    websocket_client_ref *ref = NULL;
    
    std::map<int, websocket_client_ref *>::iterator it = websocket_client_list.find(i);
    
    if(it != websocket_client_list.end())
    {
        ref = it->second;
    }
    
    return ref;
}

websocket_client_ref *websocket_client_ref::ptr(PA_ObjectRef params) {
    
    return websocket_client_ref::ref((int)ob_get_n(params, L"ref"));
}

ix::WebSocket *websocket_client_ref::find(int i) {

    ix::WebSocket *client = NULL;
    
    std::map<int, websocket_client_ref *>::iterator it = websocket_client_list.find(i);
    
    if(it != websocket_client_list.end())
    {
        client = it->second->ptr();
    }
    
    return client;
}

ix::WebSocket *websocket_client_ref::get(PA_ObjectRef params) {
    
    return websocket_client_ref::find((int)ob_get_n(params, L"ref"));
}

void websocket_client_ref::clear(PA_ObjectRef params) {
    
    std::lock_guard<std::mutex> lock(websocket_client_ref_mutex);
    
    std::map<int, websocket_client_ref *>::iterator it = websocket_client_list.find((int)ob_get_n(params, L"ref"));
    
    if(it != websocket_client_list.end())
    {
        //need to stop first!
        
        delete it->second;
        
        websocket_client_list.erase(it);
    }
}

void websocket_client_ref::clearAll() {
    
    std::lock_guard<std::mutex> lock(websocket_client_ref_mutex);
    
    std::map<int, websocket_client_ref *>::iterator it;
    
    for(it = websocket_client_list.begin(); it != websocket_client_list.end(); ++it)
    {
        delete it->second;
    }
    
    websocket_client_list.clear();
}
