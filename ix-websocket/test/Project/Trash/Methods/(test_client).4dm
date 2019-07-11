//%attributes = {}
$w:=Open form window:C675("Client")

$o:=New object:C1471
$o.method:="callback"
$o.window:=$w
$o.url:="ws://localhost:8080/"

$client:=Websocket client ($o)
$status:=Websocket client start ($client)

If ($status.success)
	
	$o:=New object:C1471("client";$client;"context";New object:C1471)
	DIALOG:C40("Client";$o)
	
Else 
	Websocket client clear ($client)
End if 

CLOSE WINDOW:C154($w)