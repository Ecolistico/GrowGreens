<?php
    // Get inputs
    $state = $_GET["state"];
    $city = $_GET["city"];
    $num = $_GET["num"];
    
    
    if (!is_dir('../data')) {
      // dir doesn't exist, make it
      mkdir('../data/');
      echo "Creating Data Folder\n";
    }
    
    // Set header "content-type" as "JSON" in order jQuery recognize it
    header('Content-Type: application/json');

    
    // Save data in array
    $datos = array(
        'state' => $state,
        'city' => $city,
        'num' => $num
        );

    $myJSON = json_encode($datos, JSON_FORCE_OBJECT);
    // Save info in json file
    if(file_put_contents("../data/config.json", $myJSON)){
        $out = exec("sudo cp /var/www/growgreens-setup.com/public/data/config.json /home/pi/Documents/Grow-Master/config.json");
        echo "Data Saved Correctly";
    }
    else echo "Error: Data did not save";
    
?>
