//%attributes = {}
$o:=New object:C1471

$o.worker:="worker_client"
$o.method:="worker_client"

$o.url:="ws://localhost:8080/"

$client:=Websocket client ($o)

$status:=Websocket client start ($client)

CONVERT FROM TEXT:C1011("Hello!";"utf-8";$data)

$status:=Websocket client send ($client;$data)

  //$client:=Websocket client stop ($client)

  //Websocket client clear ($client)


