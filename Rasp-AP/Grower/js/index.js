function download_csv() {
    var csv = 'containerID,floor,SSID,password,mqtt_ip\n';
    var fl;
    
    csv += document.getElementById("containerID").value;
    csv +=",";

    if(document.getElementById("r1").checked){
      fl = 1;
      csv += "1";
      csv +=",";
    }
    else if(document.getElementById("r2").checked){
      fl = 2;
      csv += "2";
      csv +=",";
    }
    else if(document.getElementById("r3").checked){
      fl = 3;
      csv += "3";
      csv +=",";
    }
    else if(document.getElementById("r4").checked){
      fl = 4;
      csv += "4";
      csv +=",";
    }
    
    csv += document.getElementById("ssid").value;
    csv +=",";
    csv += document.getElementById("passw").value;
    csv +=",";
    csv += document.getElementById("mqttIP").value;
    csv +="\n";

    console.log(csv);
    var hiddenElement = document.createElement('a');
    hiddenElement.href = 'data:text/csv;charset=utf-8,' + encodeURI(csv);
    hiddenElement.target = '_blank';
    hiddenElement.download = 'tocaniConfig'+fl+'.csv';
    hiddenElement.click();
}

function saveData(){
  var contID = document.getElementById("containerID").value;
  var floor = 0;
  var ssid = document.getElementById("ssid").value;
  var password = document.getElementById("passw").value;
  var mqttIP = document.getElementById("mqttIP").value;

  if(document.getElementById("r1").checked){
    floor = 1;
  }
  else if(document.getElementById("r2").checked){
    floor = 2;
  }
  else if(document.getElementById("r3").checked){
    floor = 3;
  }
  else if(document.getElementById("r4").checked){
    floor = 4;
  }

  $.get("php/data.php?contID="+contID+"&floor="+floor+"&ssid="+ssid+"&password="+password+"&mqttIP="+mqttIP, function(data){
        alert(data);
        });
  
}

$(document).ready(function(){

  $(".form-wrapper .button").click(function(){
    var button = $(this);
    var currentSection = button.parents(".section");
    var currentSectionIndex = currentSection.index();
    var headerSection = $('.steps li').eq(currentSectionIndex);
    var control = 0;

    if(currentSectionIndex === 1){
        var IP_REGEXP = /^([0][1-9]|[1-9][0-9])([-])([0][0][1-9]|[1-9][0-9][0-9]|[0-9][1-9][0-9])([-])([0][0][1-9]|[1-9][0-9][0-9]|[0-9][1-9][0-9])$/;
        if( IP_REGEXP.test(document.getElementById("containerID").value) == false || document.getElementById("containerID").value.length != 10){
            control = 1;
            document.getElementById("warn1").innerHTML = "Container ID inválido";
        }
    }

    if(currentSectionIndex === 2 && !document.getElementById("r1").checked && !document.getElementById("r2").checked && !document.getElementById("r3").checked && !document.getElementById("r4").checked ){
      control = 1;
      document.getElementById("warn2").innerHTML = "Seleccione una opción";
    }
    
    if(currentSectionIndex === 3){
      if(document.getElementById("ssid").value == "" || document.getElementById("passw").value == ""){
          control = 1;
          document.getElementById("warn3").innerHTML = "Nombre y/o contraseña incorrectos";
      }
      else if(document.getElementById("ssid").value.length < 6){
          control = 1;
          document.getElementById("warn3").innerHTML = "Nombre de la red no cumple con la longitud mínima";
      }
      else if(document.getElementById("passw").value.length < 6){
          control = 1;
          document.getElementById("warn3").innerHTML = "Contraseña no cumple con la longitud mínima";
      }
      
    }

    if(currentSectionIndex === 4 ){
      var IP_REGEXP = /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z]|[a-zA-Z][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z]|[A-Za-z][A-Za-z0-9\-]*[A-Za-z0-9])$|^\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*$/;
      if( IP_REGEXP.test(document.getElementById("mqttIP").value) == false){
        control = 1;
        document.getElementById("warn4").innerHTML = "Dirección IP inválida";
      }
      else{
          var i;
          var IP_REGEXP = /[0-9]/
          for(i = 0;i<document.getElementById("mqttIP").value.length; i++){
              if(IP_REGEXP.test(+document.getElementById("mqttIP").value.charAt(i)) == false && document.getElementById("mqttIP").value.charAt(i) !== '.' ){
                  control = 1;
                  document.getElementById("warn4").innerHTML = "Dirección IP inválida";
                  break;
              }
          }
      }
    }
    
    if(control === 0){
      document.getElementById("warn1").innerHTML = "";
      document.getElementById("warn2").innerHTML = "";
      document.getElementById("warn3").innerHTML = "";
      document.getElementById("warn4").innerHTML = "";

      currentSection.removeClass("is-active").next().addClass("is-active");
      headerSection.removeClass("is-active").next().addClass("is-active");
      
      $(".form-wrapper").submit(function(e) {
        e.preventDefault();
      });
      
      if(currentSectionIndex === 5){
        $(document).find(".form-wrapper .section").first().addClass("is-active");
        $(document).find(".steps li").first().addClass("is-active");
        download_csv();
        saveData();
     }
    }

  });


  $(".form-wrapper .button1").click(function(){
    var button = $(this);
    var currentSection = button.parents(".section");
    var currentSectionIndex = currentSection.index();
    var headerSection = $('.steps li').eq(currentSectionIndex);
    
 
    currentSection.removeClass("is-active").prev().addClass("is-active");
    headerSection.removeClass("is-active").prev().addClass("is-active");
    
    $(".form-wrapper").submit(function(e) {
      e.preventDefault();
    });
    
  });

});
