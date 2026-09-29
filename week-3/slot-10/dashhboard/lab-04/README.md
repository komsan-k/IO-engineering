# Lab 4: Real-Time IoT Data Visualization Using MQTT, JSON, Chart.js, and GitHub.io

## 1. Introduction

In Lab 3, the ESP32 transmitted multiple sensor variables using a
structured JSON message. The GitHub.io dashboard received the MQTT
message, parsed the JSON payload, and displayed temperature, humidity,
and light values.

In this laboratory, the dashboard is extended from a simple numerical
display to a **real-time data visualization system**.

Instead of showing only the most recent value, the dashboard will
maintain a short history of received MQTT data and display the
measurements as dynamic charts.

The system architecture is:

``` text
Sensors
   ↓
ESP32
   ↓
JSON
   ↓
MQTT
   ↓
MQTT Broker
   ↓
MQTT over WSS
   ↓
GitHub.io Dashboard
   ↓
Real-Time Charts
```

The dashboard will use **Chart.js** in the browser to visualize incoming
sensor values.

------------------------------------------------------------------------

# 2. Objectives

After completing this laboratory, students should be able to:

1.  Explain the role of visualization in IoT systems.
2.  Receive structured MQTT JSON data in a web dashboard.
3.  Extract multiple sensor variables from a JSON message.
4.  Store recent sensor samples in browser memory.
5.  Create dynamic charts using Chart.js.
6.  Update chart data when new MQTT messages arrive.
7.  Limit the number of samples displayed on a live dashboard.
8.  Interpret trends in temperature, humidity, and light data.
9.  Distinguish between real-time visualization and permanent historical
    storage.
10. Explain the limitations of using browser memory for IoT history.

------------------------------------------------------------------------

# 3. System Architecture

The complete system is:

``` text
┌───────────────────────┐
│       Sensors         │
│                       │
│ Temperature           │
│ Humidity              │
│ Light                 │
└───────────┬───────────┘
            │
            ▼
┌───────────────────────┐
│        ESP32          │
│                       │
│ Read Data             │
│ Create JSON           │
│ Publish MQTT          │
└───────────┬───────────┘
            │
            │ MQTT
            ▼
┌───────────────────────┐
│     MQTT Broker       │
└───────────┬───────────┘
            │
            │ WSS
            ▼
┌─────────────────────────────┐
│         GitHub.io           │
│                             │
│ MQTT.js                     │
│ JSON.parse()                │
│ Chart.js                    │
│                             │
│ Numeric Values              │
│ Real-Time Charts            │
└─────────────────────────────┘
```

------------------------------------------------------------------------

# 4. Input JSON Message

The ESP32 can continue using the JSON structure from Lab 3:

``` json
{
  "device": "ESP32-01",
  "sequence": 125,
  "temperature": 28.5,
  "humidity": 72.0,
  "light": 640
}
```

The MQTT topic may be:

``` text
iot/lab/sensors
```

or, for a shared laboratory:

``` text
iot/lab/student01/sensors
```

------------------------------------------------------------------------

# 5. Real-Time Data Visualization

A real-time dashboard continuously receives data and updates the user
interface.

For example:

``` text
Time        Temperature
10:00:01      28.1
10:00:03      28.2
10:00:05      28.5
10:00:07      28.6
10:00:09      28.4
```

Instead of showing only:

``` text
Temperature: 28.4 °C
```

the dashboard can display the trend:

``` text
Temperature

29 ┤
28 ┤      ●──●
27 ┤   ●
26 ┤
   └────────────────
      Time →
```

This allows the user to observe changes rather than only the current
state.

------------------------------------------------------------------------

# Part A --- Chart.js

## 6. Chart.js Library

Chart.js is a JavaScript library for browser-based charts.

Add:

``` html
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
```

to the HTML page.

The dashboard therefore uses:

``` text
MQTT.js
   ↓
Receive MQTT Data

JSON.parse()
   ↓
Extract Sensor Values

Chart.js
   ↓
Visualize Data
```

------------------------------------------------------------------------

# 7. Basic Chart Structure

A chart requires an HTML canvas:

