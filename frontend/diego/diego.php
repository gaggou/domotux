<?php
try
{
 // On se connecte à MySQL
 $bdd = new PDO('mysql:host=localhost;dbname=diego', 'www', '');
}
catch(Exception $e)
{
 // En cas d'erreur, on affiche un message et on arrête tout
 die('Erreur : '.$e->getMessage());
}
//Function to check if the request is an AJAX request
function is_ajax() {
        return isset($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest';
}

function diego_act($bdd){
  $return = $_POST;
  $id = intval($_POST["id"]);
  $state = $_POST["state"];
  $return["string"] = "ID Not Found";
  $stmt = $bdd->prepare("SELECT * FROM V_control WHERE id = ? AND action = ?;");
  if ($stmt->execute(array($id, $state)) && $row = $stmt->fetch()) {
    $return["string"] = $row["type"] . " set to " . $row["action"] . " with parameter " . $row["parameter"];
  }
  $reponse = $bdd->query('update status set status="' . $state . '" where id ='.$id . ';');
  $return["status"] = $status;
  $return["id"] = $id;
  return $return;
}

function get_status(){
$reponse = $bdd->query('select control.id as id, control.name as identifier, status.status from control inner join type on type.id=control.type left join status on status.id=control.id;');

if ($reponse){
// On affiche chaque entrée une à une
while ($donnees = $reponse->fetch())
{
  $return_val = exec("tdtool -l", $table, $status);
  foreach($table as $line){
    if(strpos($line, "\t")){
        list($id, $long_id, $status) = explode("\t", $line, 3);
        $return[$id]["id"] = $id;
        $return[$id]["identifier"] = htmlentities($long_id, ENT_NOQUOTES, "UTF-8");
        $return[$id]["status"] = $status;
    }
  }
  return $return;
}
}
}
if (is_ajax()) {
  if (isset($_POST["action"]) && !empty($_POST["action"])) { //Checks if action value exists
    $action = $_POST["action"];
    switch($action) { //Switch case for value of action
      case "act":  echo json_encode(diego_act($bdd)); break;
      case "get":  echo json_encode(get_status()); break;
    }
    exit;
  }
  print "Ajax but no action\n";
  exit;
}
?>
