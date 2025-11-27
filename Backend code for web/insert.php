<?php
$servername = "localhost";
$username   = "elapukmd_rabindh";   // MySQL username
$password   = "@nibar2001";         // MySQL password
$dbname     = "elapukmd_dht";       // Database name

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

if(isset($_POST['temperature'])){
    $temperature = $_POST['temperature'];
    $sql = "INSERT INTO wc_sensor_data (temperature, created_at) VALUES ('$temperature', NOW())";

    if ($conn->query($sql) === TRUE) {
        echo "New record created successfully";
    } else {
        echo "Error: " . $conn->error;
    }
} else {
    echo "No temperature value received!";
}

$conn->close();
?>
