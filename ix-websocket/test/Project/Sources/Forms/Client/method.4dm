$event:=Form event:C388

Case of 
	: ($event=On Load:K2:1)
		
		Form:C1466.context:=New object:C1471
		
		$o:=New object:C1471
		$o.method:="callback"
		$o.window:=Current form window:C827
		$o.url:="ws://localhost:8080/"
		
		Form:C1466.client:=Websocket client ($o)
		Form:C1466.status:=Websocket client start (Form:C1466.client)
		
	: ($event=On Unload:K2:2)
		
		Form:C1466.status:=Websocket client stop (Form:C1466.client)
		Websocket client clear (Form:C1466.client)
		
End case 