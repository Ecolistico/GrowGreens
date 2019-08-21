function download_csv() {
    var csv = 'estado,municipio,numero\n';
    
    csv += document.getElementById("estado").value;
    csv +=",";
    csv += document.getElementById("municipio").value;
    csv +=",";
    csv += document.getElementById("num").value;
    
    console.log(csv);
    var hiddenElement = document.createElement('a');
    hiddenElement.href = 'data:text/csv;charset=utf-8,' + encodeURI(csv);
    hiddenElement.target = '_blank';
    hiddenElement.download = 'config.csv';
    hiddenElement.click();
}

function saveData(){
  var state = document.getElementById("estado").value;
  var city = document.getElementById("municipio").value;
  var number = document.getElementById("num").value;
  
  $.get("php/data.php?state="+state+"&city="+city+"&num="+number, function(data){
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
      var state = document.getElementById("estado").value;
      var city = document.getElementById("municipio").value;
      var consecutive = document.getElementById("num").value;
      
      if(state == ""){
        control = 1;
        document.getElementById("warn1").innerHTML = "Seleccione un Estado";
      }
      else if(city == ""){
        control = 1;
        document.getElementById("warn1").innerHTML = "Seleccione una Ciudad";
      }
      else if( Number(consecutive)<=0 || Number(consecutive)>=1000 || consecutive==""){
         control = 1;
         document.getElementById("warn1").innerHTML = "Ingrese un número entre 1 y 999";
      }
    }
    
    if(control === 0){
      document.getElementById("warn1").innerHTML = "";

      currentSection.removeClass("is-active").next().addClass("is-active");
      headerSection.removeClass("is-active").next().addClass("is-active");
      
      $(".form-wrapper").submit(function(e) {
        e.preventDefault();
      });
      
      if(currentSectionIndex === 2){
        $(document).find(".form-wrapper .section").first().addClass("is-active");
        $(document).find(".steps li").first().addClass("is-active");
        //download_csv();
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
