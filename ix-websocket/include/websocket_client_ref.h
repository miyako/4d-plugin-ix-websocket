#ifndef __WEBSOCKET_CLIENT_REF_H__
#define __WEBSOCKET_CLIENT_REF_H__ 1

#include "4DPlugin-ix-websocket.h"

#ifdef __cplusplus
extern "C" {
#endif

	class websocket_client_ref
	{
		
	private:
		
		ix::WebSocket *_client;
        
		CUTF16String* _method;
        CUTF16String* _worker;
        
        int _window = 0;
        int _process = 0;
        
        int _id;
        
	public:
		
        int getId();
        
        void setParams(PA_ObjectRef params);
        void setInitParams(PA_ObjectRef params);
        void getParams(PA_ObjectRef params);
        
        void send(PA_ObjectRef params, const std::string& data, PA_ObjectRef results);
        
        ix::WebSocket *ptr();
 
        void getMethod(CUTF16String *method);
        void getWorker(CUTF16String *worker);
        int getProcess();
        int getWindow();
        
		websocket_client_ref(PA_ObjectRef params);
		~websocket_client_ref();

        static websocket_client_ref *ref(int i);
        static websocket_client_ref *ptr(PA_ObjectRef params);
        static ix::WebSocket *find(int i);
        static ix::WebSocket *get(PA_ObjectRef params);
        static void clear(PA_ObjectRef params);
        static void clearAll();

	};
    
#ifdef __cplusplus
}
#endif

#endif
