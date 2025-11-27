<?php
// Debugging (optional: show errors during development)
error_reporting(E_ALL);
ini_set('display_errors', 1);

$servername = "localhost";
$username   = "elapukmd_rabindh";
$password   = "@nibar2001";
$dbname     = "elapukmd_dht";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

// Get last 3 records
$sql = "SELECT * FROM wc_sensor_data ORDER BY id DESC LIMIT 3";
$result = $conn->query($sql);

$labels = [];
$values = [];

if ($result->num_rows > 0) {
  while($row = $result->fetch_assoc()) {
    $labels[] = $row['created_at'];
    $values[] = $row['temperature'];
    $rows[] = $row;
  }
}
$conn->close();
?>
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>Temperature Dashboard</title>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body class="bg-light">

<div class="container mt-5">
  <h2 class="text-center mb-4">🌡️ Last 3 Temperature Records</h2>
  <div class="card shadow-lg rounded-4 mb-4">
    <div class="card-body">
      <table class="table table-striped table-bordered text-center">
        <thead class="table-dark">
          <tr>
            <th>ID</th>
            <th>Temperature (°C)</th>
            <th>Timestamp</th>
          </tr>
        </thead>
        <tbody>
          <?php
          if (!empty($rows)) {
            foreach ($rows as $row) {
              echo "<tr>";
              echo "<td>".$row["id"]."</td>";
              echo "<td>".$row["temperature"]."</td>";
              echo "<td>".$row["created_at"]."</td>";
              echo "</tr>";
            }
          } else {
            echo "<tr><td colspan='3'>No data found</td></tr>";
          }
          ?>
        </tbody>
      </table>
    </div>
  </div>

  <!-- Graph Section -->
  <div class="card shadow-lg rounded-4">
    <div class="card-body">
      <h4 class="text-center">📊 Temperature Graph</h4>
      <canvas id="tempChart"></canvas>
    </div>
  </div>
</div>

<script>
const ctx = document.getElementById('tempChart');

new Chart(ctx, {
  type: 'line',
  data: {
    labels: <?php echo json_encode($labels); ?>,
    datasets: [{
      label: 'Temperature (°C)',
      data: <?php echo json_encode($values); ?>,
      borderColor: 'blue',
      backgroundColor: 'lightblue',
      tension: 0.3,
      fill: true,
      pointRadius: 6,
      pointBackgroundColor: 'red'
    }]
  },
  options: {
    responsive: true,
    scales: {
      y: {
        beginAtZero: false
      }
    }
  }
});
</script>

</body>
</html>
