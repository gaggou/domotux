<head>
<link rel="stylesheet" href="jquery.mobile-1.4.5.min.css">
<meta name="viewport" content="width=device-width, initial-scale=2.0">
<script src="jquery-1.11.1.min.js"></script>
<script src="jquery.mobile-1.4.5.min.js"></script>
<script src="diego.js"></script>
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

$reponse = $bdd->query('SELECT * FROM V_status;');

if ($reponse){
// On affiche chaque entrée une à une
while ($donnees = $reponse->fetch())
{
?>
<div class="ui-grid-a row">
<div class="ui-block-a">
    <label for="<?= $donnees["id"] ?>"> <?= htmlentities($donnees["identifier"], ENT_NOQUOTES, "ISO-8859-1") ?></label>
</div>
<div class="right_button ui-block-b">
    <input data-role="flipswitch" id="<?= $donnees["id"]?>" state="<?= $donnees["status"]?>" class="setter" type="checkbox" <?php if ($donnees["status"] == "ON") echo 'checked=""';?>/>
</div>
</div>
<?php
}
$reponse->closeCursor(); // Termine le traitement de la requête
} else {
?>
    <p>
Erreur: requete mal formee
   </p>
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

