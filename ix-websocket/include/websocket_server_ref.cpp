#include "websocket_server_ref.h"

std::mutex websocket_server_ref_mutex;/* used in constructor, clearAll */
std::map<int, websocket_server_ref *> websocket_server_list;

int websocket_server_ref::getId() {
    
    return _id;
}

websocket_server_ref::websocket_server_ref(PA_ObjectRef params) :
_method(new CUTF16String),
_worker(new CUTF16String) {

    this->setInitParams(params);
    
    _server = new ix::WebSocketServer(_port, _host, _backlog, _maxConnections);
    
    this->setParams(params);
    
    int i = -1;
    
    std::lock_guard<std::mutex> lock(websocket_server_ref_mutex);
    
    while (websocket_server_list.find(i) != websocket_server_list.end()) i--;
    
    this->_id = i;
    
    websocket_server_list.insert(std::map<int, websocket_server_ref *>::value_type(this->getId(), this));
    
    std::cout << "create server: " << _id << std::endl;
}

void websocket_server_ref::getMethod(CUTF16String *method) {
    
    method->assign(_method->c_str(), _method->length());
}

void websocket_server_ref::getWorker(CUTF16String *worker) {
    
    worker->assign(_worker->c_str(), _worker->length());
}

int websocket_server_ref::getProcess() {
    
    return _process;
}

int websocket_server_ref::getWindow() {
    
    return _window;
}

void websocket_server_ref::getParams(PA_ObjectRef params) {
    
    if(params)
    {
        ob_set_i(params, L"ref", _id);
        ob_set_i(params, L"port", _port);
        ob_set_s(params, L"host", _host.c_str());
        ob_set_i(params, L"backlog", _backlog);
        ob_set_i(params, L"maxConnections", (int)_maxConnections);
        
        ob_set_a(params, L"method", _method);
        ob_set_a(params, L"worker", _worker);
        ob_set_i(params, L"window", _window);
        ob_set_i(params, L"process", _process);
    }
}

void websocket_server_ref::setParams(PA_ObjectRef params) {
    
    if(params)
    {
        if(ob_is_defined(params, L"method"))    ob_get_a(params, L"method", _method);
        if(ob_is_defined(params, L"worker"))      ob_get_a(params, L"worker", _worker);
        if(ob_is_defined(params, L"window"))    _window = ob_get_n(params, L"window");
        if(ob_is_defined(params, L"process"))   _process = ob_get_n(params, L"process");
    }
}

void websocket_server_ref::setInitParams(PA_ObjectRef params) {
    
    if(params)
    {
        CUTF8String host;
        if(ob_get_s(params, L"host", &host))
        {
            _host = std::string((const char *)host.c_str(), host.size());
        }
        if(ob_is_defined(params, L"port"))
        {
            _port = ob_get_n(params, L"port");
        }
        if(ob_is_defined(params, L"backlog"))
        {
            _backlog = ob_get_n(params, L"backlog");
        }
        if(ob_is_defined(params, L"maxConnections"))
        {
            _maxConnections = ob_get_n(params, L"maxConnections");
        }
    }
}

ix::WebSocketServer *websocket_server_ref::ptr() {
    
    return _server;
}

void websocket_server_ref::send(PA_ObjectRef params, const std::string& data, PA_CollectionRef results) {

    PA_CollectionRef c = ob_get_c(params, L"clients");
    
    if(c)
    {
        for(PA_long32 i = 0; i < PA_GetCollectionLength(c); ++i)
        {
            PA_Variable v = PA_GetCollectionElement(c, i);
            if(PA_GetVariableKind(v) == eVK_Real)
            {
                int j = (int)PA_GetRealVariable(v);
                std::shared_ptr<ix::WebSocket> client = getClient(j);
                
                if(client)
                {
                    ix::WebSocketSendInfo r = client->send(data);
                    
                    if(results)
                    {
                        PA_ObjectRef result = PA_CreateObject();
                        ob_set_b(result, L"success", r.success);
                        ob_set_b(result, L"compressionError", r.compressionError);
                        ob_set_n(result, L"payloadSize", r.payloadSize);
                        ob_set_n(result, L"wireSize", r.wireSize);
                        ob_set_n(result, L"client", j);
                        PA_Variable v = PA_CreateVariable(eVK_Object);
                        PA_SetObjectVariable(&v, result);
                        PA_SetCollectionElement(results, PA_GetCollectionLength(results), v);
                        PA_ClearVariable(&v);
                    }
                }
            }
        }
    }else
    {
        /* to all */
        std::set<std::shared_ptr<ix::WebSocket>> clients = _server->getClients();
        std::set<std::shared_ptr<ix::WebSocket>>::iterator it;
        for (it = clients.begin(); it != clients.end(); ++it)
        {
            std::shared_ptr<ix::WebSocket> client = *it;
            ix::WebSocketSendInfo r = client->send(data);
            
            if(results)
            {
                PA_ObjectRef result = PA_CreateObject();
                ob_set_b(result, L"success", r.success);
                ob_set_b(result, L"compressionError", r.compressionError);
                ob_set_n(result, L"payloadSize", r.payloadSize);
                ob_set_n(result, L"wireSize", r.wireSize);
                ob_set_n(result, L"client",findClient(client));
                
                PA_Variable v = PA_CreateVariable(eVK_Object);
                PA_SetObjectVariable(&v, result);
                PA_SetCollectionElement(results, PA_GetCollectionLength(results), v);
                PA_ClearVariable(&v);
            }
        } 
    }
}