``` html
<canvas id="temperatureChart"></canvas>
```

JavaScript then creates the chart:

``` javascript
const ctx =
    document.getElementById(
        "temperatureChart"
    );

const temperatureChart =
    new Chart(ctx, {
        type: "line",
        data: {
            labels: [],
            datasets: [{
                label: "Temperature °C",
                data: []
            }]
        }
    });
```

The arrays initially contain no values:

``` text
labels = []

data = []
```

New values are added when MQTT messages arrive.

------------------------------------------------------------------------

# Part B --- Dashboard Design

## 8. Dashboard Layout

The dashboard should contain both numeric displays and charts.

Example:

``` text
┌───────────────────────────────────────┐
│           IoT Dashboard               │
│                                       │
│ Device: ESP32-01                      │
│ MQTT: Connected                       │
│                                       │
│ Temperature: 28.5 °C                  │
│ Humidity: 72 %                        │
│ Light: 640                            │
│                                       │
│ Temperature Chart                     │
│     ────────╱╲──────                  │
│                                       │
│ Humidity Chart                        │
│     ─────╲────╱─────                  │
│                                       │
│ Light Chart                           │
│     ───╱╲╱╲────────                   │
└───────────────────────────────────────┘
```

------------------------------------------------------------------------

# 9. Complete HTML Dashboard

Create or modify `index.html`.

``` html
<!DOCTYPE html>

<html>

<head>

<title>Lab 4 IoT Dashboard</title>

<script src=
"https://unpkg.com/mqtt/dist/mqtt.min.js">
</script>

<script src=
"https://cdn.jsdelivr.net/npm/chart.js">
</script>

<style>

body {
    font-family: Arial;
    margin: 20px;
    text-align: center;
}

.dashboard {
    max-width: 900px;
    margin: auto;
}

.card {
    display: inline-block;
    width: 220px;
    margin: 10px;
    padding: 20px;
    border: 1px solid #ccc;
    border-radius: 12px;
}

.value {
    font-size: 32px;
}

.chart-box {
    margin-top: 30px;
}

canvas {
    max-height: 300px;
}

</style>

</head>

<body>

<div class="dashboard">

<h1>IoT Real-Time Dashboard</h1>

<p>
Device:
<span id="device">
--
</span>
</p>

<p id="status">
Connecting...
</p>


<div class="card">

<h2>Temperature</h2>

<div
    id="temperature"
    class="value">
    -- °C
</div>

</div>


<div class="card">

<h2>Humidity</h2>

<div
    id="humidity"
    class="value">
    -- %
</div>

</div>


<div class="card">

<h2>Light</h2>

<div
    id="light"
    class="value">
    --
</div>

</div>


<div class="chart-box">

<h2>Temperature History</h2>

<canvas
    id="temperatureChart">
</canvas>

</div>


<div class="chart-box">

<h2>Humidity History</h2>

<canvas
    id="humidityChart">
</canvas>

</div>


<div class="chart-box">

<h2>Light History</h2>

<canvas
    id="lightChart">
</canvas>

</div>

</div>


<script>

// ------------------------------------
// MQTT Configuration
// ------------------------------------

const broker =
    "wss://YOUR_BROKER_WEBSOCKET_ADDRESS";

const topic =
    "iot/lab/sensors";

const client =
    mqtt.connect(broker);


// ------------------------------------
// Maximum Number of Samples
// ------------------------------------

const MAX_POINTS = 20;


// ------------------------------------
// Temperature Chart
// ------------------------------------

const temperatureChart =
    new Chart(
        document.getElementById(
            "temperatureChart"
        ),
        {
            type: "line",

            data: {
                labels: [],

                datasets: [{
                    label:
                        "Temperature °C",

                    data: []
                }]
            },

            options: {
                animation: false,

                responsive: true,

                scales: {
                    y: {
                        beginAtZero: false
                    }
                }
            }
        }
    );


// ------------------------------------
// Humidity Chart
// ------------------------------------

const humidityChart =
    new Chart(
        document.getElementById(
            "humidityChart"
        ),
        {
            type: "line",

            data: {
                labels: [],

                datasets: [{
                    label:
                        "Humidity %",

                    data: []
                }]
            },

            options: {
                animation: false,

                responsive: true
            }
        }
    );


// ------------------------------------
// Light Chart
// ------------------------------------

const lightChart =
    new Chart(
        document.getElementById(
            "lightChart"
        ),
        {
            type: "line",

            data: {
                labels: [],

                datasets: [{
                    label:
                        "Light Level",

                    data: []
                }]
            },

            options: {
                animation: false,

                responsive: true
            }
        }
    );


// ------------------------------------
// Function: Add Data
// ------------------------------------

function addData(
    chart,
    label,
    value
)
{
    chart.data.labels.push(
        label
    );

    chart.data.datasets[0]
        .data.push(
            value
        );

    if (
        chart.data.labels.length >
        MAX_POINTS
    )
    {
        chart.data.labels.shift();

        chart.data.datasets[0]
            .data.shift();
    }

    chart.update();
}


// ------------------------------------
// MQTT Connected
// ------------------------------------

client.on(
    "connect",
    function()
    {
        document.getElementById(
            "status"
        ).innerText =
            "MQTT Connected";

        client.subscribe(topic);
    }
);


// ------------------------------------
// MQTT Message
// ------------------------------------

client.on(
    "message",
    function(topic, message)
    {
        try
        {
            const data =
                JSON.parse(
                    message.toString()
                );

            const time =
                new Date()
                    .toLocaleTimeString();

            document.getElementById(
                "device"
            ).innerText =
                data.device;

            document.getElementById(
                "temperature"
            ).innerText =
                data.temperature +
                " °C";

            document.getElementById(
                "humidity"
            ).innerText =
                data.humidity +
                " %";

            document.getElementById(
                "light"
            ).innerText =
                data.light;


            addData(
                temperatureChart,
                time,
                data.temperature
            );

            addData(
                humidityChart,
                time,
                data.humidity
            );

            addData(
                lightChart,
                time,
                data.light
            );
        }

        catch(error)
        {
            console.log(
                "JSON Error:",
                error
            );
        }
    }
);


// ------------------------------------
// MQTT Error
// ------------------------------------

client.on(
    "error",
    function(error)
    {
        document.getElementById(
            "status"
        ).innerText =
            "MQTT Error";

        console.log(error);
    }
);

</script>

</body>

</html>
```

