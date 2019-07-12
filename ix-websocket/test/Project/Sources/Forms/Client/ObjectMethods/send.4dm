C_TEXT:C284($message)

If (OBJECT Get name:C1087(Object with focus:K67:3)="message")
	$message:=Get edited text:C655
Else 
	$message:=Form:C1466.message
End if 

CONVERT FROM TEXT:C1011($message;"utf-8";$data)

Form:C1466.status:=Websocket client send (Form:C1466.client;$data)