websocket_server_ref::~websocket_server_ref() {
        
	delete _server;
    delete _method;
    delete _worker;
    
    _clients.clear();
    _clients_reverse.clear();
    
    std::cout << "delete server: " << _id << std::endl;
}

int websocket_server_ref::registerClient(std::shared_ptr<ix::WebSocket>p) {
    
    int i = 1;
    
    while (_clients.find(i) != _clients.end()) i++;
        
    _clients.insert(std::map<int, std::shared_ptr<ix::WebSocket>>::value_type(i, p));
    _clients_reverse.insert(std::map<std::shared_ptr<ix::WebSocket>, int>::value_type(p, i));
    
    return i;
}

int websocket_server_ref::unregisterClient(std::shared_ptr<ix::WebSocket>p) {
    
    int i = 0;
    
    {
        std::map<std::shared_ptr<ix::WebSocket>, int>::iterator it = _clients_reverse.find(p);
        
        if(it != _clients_reverse.end())
        {
            i = it->second;
            
            _clients_reverse.erase(it);
            
            {
                std::map<int, std::shared_ptr<ix::WebSocket>>::iterator it = _clients.find(i);
                
                if(it != _clients.end())
                {
                    _clients.erase(it);
                }
            }
        }
    }

    return i;
}

std::shared_ptr<ix::WebSocket> websocket_server_ref::getClient(int i) {
    
    std::shared_ptr<ix::WebSocket> p = NULL;
 
    std::map<int, std::shared_ptr<ix::WebSocket>>::iterator it = _clients.find(i);
    
    if(it != _clients.end())
    {
        p = it->second;
    }
    
    return p;
}

int websocket_server_ref::findClient(std::shared_ptr<ix::WebSocket>p) {
    
    int i = 0;
    
    std::map<std::shared_ptr<ix::WebSocket>, int>::iterator it = _clients_reverse.find(p);
    
    if(it != _clients_reverse.end())
    {
        std::set<std::shared_ptr<ix::WebSocket>> clients = _server->getClients();
        
        if(clients.find(p) != clients.end())
        {
            i = it->second;
        }
    }
    
    return i;
}

size_t websocket_server_ref::getClients(std::set<int>& c) {
    
    c.clear();
    
    std::set<std::shared_ptr<ix::WebSocket>> clients = _server->getClients();

    std::set<std::shared_ptr<ix::WebSocket>>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it)
    {
        std::shared_ptr<ix::WebSocket> client = *it;
        
        int i = this->findClient(client);
        
        if(i)
        {
            c.insert(i);
        }
        
    }
    
    return c.size();
}

websocket_server_ref *websocket_server_ref::ref(int i) {
        
    websocket_server_ref *ref = NULL;
        
    std::map<int, websocket_server_ref *>::iterator it = websocket_server_list.find(i);
    
    if(it != websocket_server_list.end())
    {
        ref = it->second;
    }
    
    return ref;
}

websocket_server_ref *websocket_server_ref::ptr(PA_ObjectRef params) {
    
    return websocket_server_ref::ref((int)ob_get_n(params, L"ref"));
}

ix::WebSocketServer *websocket_server_ref::find(int i) {
    
    ix::WebSocketServer *server = NULL;
    
    std::map<int, websocket_server_ref *>::iterator it = websocket_server_list.find(i);
    
    if(it != websocket_server_list.end())
    {
        server = it->second->ptr();
    }
    
    return server;
}

ix::WebSocketServer *websocket_server_ref::get(PA_ObjectRef params) {
    
    return websocket_server_ref::find((int)ob_get_n(params, L"ref"));
}

void websocket_server_ref::clear(PA_ObjectRef params) {
    
    std::map<int, websocket_server_ref *>::iterator it = websocket_server_list.find((int)ob_get_n(params, L"ref"));
    
    if(it != websocket_server_list.end())
    {
        delete it->second;
        
        websocket_server_list.erase(it);
    }
}

void websocket_server_ref::clearAll() {
    
    std::lock_guard<std::mutex> lock(websocket_server_ref_mutex);
    
    std::map<int, websocket_server_ref *>::iterator it;
    
    for(it = websocket_server_list.begin(); it != websocket_server_list.end(); ++it)
    {
        delete it->second;
    }
    
    websocket_server_list.clear();
}