------------------------------------------------------------------------

# Part C --- Data Buffering

## 10. Why Limit the Number of Samples?

If the dashboard continuously adds values:

``` text
1
2
3
4
5
...
1000
...
100000
```

the browser must store and render an increasingly large amount of data.

This can reduce performance.

Therefore, this laboratory uses:

``` javascript
const MAX_POINTS = 20;
```

Only the most recent 20 samples are displayed.

The logic is:

``` text
New Sample
    ↓
Add to Chart
    ↓
More Than 20?
   / \
 Yes  No
  │    │
Remove │
Oldest │
  │    │
  └────┘
    ↓
Update Chart
```

------------------------------------------------------------------------

# 11. Sliding Window

This approach is called a **sliding window**.

For example, suppose:

``` text
MAX_POINTS = 5
```

After five measurements:

``` text
[28.0, 28.1, 28.3, 28.4, 28.5]
```

a new value arrives:

``` text
28.7
```

The oldest value is removed:

``` text
[28.1, 28.3, 28.4, 28.5, 28.7]
```

The dashboard therefore always shows recent behavior.

------------------------------------------------------------------------

# Part D --- Experiments

## 12. Experiment 1 --- Temperature Chart

Run the ESP32 and dashboard.

Observe the temperature chart for at least 20 samples.

Record:

    Sample   Temperature
  -------- -------------
         1 
         2 
         3 
         4 
         5 

Describe whether the temperature appears:

``` text
Stable
Increasing
Decreasing
Fluctuating
```

------------------------------------------------------------------------

# 13. Experiment 2 --- Publish Interval

Test the following MQTT publish intervals:

    Test     Interval
  ------ ------------
       A     1 second
       B    2 seconds
       C    5 seconds
       D   10 seconds

