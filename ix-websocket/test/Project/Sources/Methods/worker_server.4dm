//%attributes = {}
C_LONGINT:C283($1;$2)
C_TEXT:C284($3;$4;$5)
C_LONGINT:C283($6;$7)

$server:=$1
$client:=$2
$type:=$3  //open, close, error, fragment, ping, pong, message
$context:=$4  //depends on type (json)
$worker:=$5  //if passed on init
$window:=$6  //if passed on init
$process:=$7  //if passed on init

If ($server#0)  //server
	
	Case of 
		: ($type="message")
			
			C_COLLECTION:C1488($mm)
			$mm:=Websocket client messages (New object:C1471("ref";$server))
			
			CONVERT FROM TEXT:C1011($context;"utf-8";$data)
			$status:=Websocket server send (New object:C1471("ref";$server);$data)
			
	End case 
	
Else   //client
	
End if 

TRACE:C157