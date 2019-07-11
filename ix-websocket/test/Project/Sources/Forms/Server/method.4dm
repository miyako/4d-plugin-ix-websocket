$event:=Form event:C388

Case of 
	: ($event=On Load:K2:1)
		
		Form:C1466.context:=New object:C1471
		
		$o:=New object:C1471
		$o.method:="callback"
		$o.window:=Current form window:C827
		
		Form:C1466.server:=Websocket server ($o)
		Form:C1466.status:=Websocket server start (Form:C1466.server)
		
	: ($event=On Unload:K2:2)
		
		Websocket server clear (Form:C1466.server)
		
End case 