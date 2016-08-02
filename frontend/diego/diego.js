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

