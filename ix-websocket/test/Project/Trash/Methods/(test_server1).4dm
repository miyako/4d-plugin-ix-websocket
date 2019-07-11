//%attributes = {}
$o:=New object:C1471

$o.ref:=-1

CONVERT FROM TEXT:C1011("Hello!";"utf-8";$data)

$status:=websocket server clear ($o)
  //$status:=websocket server stop ($o)