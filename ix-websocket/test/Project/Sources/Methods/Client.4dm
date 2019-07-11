//%attributes = {"invisible":true}
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

Form:C1466.context:=JSON Parse:C1218($context)