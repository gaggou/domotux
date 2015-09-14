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
    $return_val = exec("echo tdtool -n " . $id, $table, $status);
  } else {
    $return_val = exec("echo tdtool -f " . $id, $table, $status);
  }
  $return["status"] = $status;
  $return["string"] = $return_val;
  $return["id"] = $id;
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
      case "act":  echo json_encode(diego_act()); break;
      case "get":  echo json_encode(get_status()); break;
    }
    exit;
  }
  print "Ajax but no action\n";
  exit;
}
?>
<head>
<meta name="viewport" content="width=device-width, initial-scale=2.0">
<link rel="stylesheet" href="jquery.mobile-1.4.5.min.css">
<script src="jquery-1.11.1.min.js"></script>
<script src="jquery.mobile-1.4.5.min.js"></script>
<!--Put the following in the <head>-->
<script type="text/javascript">
$("document").ready(function(){
  $(".setter").on('change', function(){
        $(".the-return").html(
          "Sending data<br />"
        );
    if($(this).attr("state") == "ON"){
      $(this).attr("state", "OFF");
    } else {
      $(this).attr("state", "ON");
    }
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
<style>
    .right_button {float: right; text-align: right;}
</style>
</head>

<body>
<div data-role="page">
    <div data-role="header">
        <h1>Controle via Tellstick</h1>
        <a href="#nav-panel" data-icon="bars" nodisc-icon="" data-iconpos="notext">Menu</a>
        <a href="#" class="getter ui-btn ui-shadow ui-corner-all ui-btn-icon-notext ui-btn-inline ui-icon-refresh">actualiser</a>
    </div><!-- /header -->
    <div role="main" class="ui-content jqm-content jqm-fullwidth">
<form>
<?php
  foreach(get_status() as $binou){
?>
<div class="ui-grid-a row">
<div class="ui-block-a">
    <label for="<?= $binou["id"] ?>"> <?= $binou["identifier"] ?></label>
</div>
<div class="right_button ui-block-b">
    <input data-role="flipswitch" id="<?= $binou["id"]?>" state="<?= $binou["status"]?>" class="setter" type="checkbox" <?php if ($binou["status"] == "ON") echo 'checked=""';?>/>
</div>
</div>
<?php
  }
?>
</form>
    </div><!-- /content -->
    <div data-role="panel" data-display="push" data-theme="b" id="nav-panel">
<div data-role="collapsible" >
<h4> Status </h4>
<div class="the-return">
  status
</div>
    </div><!-- /panel -->

</div>


</body>
</html>
