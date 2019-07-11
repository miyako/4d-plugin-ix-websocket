#include "websocket_message_list.h"

std::mutex websocket_message_list_mutex; /* not used */
std::vector<websocket_message *> messages;

websocket_message::websocket_message(int server, int client,
                                     const char *type, const char *headers, int window, int process) :
_context(new CUTF16String),
_type(new CUTF16String),
_worker(new CUTF16String),
_method(new CUTF16String)
{
    _server = server;
    _client = client;

    _window = window;
    _process = process;
    
    websocket_event_queue::convert(type, _type);
    websocket_event_queue::convert(headers, _context);
    
    websocket_server_ref *ref = websocket_server_ref::ref(server);
    
    if(ref)
    {
        ref->getWorker(_worker);
        ref->getMethod(_method);
        
    }else
    {
        websocket_client_ref *ref = websocket_client_ref::ref(client);
        
        if(ref)
        {
            ref->getMethod(_method);
            ref->getWorker(_worker);
        }
    }
        
    /* can't use native json api here           */
    /* to parse json, use a 3rd party library   */
    /*
     
    using namespace std;
    using namespace Json;
    
    Value root;
    CharReaderBuilder builder;
    string errors;
    
    CharReader *reader = builder.newCharReader();
    bool parse = reader->parse(_context->c_str(),
                               _context->c_str() + _context->size(),
                               &root,
                               &errors);

    if(parse)
    {
        for(Value::const_iterator it = root.begin() ; it != root.end() ; it++)
        {
            JSONCPP_STRING name = it.name();
            
            if(it->isString())
            {
                string value = it->asString();
            }
        }
    }
     
     */
    
    std::cout << "create message" << std::endl;
}

void websocket_message::getMethod(CUTF16String *method) {
    
    method->assign(_method->c_str(), _method->length());
}

void websocket_message::getWorker(CUTF16String *worker) {
    
    worker->assign(_worker->c_str(), _worker->length());
}

void websocket_message::getContext(CUTF16String *context) {
    
    context->assign(_context->c_str(), _context->length());
}

void websocket_message::getType(CUTF16String *type) {
    
    type->assign(_type->c_str(), _type->length());
}

int websocket_message::getServer() {
    
    return _server;
}

int websocket_message::getClient() {
    
    return _client;
}

int websocket_message::getProcess() {
    
    return _process;
}

int websocket_message::getWindow() {
    
    return _window;
}

void websocket_message::getValue(PA_ObjectRef message) {
    
    if(message)
    {
        ob_set_a(message, L"type", _type);
        
        {
            PA_Unistring string = PA_CreateUnistring((PA_Unichar*)_context->c_str());
            PA_Variable params[2];
            PA_SetStringVariable(&params[0], &string);
            PA_SetLongintVariable(&params[1], eVK_Object);
            PA_Variable result = PA_ExecuteCommandByID( /*JSON Parse*/1218, params, 2 );
            PA_ObjectRef object = PA_GetObjectVariable(result);
            PA_DisposeUnistring(&string);
            ob_set_o(message, L"context", object);
        }

    }
    
}

websocket_message::~websocket_message() {
    
    delete _type;
    delete _context;
    delete _worker;
    delete _method;
    
    std::cout << "delete message" << std::endl;
}

websocket_message_list::websocket_message_list() {

    std::cout << "create message list" << std::endl;
}

websocket_message_list::~websocket_message_list() {
    
    this->clear();
    
    std::cout << "delete message list" << std::endl;
}

void websocket_message_list::addMessage(int server, int client, const char *type, std::string& headers, int window, int process) {
    
    websocket_message *message = new websocket_message(server, client, type, headers.c_str(), window, process);
    
    messages.push_back(message);
}

size_t websocket_message_list::size() {
    
    return messages.size();
}

std::vector<websocket_message *>::iterator websocket_message_list::begin() {
    
    return messages.begin();
}

std::vector<websocket_message *>::iterator websocket_message_list::end() {
    
    return messages.end();
}

void websocket_message_list::clear() {
    
    std::vector<websocket_message *>::iterator it;
    
    for(it = this->begin(); it != this->end(); it = this->erase(it))
    {
        
    }
    
    return messages.clear();
}

std::vector<websocket_message *>::iterator websocket_message_list::erase(std::vector<websocket_message *>::iterator it) {
    
    std::lock_guard<std::mutex> lock(websocket_message_list_mutex);
    
    websocket_message *message = *it;
    delete message;
    
    return messages.erase(it);
}

void websocket_message::convert(std::string *fromString, CUTF16String &toString) {
    
#ifdef _WIN32
    int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)fromString->c_str(), fromString->length(), NULL, 0);
    
    if(len){
        std::vector<uint8_t> buf((len + 1) * sizeof(PA_Unichar));
        if(MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)fromString->c_str(), fromString->length(), (LPWSTR)&buf[0], len)){
            toString = CUTF16String((const PA_Unichar *)&buf[0]);
        }
    }else{
        toString = CUTF16String((const PA_Unichar *)L"\0\0");
    }
#else
    CFStringRef str = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)fromString->c_str(), fromString->length(), kCFStringEncodingUTF8, true);
    if(str){
        CFIndex len = CFStringGetLength(str);
        std::vector<uint8_t> buf((len+1) * sizeof(PA_Unichar));
        CFStringGetCharacters(str, CFRangeMake(0, len), (UniChar *)&buf[0]);
        toString = CUTF16String((const PA_Unichar *)&buf[0]);
        CFRelease(str);
    }
#endif
}
