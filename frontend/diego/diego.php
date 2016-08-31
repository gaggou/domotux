<?php
//Function to check if the request is an AJAX request
function is_ajax() {
  //return isset($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest';
  return true;
}

// action
function diego_act($bdd, $id, $state){
  // initialize return value
  $return["string"] = "ID Not Found";
  // Get information of what to do from DB
  $stmt = $bdd->prepare("SELECT * FROM V_control WHERE id = ? AND action = ?;");
  if ($stmt->execute(array($id, $state)) && $row = $stmt->fetch()) {

    // Actually do it
    switch($row["type"]){
      case "td_on_off":
        switch($state){
          case "ON":
            $return["string"] = exec("tdtool -n " . $row["parameter"], $table, $status);
            $return["status"] = $status;
            break;
          case "OFF":
            $return["string"] = exec("tdtool -f " . $row["parameter"], $table, $status);
            $return["status"] = $status;
            break;
          default:
            $return["string"] = "Error: " . $state . " not understood";
        }

        break;
      case "abstract_on_off":
        // Do nothing, as abstract
        $return["string"] = $row["type"] . " set to " . $row["action"] . " with parameter " . $row["parameter"];
        break;
      default:
        $return["string"] = $row["type"] . " set to " . $row["action"] . " with parameter " . $row["parameter"];
        break;
    }

  }

  // Update status DB with new status
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
      case "act":
        // get parameters
        $id = intval($_POST["id"]);
        $state = $_POST["state"];
        echo json_encode(diego_act($bdd, $id, $state));
        break;
      case "get":
        echo json_encode(get_status($bdd));
        break;
    }
    exit;
  }
  print "Ajax but no action\n";
  exit;
}
?>
