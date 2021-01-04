<?php
    // Get inputs
    $contID = $_GET["contID"];
    $floor = $_GET["floor"];
    $ssid = $_GET["ssid"];
    $password = $_GET["password"];
    $mqttIP = $_GET["mqttIP"];
    
    // Set header "content-type" as "JSON" in order jQuery recognize it
    header('Content-Type: application/json');

    // Save data in array
    $datos = array(
        'containerID' => $contID,
        'floor' => $floor,
        'ssid' => $ssid,
        'password' => $password,
        'mqttIP' => $mqttIP
        );
    
    $myJSON = json_encode($datos, JSON_FORCE_OBJECT);
    // Save info in json file
    if(file_put_contents("../data/data.json", $myJSON)){
        //$out = exec("/usr/bin/python ../python/addWiFi.py");
        exec("sudo systemctl stop hostapd", $OUT2);
        exec("sudo systemctl stop dnsmasq", $OUT3);
        exec("sudo cp /etc/dhcpcd.conf.orig /etc/dhcpcd.conf", $OUT4);
        exec("sudo systemctl restart dhcpcd", $OUT5);
        exec("sudo cp /etc/hostapd/hostapd.conf.orig /etc/hostapd/hostapd.conf", $OUT6);
        exec("sudo cp /etc/default/hostapd.orig /etc/default/hostapd" ,$OUT7);
        exec("sudo cp /etc/init.d/hostapd.orig /etc/init.d/hostapd", $OUT8);
        exec("sudo cp /etc/dnsmasq.conf.orig /etc/dnsmasq.conf", $OUT8);
        exec("sudo cp /etc/sysctl.conf.orig /etc/sysctl.conf", $OUT9);
        exec("sudo cp /etc/rc.local.orig /etc/rc.local", $OUT10);
        exec("sudo systemctl restart dnsmasq", $OUT11);
        echo "Data saved succesfully";

    }
    else echo "Error: Data did not save";

?>
