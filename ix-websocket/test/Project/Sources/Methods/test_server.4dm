//%attributes = {}
$o:=New object:C1471
$o.worker:="worker_server"
$o.method:="worker_server"

$server:=Websocket server ($o)
$status:=Websocket server start ($server)

If ($status.success)
	
	Use (Storage:C1525)
		Storage:C1525.server:=New shared object:C1526("ref";$server.ref)
	End use 
	
Else 
	
	$server:=Storage:C1525.server
	C_COLLECTION:C1488($c;$m)
	$c:=Websocket server clients ($server)
	
	$m:=Websocket server messages ($server)
	
	CONVERT FROM TEXT:C1011("Hello!";"utf-8";$data)
	
	$r:=Websocket server send ($server;$data)  //to all
	
	$server:=OB Copy:C1225($server)
	
	$server.clients:=New collection:C1472(1;2;3)
	
	$r:=Websocket server send ($server;$data)  //to some
	
End if 