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

If ($server=0)
	
	CALL FORM:C1391($window;"Server";$1;$2;$3;$4;$5;$6;$7)
	
Else 
	
	CALL FORM:C1391($window;"Client";$1;$2;$3;$4;$5;$6;$7)
	
End if 