#include "websocket_event_queue.h"

bool websocket_event_queue_exit = true;

std::mutex *websocket_event_queue_message_list_mutex;
websocket_message_list *websocket_event_queue_message_list = NULL;

#define CALLBACK_SLEEP_TIME 29

websocket_event_queue::websocket_event_queue(websocket_message_list *list, std::mutex *mutex) :
_name(new CUTF16String)
{
    websocket_event_queue_message_list = list;
    websocket_event_queue_message_list_mutex = mutex;
    
    websocket_event_queue::convert("$websocket_event_queue", _name);
    
    _process = PA_NewProcess/*threadSafe*/((void *)loop, _stackSize, (PA_Unichar *)_name->c_str());
    
    std::cout << "create event queue" << std::endl;
}

void websocket_event_queue::ascii(CUTF16String *fromString, std::string *toString) {
    
#ifdef _WIN32
    int len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)fromString->c_str(), fromString->length(), NULL, 0, NULL, NULL);
    
    if(len){
        std::vector<uint8_t> buf(len + 1);
        if(WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)fromString->c_str(), fromString->length(), (LPSTR)&buf[0], len, NULL, NULL)){
            *toString = std::string((const uint8_t *)&buf[0]);
        }
    }else{
        *toString = std::string((const uint8_t *)"");
    }
    
#else
    CFStringRef str = CFStringCreateWithCharacters(kCFAllocatorDefault, (const UniChar *)fromString->c_str(), fromString->length());
    if(str){
        
        size_t size = CFStringGetMaximumSizeForEncoding(
                                                        CFStringGetLength(str),
                                                        kCFStringEncodingUTF8) + sizeof(uint8_t);
        std::vector<uint8_t> buf(size);
        CFIndex len = 0;
        CFStringGetBytes(str,
                         CFRangeMake(
                                     0,
                                     CFStringGetLength(str)),
                         kCFStringEncodingUTF8,
                         0, true, (UInt8 *)&buf[0], size, &len);
        
        *toString = std::string((const char *)&buf[0], len);
        CFRelease(str);
    }
#endif
}

void websocket_event_queue::loop() {
    
    std::cout << "start event queue loop" << std::endl;
    
    PA_long32 currentProcessNumber = PA_GetCurrentProcessNumber();
    
    websocket_event_queue_exit = false;
    
    do {
        
        if(websocket_event_queue_message_list->size())
        {
            if (websocket_event_queue_message_list_mutex->try_lock()) {
             
                std::vector<websocket_message *>::iterator it;
                
                for(it = websocket_event_queue_message_list->begin(); it != websocket_event_queue_message_list->end(); it = websocket_event_queue_message_list->erase(it))
                {
                    websocket_message *m = *it;
                    
                    CUTF16String _method, _context, _worker, _type;
                    
                    m->getMethod(&_method);
                    PA_long32 methodId = PA_GetMethodID((PA_Unichar *)_method.c_str());
                    
                    if(methodId)
                    {
                        m->getContext(&_context);
                        m->getWorker(&_worker);
                        m->getType(&_type);
                        
                        int server = m->getServer();
                        int client = m->getClient();
                        int window = m->getWindow();
                        int process = m->getProcess();
                        
                        PA_Variable params[7];
                        
                        /* it is necessary to create and clear variables when passing them to PA_ExecuteMethodByID */
                        
                        params[2] = PA_CreateVariable(eVK_Unistring);
                        params[3] = PA_CreateVariable(eVK_Unistring);
                        params[4] = PA_CreateVariable(eVK_Unistring);
                        
                        params[0] = PA_CreateVariable(eVK_Longint);
                        params[1] = PA_CreateVariable(eVK_Longint);
                        params[5] = PA_CreateVariable(eVK_Longint);
                        params[6] = PA_CreateVariable(eVK_Longint);
                        
                        PA_SetLongintVariable(&params[0], server);//$1
                        PA_SetLongintVariable(&params[1], client);//$2
                        
                        PA_Unistring type = PA_CreateUnistring((PA_Unichar *)_type.c_str());
                        PA_SetStringVariable(&params[2], &type);//$3
                        
                        PA_Unistring context = PA_CreateUnistring((PA_Unichar *)_context.c_str());
                        PA_SetStringVariable(&params[3], &context);//$4
                        
                        PA_Unistring worker = PA_CreateUnistring((PA_Unichar *)_worker.c_str());
                        PA_SetStringVariable(&params[4], &worker);//$5
                        PA_SetLongintVariable(&params[5], window);//$6
                        PA_SetLongintVariable(&params[6], process);//$7
                        
                        PA_ExecuteMethodByID(methodId, params, 7);
                        
                        PA_ClearVariable(&params[0]);
                        PA_ClearVariable(&params[1]);
                        PA_ClearVariable(&params[2]);
                        PA_ClearVariable(&params[3]);
                        PA_ClearVariable(&params[4]);
                        PA_ClearVariable(&params[5]);
                        PA_ClearVariable(&params[6]);
                        
                        if(PA_IsProcessDying())
                        {
                            websocket_event_queue_exit = true;
                            std::cout << "abort event queue loop" << std::endl;
                            break;
                        }
                        
                            
                        //TODO: break in case of emergency
                        
                    }
                    
                }
                
                websocket_event_queue_message_list_mutex->unlock();
            }else
            {
                std::cout << "message list is busy!" << std::endl;
            }

        }else
        {
            if(!websocket_event_queue_exit)
            {
                /* DELAY PROCESS does not work for PA_NewProcess */
                PA_PutProcessToSleep(currentProcessNumber, CALLBACK_SLEEP_TIME);
            }

        }
        
        PA_YieldAbsolute();
        
    } while (!websocket_event_queue_exit);
        
    std::cout << "end event queue loop" << std::endl;
}

websocket_event_queue::~websocket_event_queue() {
    
    websocket_event_queue_exit = true;
    
    delete _name;
    
    std::cout << "delete event queue" << std::endl;
}

void websocket_event_queue::convert(const char *fromString, CUTF16String *toString) {
    
#ifdef _WIN32
    int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)fromString, strlen(fromString), NULL, 0);
    
    if(len){
        std::vector<uint8_t> buf((len + 1) * sizeof(PA_Unichar));
        if(MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)fromString, strlen(fromString), (LPWSTR)&buf[0], len)){
            *toString = CUTF16String((const PA_Unichar *)&buf[0]);
        }
    }else{
        *toString = CUTF16String((const PA_Unichar *)L"\0\0");
    }
#else
    CFStringRef str = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)fromString, strlen(fromString), kCFStringEncodingUTF8, true);
    if(str){
        CFIndex len = CFStringGetLength(str);
        std::vector<uint8_t> buf((len+1) * sizeof(PA_Unichar));
        CFStringGetCharacters(str, CFRangeMake(0, len), (UniChar *)&buf[0]);
        *toString = CUTF16String((const PA_Unichar *)&buf[0]);
        CFRelease(str);
    }
#endif
}
