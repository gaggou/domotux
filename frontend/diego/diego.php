<?php
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
  $stmt = $bdd->prepare("UPDATE status SET status = ? WHERE id = ?;");
  if(!$stmt->execute(array($state,$id))){
    put_r($stmt->errorInfo());
  }
  $return["status"] = $status;
  $return["id"] = $id;
  return $return;
}

function get_status($bdd){
  $reponse = $bdd->query('SELECT * FROM V_status;');

  if ($reponse){
    // On affiche chaque entrée un a une
    while ($donnees = $reponse->fetch())
    {
      $id = $donnees["id"];
      $return[$id]["id"] = $donnees["id"];
      $return[$id]["identifier"] = htmlentities($donnees["identifier"], ENT_NOQUOTES, "ISO-8859-1");
      $return[$id]["status"] = $donnees["status"];
    }
    return $return;
  }
}

if (is_ajax()) {
  try
  {
    // On se connecte a MySQL
    $bdd = new PDO('mysql:host=localhost;dbname=diego', 'www', '');
  }
  catch(Exception $e)
  {
    // En cas d'erreur, on affiche un message et on arrête tout
    die('Erreur : '.$e->getMessage());
  }
  if (isset($_POST["action"]) && !empty($_POST["action"])) { //Checks if action value exists
    $action = $_POST["action"];
    switch($action) { //Switch case for value of action
      case "act":  echo json_encode(diego_act($bdd)); break;
      case "get":  echo json_encode(get_status($bdd)); break;
    }
    exit;
  }
  print "Ajax but no action\n";
  exit;
}
?>