Observe how quickly the charts change.

Discuss:

-   responsiveness;
-   visual smoothness;
-   network traffic;
-   number of messages.

------------------------------------------------------------------------

# 14. Experiment 3 --- Maximum Chart Points

Change:

``` javascript
const MAX_POINTS = 20;
```

to:

``` javascript
const MAX_POINTS = 5;
```

Then try:

``` javascript
const MAX_POINTS = 50;
```

Compare the results.

    MAX_POINTS Observation
  ------------ -------------
             5 
            20 
            50 

Discuss the trade-off between:

``` text
Short History
vs.
Long History
```

------------------------------------------------------------------------

# 15. Experiment 4 --- Light Variation

If using a physical light sensor, cover and uncover it.

If using simulated values, change the simulation range.

Observe:

``` text
Light Level
    ↓
MQTT JSON
    ↓
Dashboard
    ↓
Real-Time Chart
```

Describe the relationship between the physical or simulated event and
the chart.

------------------------------------------------------------------------

# 16. Experiment 5 --- Sudden Sensor Change

Create a sudden temperature change.

For simulation:

``` cpp
float temperature;

if (
    sequence < 20
)
{
    temperature = 25.0;
}
else
{
    temperature = 32.0;
}
```

Observe the chart.

Expected pattern:

``` text
32 ┤             ──────
30 ┤
28 ┤
26 ┤────────────
24 ┤
   └──────────────────
         Time →
```

Discuss why trend visualization can reveal events more clearly than a
single numeric display.

------------------------------------------------------------------------

# Part E --- Timestamps

## 17. Browser Timestamp

The current example generates timestamps in the browser:

``` javascript
new Date().toLocaleTimeString()
```

This represents the time when the browser receives the message.

The data flow is:

``` text
ESP32
  ↓
MQTT
  ↓
Network Delay
  ↓
Browser Receives Message
  ↓
Timestamp Generated
```

Therefore, it is not necessarily the exact sensor measurement time.

------------------------------------------------------------------------

# 18. Device Timestamp

A better data record may contain its own timestamp:

``` json
{
  "device": "ESP32-01",
  "timestamp": 1720000000,
  "temperature": 28.5,
  "humidity": 72,
  "light": 640
}
```

Then:

``` text
Sensor Measurement
      ↓
Timestamp Added
      ↓
MQTT Transmission
      ↓
Dashboard
```

This approach will become more important when storing data in databases.

------------------------------------------------------------------------

# Part F --- Visualization Design

## 19. Current Value vs. Trend

A good dashboard often contains both.

Current value:

``` text
Temperature
   28.5 °C
```

Trend:

``` text
29 ┤        ●
28 ┤   ●──●
27 ┤ ●
   └────────────
```

The current value answers:

> What is happening now?

The trend answers:

> How has the value been changing?

Both forms of information are useful.

------------------------------------------------------------------------

# 20. Different Chart Types

Chart.js supports several chart types.

Examples include:

``` text
Line Chart
Bar Chart
Scatter Plot
Pie Chart
Doughnut Chart
```

For continuous IoT sensor measurements, a **line chart** is usually
appropriate because the horizontal axis represents time.

------------------------------------------------------------------------

# 21. Dashboard Data Flow

The browser performs:

``` text
MQTT Message
     ↓
message.toString()
     ↓
JSON.parse()
     ↓
Extract Data
     ↓
Update Numeric Display
     ↓
Append Chart Data
     ↓
Remove Old Sample
     ↓
chart.update()
```

This process repeats whenever a new MQTT message arrives.

------------------------------------------------------------------------

# Part G --- Short-Term vs. Historical Data

## 22. Browser Memory

In this laboratory, sensor history exists only in the web browser.

For example:

``` text
MQTT Data
   ↓
JavaScript Array
   ↓
Chart.js
```

If the page is refreshed:

``` text
Refresh Page
    ↓
JavaScript Memory Cleared
    ↓
Chart History Lost
```

Therefore, this system provides only **temporary history**.

------------------------------------------------------------------------

# 23. Permanent History

