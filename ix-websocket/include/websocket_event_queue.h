#ifndef __WEBSOCKET_EVENT_QUEUE_H__
#define __WEBSOCKET_EVENT_QUEUE_H__ 1

#include "4DPlugin-ix-websocket.h"

#include "websocket_message_list.h"

#ifdef __cplusplus
extern "C" {
#endif

    class websocket_message_list;
    
    class websocket_event_queue
    {
        
    private:
        
        PA_long32 _stackSize = 0;
        PA_long32 _process = 0;
        CUTF16String* _name;
        
    public:
        
        websocket_event_queue(websocket_message_list *list, std::mutex *mutex);
        ~websocket_event_queue();
        
        static void ascii(CUTF16String *fromString, std::string *toString);
        
        void quit();
        
        static void loop();
        
        static void convert(const char *fromString, CUTF16String *toString);
    };
    
#ifdef __cplusplus
}
#endif

#endif
