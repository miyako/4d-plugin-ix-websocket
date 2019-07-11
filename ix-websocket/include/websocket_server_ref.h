#ifndef __WEBSOCKET_SERVER_REF_H__
#define __WEBSOCKET_SERVER_REF_H__ 1

#include "4DPlugin-ix-websocket.h"

#ifdef __cplusplus
extern "C" {
#endif

	class websocket_server_ref
	{
		
	private:
		
		ix::WebSocketServer *_server;
        
		CUTF16String* _method;
        CUTF16String* _worker;
        
        int _window = 0;
        int _process = 0;
        
        int _port = ix::SocketServer::kDefaultPort;
        std::string _host = ix::SocketServer::kDefaultHost;
        int _backlog = ix::SocketServer::kDefaultTcpBacklog;
        size_t _maxConnections = ix::SocketServer::kDefaultMaxConnections;
        int _id;
        
        std::map<int, std::shared_ptr<ix::WebSocket>>_clients;
        std::map<std::shared_ptr<ix::WebSocket>, int>_clients_reverse;
        
	public:
		
        int getId();
        
        void setParams(PA_ObjectRef params);
        void setInitParams(PA_ObjectRef params);
        void getParams(PA_ObjectRef params);
        
        void send(PA_ObjectRef params, const std::string& data, PA_CollectionRef results);
        
        ix::WebSocketServer *ptr();
        
        int registerClient(std::shared_ptr<ix::WebSocket>p);
        int unregisterClient(std::shared_ptr<ix::WebSocket>p);
        
        std::shared_ptr<ix::WebSocket>getClient(int i);
        int findClient(std::shared_ptr<ix::WebSocket>p);
        size_t getClients(std::set<int>& clients);
        
        void getMethod(CUTF16String *method);
        void getWorker(CUTF16String *worker);
        int getProcess();
        int getWindow();
        
		websocket_server_ref(PA_ObjectRef params);
		~websocket_server_ref();

        static websocket_server_ref *ref(int i);
        static websocket_server_ref *ptr(PA_ObjectRef params);
        static ix::WebSocketServer *find(int i);
        static ix::WebSocketServer *get(PA_ObjectRef params);
        static void clear(PA_ObjectRef params);
        static void clearAll();
	};

#ifdef __cplusplus
}
#endif

#endif
