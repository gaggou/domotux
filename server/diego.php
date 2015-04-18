<?php
//Function to check if the request is an AJAX request
function is_ajax() {
  return isset($_SERVER['HTTP_X_REQUESTED_WITH']) && strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest';
}

function diego_act(){
  $return = $_POST;
  $id = intval($_POST["id"]);
  $state = $_POST["state"];
  if ($state == "ON"){
    $return_val = exec("tdtool -f " . $id, $table, $status);
  } else {
    $return_val = exec("tdtool -n " . $id, $table, $status);
  }
  $return["status"] = $status;
  $return["string"] = $return_val;
  $return["id"] = $id;
  return $return;
}
function test_function(){
  $return = $_POST;
  
  $return["json"] = json_encode($return);
  return $return;
}

function get_status(){
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
if (is_ajax()) {
  if (isset($_POST["action"]) && !empty($_POST["action"])) { //Checks if action value exists
    $action = $_POST["action"];
    switch($action) { //Switch case for value of action
      case "test": echo json_encode(test_function()); break;
      case "act":  echo json_encode(diego_act()); break;
      case "get":  echo json_encode(get_status()); break;
    }
    exit;
  }
}
?>
<head>
<script src="http://code.jquery.com/jquery-1.11.0.min.js"></script>
<!--Put the following in the <head>-->
<script type="text/javascript">
$("document").ready(function(){
  $(".setter").click(function(){
    var data = {
      "action": "act",
      "id": $(this).attr("id"),
      "state": $(this).attr("state")
    };
    data = $(this).serialize() + "&" + $.param(data);
    $.ajax({
      type: "POST",
      dataType: "json",
      url: "diego.php", //Relative or absolute path to response.php file
      data: data,
      success: function(data) {
        $(".the-return").html(
          "Command done: " + data["string"] + "<br />status: " + data["status"] + "<br />"
        );
        var target = "#" + data["id"];
        if($(target).attr("state") == "ON"){
          $(target).attr("state", "OFF");
        } else {
          $(target).attr("state", "ON");
        }
      }
    });
    return false;
  });
  $(".getter").click(function(){
    var data = {
      "action": "get"
    };
    data = $(this).serialize() + "&" + $.param(data);
    $.ajax({
      type: "POST",
      dataType: "json",
      url: "diego.php", //Relative or absolute path to response.php file
      data: data,
      success: function(data) {
        $(".the-return").html(
          "Command done: " + data["string"] + "<br />status: " + data["status"] + "<br />"
        );
      }
    });
    return false;
  });
});
</script>
</head>

<body>
<?php
  foreach(get_status() as $binou){
    echo "<button id=\"${binou["id"]}\" state=\"${binou["status"]}\" class=\"setter\">${binou["identifier"]}</button></br>";
  }
?>
<button class="getter">actualiser</button></br>

<div class="the-return">
  [HTML is replaced when successful.]
</div>


</body>
</html>