A real IoT platform may use:

``` text
ESP32
  ↓
MQTT Broker
  ↓
Database
  ↓
Historical Storage
  ↓
Dashboard
```

Possible database systems include:

``` text
InfluxDB
MySQL
PostgreSQL
Cloud Database
```

This will be introduced in a later laboratory.

------------------------------------------------------------------------

# Part H --- Multiple Devices

## 24. Visualizing Multiple ESP32 Devices

Suppose two devices publish:

``` text
iot/lab/device01/sensors

iot/lab/device02/sensors
```

The dashboard may subscribe to:

``` text
iot/lab/+/sensors
```

The resulting visualization could display:

``` text
Temperature

30 ┤      Device 2
29 ┤    ─────────
28 ┤ Device 1
27 ┤─────────────
   └────────────────
          Time
```

This introduces multi-device monitoring.

------------------------------------------------------------------------

# Part I --- Data Quality

## 25. Sensor Noise

Real sensors usually contain measurement noise.

Example:

``` text
28.3
28.5
28.2
28.6
28.4
28.5
```

A graph makes this variation easier to observe.

Later laboratories may introduce techniques such as:

``` text
Moving Average
Median Filter
Threshold Detection
Anomaly Detection
Machine Learning
```

------------------------------------------------------------------------

# 26. Missing Data

Suppose the network temporarily fails.

The measurements may appear as:

``` text
28.1
28.2
28.4
      missing
      missing
28.7
28.8
```

Questions include:

-   Should the chart draw a line across the missing interval?
-   Should the missing value be marked?
-   How can the dashboard detect missing messages?

Sequence numbers introduced in Lab 3 can help detect lost messages.

------------------------------------------------------------------------

# Part J --- Security and Reliability

## 27. MQTT Security

As in previous laboratories, do not place privileged MQTT credentials in
public dashboard code.

A production IoT system should consider:

``` text
TLS
Authentication
Topic Permissions
Input Validation
Device Identity
Secure APIs
```

------------------------------------------------------------------------

# 28. Validate Sensor Data

Before adding a value to a graph, verify that it is valid.

For example:

``` javascript
if (
    typeof data.temperature ===
        "number"
)
{
    addData(
        temperatureChart,
        time,
        data.temperature
    );
}
```

This prevents invalid messages from damaging the visualization.

------------------------------------------------------------------------

# Part K --- Review Questions

## 29. Questions

1.  Why are graphs useful in IoT dashboards?

2.  What is the role of Chart.js?

3.  What is a sliding window?

4.  Why should the number of chart points be limited?

5.  What happens if `MAX_POINTS` is very large?

6.  Why does the dashboard use a timestamp for each sample?

7.  What is the difference between measurement time and browser
    reception time?

8.  Why is browser memory not suitable for permanent historical storage?

9.  What happens to chart data when the page is refreshed?

10. Why is a line chart suitable for sensor data?

11. How can a graph reveal sudden changes?

12. How could multiple devices be displayed on one chart?

13. How can sequence numbers help identify missing messages?

14. Why should sensor values be validated before visualization?

15. What additional system component is needed for permanent history?

------------------------------------------------------------------------

# Part L --- Student Assignment

## 30. Required Implementation

Develop a real-time IoT dashboard with:

``` text
ESP32
  ↓
JSON
  ↓
MQTT Broker
  ↓
GitHub.io
  ↓
Chart.js
```

The dashboard must display:

-   device ID;
-   MQTT status;
-   current temperature;
-   current humidity;
-   current light value;
-   temperature history chart;
-   humidity history chart;
-   light history chart.

The dashboard must keep at least:

``` text
20 recent samples
```

for each sensor.

------------------------------------------------------------------------

# 31. Extension Task

Add:

``` text
Minimum Temperature
Maximum Temperature
Average Temperature
```

For the displayed samples.

For example:

``` text
Temperature

Current: 28.5 °C
Minimum: 27.9 °C
Maximum: 29.2 °C
Average: 28.4 °C
```

The average can be calculated using:

``` text
Average =
Sum of Samples
──────────────
Number of Samples
```

