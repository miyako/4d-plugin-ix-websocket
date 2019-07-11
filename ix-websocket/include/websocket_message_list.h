#ifndef __WEBSOCKET_MESSAGE_LIST_H__
#define __WEBSOCKET_MESSAGE_LIST_H__ 1

#include "4DPlugin-ix-websocket.h"

#include "websocket_event_queue.h"
#include "websocket_server_ref.h"

#ifdef __cplusplus
extern "C" {
#endif

    class websocket_message
    {
        
    private:
        
        int _server;
        int _client;
        int _window = 0;
        int _process = 0;
        
        CUTF16String *_context;
        CUTF16String *_type;
        CUTF16String *_worker;
        CUTF16String *_method;
        
    public:
        
        websocket_message(int server, int client,
                          const char *type, const char *headers, int _window, int _process);
        
        ~websocket_message();
        
        void getValue(PA_ObjectRef message);

        void getMethod(CUTF16String *method);
        void getWorker(CUTF16String *worker);
        void getContext(CUTF16String *context);
        void getType(CUTF16String *type);
        
        int getServer();
        int getClient();
        int getProcess();
        int getWindow();
        
        static void convert(std::string *fromString, CUTF16String &toString);
    };
    
	class websocket_message_list
	{
		
	private:
		

	public:

        size_t size();
                
        void addMessage(int server, int client,
                        const char *type, std::string& headers, int _window, int _process);
        
        std::vector<websocket_message *>::iterator begin();
        std::vector<websocket_message *>::iterator end();
        std::vector<websocket_message *>::iterator erase(std::vector<websocket_message *>::iterator it);
        void clear();
        
		websocket_message_list();
		~websocket_message_list();
        
	};
    
#ifdef __cplusplus
}
#endif

#endif
