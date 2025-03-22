// Import required modules
const express = require("express");
const axios = require("axios");
const redis = require("redis");

// Define constants
const url = "https://jsonplaceholder.typicode.com/photos";
const REDIS_URL = "redis://localhost:6379";

const redisClient = redis.createClient({ url: REDIS_URL });

redisClient
  .connect()
  .then(() => {
    console.log("Connected to Redis");
  })
  .catch((err) => {
    console.error("Failed to connect to Redis:", err);
  });

const app = express();

app.get("/", (req, res) => {
  res.send("Hello, World!");
});

app.get("/fetch_net", async (req, res) => {
  try {
    const response = await axios.get(url);
    const data = response.data;
    console.log(data);
    res.status(200).send(data);
  } catch (error) {
    console.error("Error fetching data:", error.message);
    res.status(500).json({ error: "Error fetching data" });
  }
});

app.get("/fetch_cash", async (req, res) => {
  try {
    const cacheKey = "photos";

    const cachedData = await redisClient.get(cacheKey);
    if (cachedData) {
      console.log("Data retrieved from Redis cache");
      return res.status(200).json(JSON.parse(cachedData));
    } else {
      console.log("Fetching data from the internet...");
      const response = await axios.get(url);
      const data = response.data;

      await redisClient.setEx(cacheKey, 3600, JSON.stringify(data));

      return res.status(200).json(data);
    }
  } catch (error) {
    console.error("Error fetching data:", error.message);
    return res.status(500).json({ error: "Error fetching data" });
  }
});

const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
});