------------------------------------------------------------------------

# 32. Advanced Extension

Add an alert threshold.

For example:

``` text
If Temperature > 30 °C

Status:
HIGH TEMPERATURE
```

Conceptually:

``` text
MQTT Data
   ↓
Temperature > 30?
   / \
 Yes  No
  │    │
Alert Normal
```

This prepares the system for automated IoT event detection.

------------------------------------------------------------------------

# Part M --- Laboratory Report

## 33. Required Report

The report should contain:

### 1. Objective

Explain why real-time visualization is important in IoT systems.

### 2. Architecture

Draw:

``` text
Sensors
   ↓
ESP32
   ↓
MQTT
   ↓
GitHub.io
   ↓
Chart.js
```

### 3. JSON Format

Show the JSON message used.

### 4. Dashboard Design

Describe:

-   numeric cards;
-   real-time charts;
-   MQTT connection;
-   sample buffer.

### 5. Data Buffer

Explain the purpose of `MAX_POINTS`.

### 6. Experimental Results

Provide screenshots showing:

-   live temperature chart;
-   live humidity chart;
-   live light chart;
-   current values.

### 7. Publish Rate Analysis

Compare at least two MQTT publishing intervals.

### 8. Discussion

Discuss:

-   short-term history;
-   chart responsiveness;
-   memory limitations;
-   network traffic;
-   data loss;
-   timestamps.

### 9. Conclusion

Summarize the role of visualization in an IoT monitoring system.

------------------------------------------------------------------------

# 34. Expected Result

At the end of Lab 4, the system should resemble:

``` text
┌───────────────────────┐
│        ESP32          │
│                       │
│ Temperature           │
│ Humidity              │
│ Light                 │
└───────────┬───────────┘
            │
            │ JSON / MQTT
            ▼
┌───────────────────────┐
│      MQTT Broker      │
└───────────┬───────────┘
            │
            │ WSS
            ▼
┌──────────────────────────────┐
│        GitHub.io            │
│                              │
│ Temp: 28.5 °C                │
│ Humidity: 72 %               │
│ Light: 640                   │
│                              │
│ Temperature                  │
│  ─────╱╲────╱────            │
│                              │
│ Humidity                     │
│  ──╲────╱────────            │
│                              │
│ Light                        │
│  ─╱╲╱╲──────                 │
└──────────────────────────────┘
```

------------------------------------------------------------------------

# 35. Progression from Lab 1 to Lab 4

The laboratory sequence now develops as:

``` text
Lab 1
Basic Monitoring
ESP32 ───► Dashboard

        ↓

Lab 2
Monitoring + Control
ESP32 ◄──► Dashboard

        ↓

Lab 3
Structured Data
Sensors → JSON → MQTT → Dashboard

        ↓

Lab 4
Visualization
Sensors → JSON → MQTT → Live Charts
```

Each laboratory adds one important layer to the complete IoT system.

------------------------------------------------------------------------

# 36. Key Concept

Lab 4 introduces the transition from:

``` text
DATA
```

to:

``` text
INFORMATION
```

A single value provides the current state:

``` text
Temperature = 28.5 °C
```

A graph provides context:

``` text
Temperature
    ↓
Change over time
    ↓
Trend
    ↓
Potential Event
```

Therefore, visualization helps users interpret IoT data and identify
meaningful patterns.

------------------------------------------------------------------------

# 37. Summary

In this laboratory, students extended the structured MQTT system from
Lab 3 to support real-time sensor visualization.

The ESP32 publishes JSON sensor data through MQTT. The GitHub.io
dashboard receives the data using MQTT over secure WebSockets, parses
the JSON payload, and updates both numerical indicators and Chart.js
line charts.

A sliding window stores only recent samples, allowing the browser to
provide lightweight short-term history without requiring a database.

The complete architecture is:

**Sensors → ESP32 → JSON → MQTT → Broker → WSS → GitHub.io → Chart.js**

Lab 4 therefore provides the foundation for the next stage: **persistent
IoT data storage and historical analysis**, where sensor information
will be saved in a database rather than disappearing when the dashboard
is refreshed.
