<?php
require "common.php";

$loginrow=verify_login($login,$password);
require_administrator_privileges($loginrow);

$id=intval($_REQUEST["id"]);

$query= "DELETE FROM residents WHERE id=".$id;
$results= mysql_query($query);

if ($results<=0)
{
  print "an error occured with query ".$query;
  return;
}

header("Location: admin.php");

?>

</html>
